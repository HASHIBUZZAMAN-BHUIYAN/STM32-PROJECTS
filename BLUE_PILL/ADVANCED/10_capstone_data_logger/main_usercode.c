// ============================================================
// Blue Pill ADVANCED/10 — Capstone: FreeRTOS Data Logger
// STM32F103C8T6  |  HAL + FreeRTOS CMSIS-V2 + FatFs
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include "cmsis_os.h"
#include "fatfs.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
#define BMP280_ADDR      (0x76 << 1)
#define OLED_ADDR        (0x3C << 1)
#define SD_CS_PIN        GPIO_PIN_12
#define SD_CS_PORT       GPIOB
#define LOG_FILENAME     "LOG.TXT"
#define WDT_TASK_COUNT   4
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
osThreadId_t sensorTaskHandle;
osThreadId_t logTaskHandle;
osThreadId_t displayTaskHandle;
osThreadId_t cmdTaskHandle;

osMutexId_t  i2cMutex;
osMessageQueueId_t sensorQueue;

typedef struct {
    uint32_t tick;
    int32_t  temp_cdeg;    // temperature in 0.01°C
    int32_t  pressure_pa;  // pressure in Pa
} SensorData_t;

static volatile uint8_t logging_active = 0;
static volatile uint32_t log_count = 0;
static volatile uint8_t  wdt_flags = 0;   // bit per task

static char uart_rx_byte;
static char cmd_buf[32];
static uint8_t cmd_idx = 0;
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====

/* ---- BMP280 helpers ---- */
static void bmp280_write(uint8_t reg, uint8_t val) {
    uint8_t buf[2] = {reg, val};
    HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, buf, 2, 10);
}
static void bmp280_read(uint8_t reg, uint8_t *data, uint16_t len) {
    HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, &reg, 1, 10);
    HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, data, len, 20);
}
static void bmp280_init(void) {
    bmp280_write(0xF4, 0x57); // forced mode, osrs_t=x2, osrs_p=x2
    bmp280_write(0xF5, 0x00);
}
static void bmp280_read_data(SensorData_t *d) {
    bmp280_write(0xF4, 0x57); // trigger forced mode
    HAL_Delay(10);
    uint8_t raw[6];
    bmp280_read(0xF7, raw, 6);
    int32_t adc_P = ((int32_t)raw[0]<<12)|((int32_t)raw[1]<<4)|(raw[2]>>4);
    int32_t adc_T = ((int32_t)raw[3]<<12)|((int32_t)raw[4]<<4)|(raw[5]>>4);
    d->temp_cdeg  = (adc_T * 100) / 5243; // rough, no calibration
    d->pressure_pa = adc_P / 16;           // rough
    d->tick = HAL_GetTick();
}

/* ---- SSD1306 minimal ---- */
static void oled_cmd(uint8_t cmd) {
    uint8_t buf[2] = {0x00, cmd};
    HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDR, buf, 2, 10);
}
static void oled_init(void) {
    const uint8_t init_seq[] = {
        0xAE,0xD5,0x80,0xA8,0x3F,0xD3,0x00,0x40,0x8D,0x14,
        0x20,0x00,0xA1,0xC8,0xDA,0x12,0x81,0xCF,0xD9,0xF1,
        0xDB,0x40,0xA4,0xA6,0xAF
    };
    for (uint8_t i = 0; i < sizeof(init_seq); i++) oled_cmd(init_seq[i]);
}
static void oled_clear(void) {
    oled_cmd(0x21); oled_cmd(0); oled_cmd(127);
    oled_cmd(0x22); oled_cmd(0); oled_cmd(7);
    uint8_t zeros[17] = {0x40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    for (int i = 0; i < 64; i++)
        HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDR, zeros, 17, 10);
}
static void oled_print_line(uint8_t row, const char *str) {
    // Minimal: just write ASCII bytes as column data (requires font — placeholder)
    oled_cmd(0xB0 | (row & 0x07));
    oled_cmd(0x00); oled_cmd(0x10);
    uint8_t buf[2] = {0x40, 0x00};
    while (*str) { buf[1] = (uint8_t)(*str++); HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDR, buf, 2, 5); }
}

/* ---- FatFs SD helpers ---- */
static FATFS fs;
static FIL   logfile;
static void sd_cs_low(void)  { HAL_GPIO_WritePin(SD_CS_PORT, SD_CS_PIN, GPIO_PIN_RESET); }
static void sd_cs_high(void) { HAL_GPIO_WritePin(SD_CS_PORT, SD_CS_PIN, GPIO_PIN_SET); }

/* ---- UART command processing ---- */
static void process_command(const char *cmd) {
    char resp[64];
    if (strncmp(cmd, "start", 5) == 0) {
        logging_active = 1;
        HAL_UART_Transmit(&huart1, (uint8_t*)"Logging started\r\n", 17, 100);
    } else if (strncmp(cmd, "stop", 4) == 0) {
        logging_active = 0;
        HAL_UART_Transmit(&huart1, (uint8_t*)"Logging stopped\r\n", 17, 100);
    } else if (strncmp(cmd, "status", 6) == 0) {
        int n = snprintf(resp, sizeof(resp), "Log=%lu Active=%d\r\n", log_count, logging_active);
        HAL_UART_Transmit(&huart1, (uint8_t*)resp, n, 100);
    } else if (strncmp(cmd, "erase", 5) == 0) {
        f_unlink(LOG_FILENAME);
        log_count = 0;
        HAL_UART_Transmit(&huart1, (uint8_t*)"Log erased\r\n", 12, 100);
    } else {
        HAL_UART_Transmit(&huart1, (uint8_t*)"?\r\n", 3, 100);
    }
}

/* ---- FreeRTOS task functions ---- */
void SensorTask(void *arg) {
    bmp280_init();
    SensorData_t data;
    for (;;) {
        osMutexAcquire(i2cMutex, osWaitForever);
        bmp280_read_data(&data);
        osMutexRelease(i2cMutex);
        osMessageQueuePut(sensorQueue, &data, 0, 0);
        wdt_flags |= (1 << 0);
        osDelay(500);
    }
}

void LogTask(void *arg) {
    f_mount(&fs, "", 1);
    SensorData_t data;
    char line[64];
    for (;;) {
        if (osMessageQueueGet(sensorQueue, &data, NULL, osWaitForever) == osOK) {
            if (logging_active) {
                if (f_open(&logfile, LOG_FILENAME, FA_OPEN_APPEND | FA_WRITE) == FR_OK) {
                    int n = snprintf(line, sizeof(line), "%lu,%ld,%ld\r\n",
                                     data.tick, data.temp_cdeg, data.pressure_pa);
                    UINT bw;
                    f_write(&logfile, line, n, &bw);
                    f_close(&logfile);
                    log_count++;
                }
            }
        }
        wdt_flags |= (1 << 1);
    }
}

void DisplayTask(void *arg) {
    osMutexAcquire(i2cMutex, osWaitForever);
    oled_init();
    oled_clear();
    osMutexRelease(i2cMutex);
    char line[24];
    static SensorData_t last = {0};
    for (;;) {
        // Peek at latest queue value (non-blocking)
        osMessageQueueGet(sensorQueue, &last, NULL, 0);
        osMutexAcquire(i2cMutex, osWaitForever);
        snprintf(line, sizeof(line), "T:%ld.%02ldC", last.temp_cdeg/100,
                 (last.temp_cdeg<0?-last.temp_cdeg:last.temp_cdeg)%100);
        oled_print_line(0, line);
        snprintf(line, sizeof(line), "Log:%lu %s", log_count, logging_active?"REC":"---");
        oled_print_line(2, line);
        osMutexRelease(i2cMutex);
        wdt_flags |= (1 << 2);
        osDelay(500);
    }
}

void CmdTask(void *arg) {
    HAL_UART_Receive_IT(&huart1, (uint8_t*)&uart_rx_byte, 1);
    for (;;) {
        // Command processing is interrupt-driven; task just refreshes WDT flag
        wdt_flags |= (1 << 3);
        // Refresh IWDG if all tasks are alive
        if (wdt_flags == 0x0F) {
            HAL_IWDG_Refresh(&hiwdg);
            wdt_flags = 0;
        }
        osDelay(100);
    }
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    // Create FreeRTOS objects
    i2cMutex    = osMutexNew(NULL);
    sensorQueue = osMessageQueueNew(4, sizeof(SensorData_t), NULL);

    const osThreadAttr_t sensorAttr  = {.name="Sensor",  .stack_size=512, .priority=osPriorityAboveNormal};
    const osThreadAttr_t logAttr     = {.name="Log",     .stack_size=512, .priority=osPriorityNormal};
    const osThreadAttr_t displayAttr = {.name="Display", .stack_size=256, .priority=osPriorityBelowNormal};
    const osThreadAttr_t cmdAttr     = {.name="Cmd",     .stack_size=256, .priority=osPriorityNormal};

    sensorTaskHandle  = osThreadNew(SensorTask,  NULL, &sensorAttr);
    logTaskHandle     = osThreadNew(LogTask,     NULL, &logAttr);
    displayTaskHandle = osThreadNew(DisplayTask, NULL, &displayAttr);
    cmdTaskHandle     = osThreadNew(CmdTask,     NULL, &cmdAttr);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    // FreeRTOS scheduler started by CubeMX-generated MX_FREERTOS_Init() call
    // This while(1) is not reached when RTOS scheduler is running
    while (1)
    {
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====

// ===== USER CODE BEGIN 4 =====
/* UART receive interrupt callback — accumulates command characters */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        if (uart_rx_byte == '\n' || uart_rx_byte == '\r') {
            if (cmd_idx > 0) {
                cmd_buf[cmd_idx] = '\0';
                process_command(cmd_buf);
                cmd_idx = 0;
            }
        } else if (cmd_idx < sizeof(cmd_buf) - 1) {
            cmd_buf[cmd_idx++] = uart_rx_byte;
        }
        HAL_UART_Receive_IT(&huart1, (uint8_t*)&uart_rx_byte, 1);
    }
}
// ===== USER CODE END 4 =====
