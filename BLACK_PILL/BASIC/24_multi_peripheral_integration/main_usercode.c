// ============================================================
// Black Pill BASIC/24 — Multi-Peripheral Integration
// STM32F411CEU6  |  HAL + CubeMX 6.16  |  100 MHz
// I2C1: BMP280(0x76) + SSD1306(0x3C) shared, ADC1 PA1, USART1
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
#define BMP_ADDR   (0x76 << 1)
#define OLED_ADDR  (0x3C << 1)
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
static float bmp_temp = 0; static int32_t bmp_press = 0;
static uint16_t adc_raw = 0;
static char buf[80];
// BMP280 trim (T only for brevity)
static uint16_t dT1; static int16_t dT2, dT3;
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static void uart_tx(const char *s) {
    HAL_UART_Transmit(&huart1, (const uint8_t*)s, strlen(s), 200);
}

static void bmp_rd(uint8_t reg, uint8_t *dst, uint8_t len) {
    HAL_I2C_Master_Transmit(&hi2c1, BMP_ADDR, &reg, 1, 10);
    HAL_I2C_Master_Receive(&hi2c1, BMP_ADDR, dst, len, 20);
}
static void bmp_wr(uint8_t reg, uint8_t val) {
    uint8_t d[2]={reg,val};
    HAL_I2C_Master_Transmit(&hi2c1, BMP_ADDR, d, 2, 10);
}
static void bmp280_init(void) {
    uint8_t c[6]; bmp_rd(0x88, c, 6);
    dT1=(uint16_t)(c[1]<<8|c[0]); dT2=(int16_t)(c[3]<<8|c[2]); dT3=(int16_t)(c[5]<<8|c[4]);
    bmp_wr(0xF4, 0x27);
}
static void bmp280_read(void) {
    uint8_t r[3]; bmp_rd(0xFA, r, 3);
    int32_t adc_T=(int32_t)((r[0]<<12)|(r[1]<<4)|(r[2]>>4));
    int32_t v1=((((adc_T>>3)-((int32_t)dT1<<1)))*dT2)>>11;
    int32_t v2=(((((adc_T>>4)-(int32_t)dT1)*((adc_T>>4)-(int32_t)dT1))>>12)*dT3)>>14;
    int32_t tf=v1+v2;
    bmp_temp=(float)((tf*5+128)>>8)/100.0f;
    bmp_press=101325; // pressure reading omitted for brevity (use full driver)
}

static void oled_cmd(uint8_t c) {
    uint8_t d[2]={0x00,c}; HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDR, d, 2, 10);
}
static void oled_init(void) {
    HAL_Delay(100);
    const uint8_t cmds[]={0xAE,0x20,0x00,0xB0,0xC8,0x00,0x10,0x40,
        0x81,0xCF,0xA1,0xA6,0xA8,0x3F,0xA4,0xD3,0x00,0xD5,0xF0,
        0xD9,0x22,0xDA,0x12,0xDB,0x20,0x8D,0x14,0xAF};
    for (uint8_t i=0;i<sizeof(cmds);i++) oled_cmd(cmds[i]);
}
static void oled_set_pos(uint8_t page, uint8_t col) {
    oled_cmd(0xB0|page); oled_cmd(0x00|(col&0x0F)); oled_cmd(0x10|(col>>4));
}
static void oled_write_text(uint8_t page, const char *s) {
    oled_set_pos(page, 0);
    // Send text as raw byte data (simplified, not full font)
    // Each char maps to its ASCII code — OLED will show blocks but demonstrates the call pattern
    uint8_t line[129]; line[0]=0x40;
    uint8_t len=strlen(s); if(len>21) len=21;
    for(uint8_t i=0;i<len;i++) line[1+i*6]=0x7F; // simplified: solid block per char
    memset(line+1+len*6,0,128-(len*6));
    HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDR, line, 1+128, 50);
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    bmp280_init();
    oled_init();
    uart_tx("Black Pill multi-peripheral demo\r\n");
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    uint32_t t_bmp=0, t_oled=0, t_adc=0, t_uart=0, t_led=0;
    while (1)
    {
        uint32_t now = HAL_GetTick();

        if (now - t_bmp >= 2000) {
            t_bmp = now;
            bmp280_read();
        }
        if (now - t_adc >= 500) {
            t_adc = now;
            HAL_ADC_Start(&hadc1);
            if (HAL_ADC_PollForConversion(&hadc1, 5) == HAL_OK)
                adc_raw = (uint16_t)HAL_ADC_GetValue(&hadc1);
            HAL_ADC_Stop(&hadc1);
        }
        if (now - t_oled >= 200) {
            t_oled = now;
            snprintf(buf, sizeof(buf), "T=%.1fC ADC=%u", (double)bmp_temp, adc_raw);
            oled_write_text(0, buf);
            snprintf(buf, sizeof(buf), "P=%ldhPa", bmp_press/100L);
            oled_write_text(2, buf);
        }
        if (now - t_uart >= 5000) {
            t_uart = now;
            int n = snprintf(buf, sizeof(buf),
                "T=%.1fC P=%ldhPa ADC=%u(%lumV) tick=%lu\r\n",
                (double)bmp_temp, bmp_press/100L,
                adc_raw, ((uint32_t)adc_raw*3300U)/4096U, now);
            uart_tx(buf);
            (void)n;
        }
        if (now - t_led >= 1000) {
            t_led = now;
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        }
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====
