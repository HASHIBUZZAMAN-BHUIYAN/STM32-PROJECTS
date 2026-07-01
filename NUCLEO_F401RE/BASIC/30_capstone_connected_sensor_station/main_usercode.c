// ============================================================
// Nucleo-F401RE BASIC/30 — Capstone: Connected Sensor Station
// STM32F401RETx  |  HAL + CubeMX 6.16  |  84 MHz
// Peripherals: I2C1, USART2, TIM3, GPIO PA8
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
#include <string.h>
#include <math.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
// BMP280 register addresses
#define BMP280_ADDR       (0x76 << 1)
#define BMP280_REG_ID     0xD0
#define BMP280_REG_CTRL   0xF4
#define BMP280_REG_CONF   0xF5
#define BMP280_REG_PMSB   0xF7
#define BMP280_CALIB_BASE 0x88

// SSD1306 I2C address
#define OLED_ADDR         (0x3C << 1)

// DHT11 GPIO
#define DHT_PORT          GPIOA
#define DHT_PIN           GPIO_PIN_8
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
static char buf[160];
static volatile uint8_t uart_rx_byte;
static char cmd_buf[32];
static uint8_t cmd_idx = 0;
static volatile uint8_t cmd_ready = 0;
static uint32_t last_sample_tick = 0;

// BMP280 trimming params
static uint16_t dig_T1; static int16_t dig_T2, dig_T3;
static uint16_t dig_P1; static int16_t dig_P2,dig_P3,dig_P4,dig_P5,dig_P6,dig_P7,dig_P8,dig_P9;

// Sensor readings
static float bmp_temp = 0.0f;
static int32_t bmp_press = 0;
static int8_t  dht_temp = 0;
static uint8_t dht_hum  = 0;
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====

/* ---- TIM3 µs delay (PSC=83, ARR=65535 → 1 MHz tick at 84 MHz) ---- */
static void delay_us(uint16_t us) {
    __HAL_TIM_SET_COUNTER(&htim3, 0);
    while (__HAL_TIM_GET_COUNTER(&htim3) < us);
}

/* ---- BMP280 helpers ---- */
static HAL_StatusTypeDef bmp_write(uint8_t reg, uint8_t val) {
    uint8_t d[2] = {reg, val};
    return HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, d, 2, 10);
}
static HAL_StatusTypeDef bmp_read(uint8_t reg, uint8_t *dst, uint8_t len) {
    if (HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, &reg, 1, 10) != HAL_OK) return HAL_ERROR;
    return HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, dst, len, 10);
}

static void bmp280_init(void) {
    uint8_t calib[24];
    bmp_read(BMP280_CALIB_BASE, calib, 24);
    dig_T1 = (uint16_t)(calib[1]<<8|calib[0]);
    dig_T2 = (int16_t)(calib[3]<<8|calib[2]);
    dig_T3 = (int16_t)(calib[5]<<8|calib[4]);
    dig_P1 = (uint16_t)(calib[7]<<8|calib[6]);
    dig_P2 = (int16_t)(calib[9]<<8|calib[8]);
    dig_P3 = (int16_t)(calib[11]<<8|calib[10]);
    dig_P4 = (int16_t)(calib[13]<<8|calib[12]);
    dig_P5 = (int16_t)(calib[15]<<8|calib[14]);
    dig_P6 = (int16_t)(calib[17]<<8|calib[16]);
    dig_P7 = (int16_t)(calib[19]<<8|calib[18]);
    dig_P8 = (int16_t)(calib[21]<<8|calib[20]);
    dig_P9 = (int16_t)(calib[23]<<8|calib[22]);
    // Normal mode, osrs_t=2x, osrs_p=16x, filter=4
    bmp_write(BMP280_REG_CONF, 0x10);
    bmp_write(BMP280_REG_CTRL, 0x57);
}

static void bmp280_read(void) {
    uint8_t raw[6];
    bmp_read(BMP280_REG_PMSB, raw, 6);
    int32_t adc_P = (int32_t)((raw[0]<<12)|(raw[1]<<4)|(raw[2]>>4));
    int32_t adc_T = (int32_t)((raw[3]<<12)|(raw[4]<<4)|(raw[5]>>4));

    // BMP280 compensate temperature
    int32_t var1 = ((((adc_T>>3) - ((int32_t)dig_T1<<1))) * dig_T2) >> 11;
    int32_t var2 = (((((adc_T>>4)-((int32_t)dig_T1))*((adc_T>>4)-((int32_t)dig_T1)))>>12)*dig_T3)>>14;
    int32_t t_fine = var1 + var2;
    bmp_temp = (float)((t_fine*5+128)>>8) / 100.0f;

    // Compensate pressure
    int64_t p1 = ((int64_t)t_fine) - 128000;
    int64_t p2 = p1*p1*(int64_t)dig_P6;
    p2 = p2 + ((p1*(int64_t)dig_P5)<<17);
    p2 = p2 + (((int64_t)dig_P4)<<35);
    p1 = ((p1*p1*(int64_t)dig_P3)>>8) + ((p1*(int64_t)dig_P2)<<12);
    p1 = (((((int64_t)1)<<47)+p1)*(int64_t)dig_P1)>>33;
    if (p1 == 0) { bmp_press = 0; return; }
    int64_t pp = 1048576 - adc_P;
    pp = (((pp<<31) - p2)*3125)/p1;
    p1 = (((int64_t)dig_P9)*(pp>>13)*(pp>>13))>>25;
    p2 = (((int64_t)dig_P8)*pp)>>19;
    pp = ((pp+p1+p2)>>8) + (((int64_t)dig_P7)<<4);
    bmp_press = (int32_t)(pp >> 8);
}

/* ---- SSD1306 minimal driver ---- */
static void oled_cmd(uint8_t c) {
    uint8_t d[2] = {0x00, c};
    HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDR, d, 2, 10);
}
static void oled_init(void) {
    HAL_Delay(100);
    const uint8_t cmds[] = {
        0xAE,0x20,0x00,0xB0,0xC8,0x00,0x10,0x40,
        0x81,0xCF,0xA1,0xA6,0xA8,0x3F,0xA4,0xD3,
        0x00,0xD5,0xF0,0xD9,0x22,0xDA,0x12,0xDB,
        0x20,0x8D,0x14,0xAF
    };
    for (uint8_t i = 0; i < sizeof(cmds); i++) oled_cmd(cmds[i]);
}
static void oled_set_pos(uint8_t page, uint8_t col) {
    oled_cmd(0xB0 | page);
    oled_cmd(0x00 | (col & 0x0F));
    oled_cmd(0x10 | (col >> 4));
}
static const uint8_t font5x8[][5] = {
    {0x00,0x00,0x00,0x00,0x00}, // ' '
    {0x3E,0x51,0x49,0x45,0x3E}, // '0'
    {0x00,0x42,0x7F,0x40,0x00}, // '1'
    {0x42,0x61,0x51,0x49,0x46}, // '2'
    {0x21,0x41,0x45,0x4B,0x31}, // '3'
    {0x18,0x14,0x12,0x7F,0x10}, // '4'
    {0x27,0x45,0x45,0x45,0x39}, // '5'
    {0x3C,0x4A,0x49,0x49,0x30}, // '6'
    {0x01,0x71,0x09,0x05,0x03}, // '7'
    {0x36,0x49,0x49,0x49,0x36}, // '8'
    {0x06,0x49,0x49,0x29,0x1E}, // '9'
    {0x00,0x36,0x36,0x00,0x00}, // ':'
    {0x08,0x1C,0x2A,0x08,0x08}, // 'T' approx — not full charset, demo only
};
static void oled_char(char ch) {
    uint8_t idx = 0;
    if (ch >= '0' && ch <= '9') idx = 1 + (ch - '0');
    else if (ch == ':') idx = 11;
    uint8_t d[6] = {0x40,0,0,0,0,0};
    if (idx < 13) memcpy(d+1, font5x8[idx], 5);
    HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDR, d, 6, 10);
}
static void oled_string(const char *s) {
    while (*s) oled_char(*s++);
}
static void oled_clear(void) {
    for (uint8_t page = 0; page < 8; page++) {
        oled_set_pos(page, 0);
        uint8_t blank[129]; blank[0] = 0x40;
        memset(blank+1, 0, 128);
        HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDR, blank, 129, 50);
    }
}

/* ---- DHT11 bit-bang ---- */
static void dht_pin_output(void) {
    GPIO_InitTypeDef g = {0};
    g.Pin = DHT_PIN; g.Mode = GPIO_MODE_OUTPUT_PP; g.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(DHT_PORT, &g);
}
static void dht_pin_input(void) {
    GPIO_InitTypeDef g = {0};
    g.Pin = DHT_PIN; g.Mode = GPIO_MODE_INPUT; g.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(DHT_PORT, &g);
}
static uint8_t dht11_read(void) {
    uint8_t bytes[5] = {0};
    // Send start: pull low 18ms, then high
    dht_pin_output();
    HAL_GPIO_WritePin(DHT_PORT, DHT_PIN, GPIO_PIN_RESET);
    HAL_Delay(18);
    HAL_GPIO_WritePin(DHT_PORT, DHT_PIN, GPIO_PIN_SET);
    delay_us(40);
    dht_pin_input();

    // Wait for DHT11 response: low 80µs + high 80µs
    uint32_t t = HAL_GetTick();
    while (HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN) == GPIO_PIN_SET) {
        if (HAL_GetTick() - t > 2) return 0; // timeout
    }
    while (HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN) == GPIO_PIN_RESET) {
        if (HAL_GetTick() - t > 2) return 0;
    }
    while (HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN) == GPIO_PIN_SET) {
        if (HAL_GetTick() - t > 2) return 0;
    }

    // Read 40 bits
    for (uint8_t i = 0; i < 40; i++) {
        while (HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN) == GPIO_PIN_RESET); // wait low->high
        delay_us(35); // sample at 35µs: bit=1 if still high, bit=0 if gone low
        bytes[i/8] <<= 1;
        if (HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN)) bytes[i/8] |= 1;
        while (HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN) == GPIO_PIN_SET); // wait for next low
    }
    // Checksum
    if (bytes[4] == (uint8_t)(bytes[0]+bytes[1]+bytes[2]+bytes[3])) {
        dht_hum  = bytes[0];
        dht_temp = (int8_t)bytes[2];
        return 1;
    }
    return 0;
}

static void process_cmd(const char *cmd) {
    if (strncmp(cmd, "read", 4) == 0) {
        bmp280_read();
        dht11_read();
        int n = snprintf(buf, sizeof(buf),
            "{\"t1\":%.1f,\"p\":%ld,\"t2\":%d,\"h\":%u,\"tick\":%lu}\r\n",
            (double)bmp_temp, bmp_press, dht_temp, dht_hum, HAL_GetTick());
        HAL_UART_Transmit(&huart2, (uint8_t*)buf, n, 200);
    } else if (strncmp(cmd, "info", 4) == 0) {
        const char *info = "Nucleo-F401RE | 84MHz | BMP280+DHT11+SSD1306\r\n";
        HAL_UART_Transmit(&huart2, (uint8_t*)info, strlen(info), 200);
    }
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    HAL_TIM_Base_Start(&htim3);
    bmp280_init();
    oled_init();
    oled_clear();

    oled_set_pos(0, 0);
    oled_string("SENSOR STATION");

    HAL_UART_Receive_IT(&huart2, &uart_rx_byte, 1);
    const char *banner = "Nucleo Sensor Station ready. Commands: read, info\r\n";
    HAL_UART_Transmit(&huart2, (uint8_t*)banner, strlen(banner), 200);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        uint32_t now = HAL_GetTick();

        // Sample every 5 seconds
        if (now - last_sample_tick >= 5000) {
            last_sample_tick = now;
            bmp280_read();
            dht11_read();

            // Transmit JSON
            int n = snprintf(buf, sizeof(buf),
                "{\"t1\":%.1f,\"p\":%ld,\"t2\":%d,\"h\":%u,\"tick\":%lu}\r\n",
                (double)bmp_temp, bmp_press, dht_temp, dht_hum, now);
            HAL_UART_Transmit(&huart2, (uint8_t*)buf, n, 200);

            // Update OLED
            oled_clear();
            oled_set_pos(0, 0); oled_string("SENSOR STATION");

            char line[24];
            snprintf(line, sizeof(line), "BMP:%.1fC %ldhPa",
                (double)bmp_temp, bmp_press/100L);
            oled_set_pos(2, 0); oled_string(line);

            snprintf(line, sizeof(line), "DHT:%dC %u%%", dht_temp, dht_hum);
            oled_set_pos(4, 0); oled_string(line);

            snprintf(line, sizeof(line), "UP:%lus", now/1000UL);
            oled_set_pos(6, 0); oled_string(line);
        }

        // Handle UART command
        if (cmd_ready) {
            cmd_ready = 0;
            process_cmd(cmd_buf);
            cmd_idx = 0;
            memset(cmd_buf, 0, sizeof(cmd_buf));
        }

        // Heartbeat LED every 1 second
        static uint32_t led_tick = 0;
        if (now - led_tick >= 1000) {
            led_tick = now;
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        }
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====

// ===== USER CODE BEGIN 4 =====
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        char c = (char)uart_rx_byte;
        if (c == '\r' || c == '\n') {
            cmd_buf[cmd_idx] = '\0';
            if (cmd_idx > 0) cmd_ready = 1;
        } else if (cmd_idx < sizeof(cmd_buf) - 1) {
            cmd_buf[cmd_idx++] = c;
        }
        HAL_UART_Receive_IT(&huart2, &uart_rx_byte, 1);
    }
}
// ===== USER CODE END 4 =====
