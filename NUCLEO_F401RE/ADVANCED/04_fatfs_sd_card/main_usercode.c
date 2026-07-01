// ============================================================
// Nucleo-F401RE ADVANCED/04 — FatFs SD Card Logging
// STM32F401RETx  |  HAL + CubeMX 6.16 + FatFs
// SPI2: PB13/PB14/PB15, CS=PB12
// BMP280 on I2C1: PB8/PB9
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include "fatfs.h"
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
#define SD_CS_PORT GPIOB
#define SD_CS_PIN  GPIO_PIN_12

// BMP280 minimal
#define BMP280_ADDR  (0x76 << 1)
#define BMP280_CTRL  0xF4
#define BMP280_PMSB  0xF7
#define BMP280_CALIB 0x88
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
static FATFS fs;
static FIL   file;
static char  line[80];
static float bmp_temp = 0.0f;

// BMP280 trim (minimal, T only)
static uint16_t dT1; static int16_t dT2, dT3;
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static void cs_low(void)  { HAL_GPIO_WritePin(SD_CS_PORT, SD_CS_PIN, GPIO_PIN_RESET); }
static void cs_high(void) { HAL_GPIO_WritePin(SD_CS_PORT, SD_CS_PIN, GPIO_PIN_SET); }

/* SPI helper for diskio layer */
uint8_t spi2_tx_rx(uint8_t data) {
    uint8_t rx;
    HAL_SPI_TransmitReceive(&hspi2, &data, &rx, 1, 10);
    return rx;
}

static void bmp_read_reg(uint8_t reg, uint8_t *dst, uint8_t len) {
    HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, &reg, 1, 10);
    HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, dst, len, 10);
}
static void bmp_write_reg(uint8_t reg, uint8_t val) {
    uint8_t d[2] = {reg, val};
    HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, d, 2, 10);
}

static void bmp280_init(void) {
    uint8_t c[6];
    bmp_read_reg(BMP280_CALIB, c, 6);
    dT1 = (uint16_t)(c[1]<<8|c[0]);
    dT2 = (int16_t)(c[3]<<8|c[2]);
    dT3 = (int16_t)(c[5]<<8|c[4]);
    bmp_write_reg(BMP280_CTRL, 0x27); // normal mode, osrs_t=1x, osrs_p=1x
}

static void bmp280_read_temp(void) {
    uint8_t raw[3];
    bmp_read_reg(BMP280_PMSB + 3, raw, 3); // temp registers at 0xFA..0xFC
    int32_t adc_T = (int32_t)((raw[0]<<12)|(raw[1]<<4)|(raw[2]>>4));
    int32_t v1 = ((((adc_T>>3)-((int32_t)dT1<<1)))*dT2)>>11;
    int32_t v2 = (((((adc_T>>4)-(int32_t)dT1)*((adc_T>>4)-(int32_t)dT1))>>12)*dT3)>>14;
    int32_t t_fine = v1 + v2;
    bmp_temp = (float)((t_fine*5+128)>>8)/100.0f;
}

static void uart_tx(const char *s) {
    HAL_UART_Transmit(&huart2, (const uint8_t*)s, strlen(s), 200);
}

static void log_to_sd(void) {
    FRESULT res = f_mount(&fs, "", 1);
    if (res != FR_OK) {
        snprintf(line, sizeof(line), "Mount fail: %d\r\n", res);
        uart_tx(line);
        return;
    }
    res = f_open(&file, "LOG.CSV", FA_WRITE | FA_OPEN_APPEND);
    if (res == FR_OK) {
        snprintf(line, sizeof(line), "%lu,%.2f\r\n", HAL_GetTick(), (double)bmp_temp);
        UINT bw;
        f_write(&file, line, strlen(line), &bw);
        f_close(&file);
        uart_tx(line);
    } else {
        snprintf(line, sizeof(line), "Open fail: %d\r\n", res);
        uart_tx(line);
    }
    f_mount(NULL, "", 0);
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    cs_high(); // SD card CS idle high
    HAL_Delay(100); // SD card power-up settle
    bmp280_init();

    // Write CSV header if file doesn't exist
    if (f_mount(&fs, "", 1) == FR_OK) {
        if (f_open(&file, "LOG.CSV", FA_WRITE | FA_CREATE_NEW) == FR_OK) {
            UINT bw;
            f_write(&file, "tick_ms,temp_c\r\n", 16, &bw);
            f_close(&file);
        }
        f_mount(NULL, "", 0);
    }

    uart_tx("FatFs SD logger ready. Logging every 5s.\r\n");
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        bmp280_read_temp();
        log_to_sd();
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        HAL_Delay(5000);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====
