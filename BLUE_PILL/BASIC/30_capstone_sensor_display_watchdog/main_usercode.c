/*
 * main_usercode.c — Project 30: Capstone — BMP280 + SSD1306 OLED + IWDG
 * Target: STM32F103C8T6 (Blue Pill)
 *
 * Paste each labeled block into the matching USER CODE section of the
 * CubeMX-generated main.c.
 *
 * External libraries required (not included):
 *   - ssd1306 by afiskon: https://github.com/afiskon/stm32-ssd1306
 *     Copy ssd1306.c/.h and ssd1306_fonts.c/.h into project Src/Inc folders.
 *   - BMP280: raw I2C register access via HAL is implemented below.
 *     For production use, integrate the official Bosch BMP2-Sensor-API.
 *
 * CubeMX requirements:
 *   - I2C1 enabled (PB6=SCL, PB7=SDA), Standard mode 100 kHz
 *   - IWDG enabled, Prescaler /32, Reload ~3124 (~2.5 s timeout)
 *   - PC13: GPIO Output (onboard LED)
 *   - SysTick for HAL_GetTick/HAL_Delay
 *
 * NOTE: This code has not been tested on hardware.
 */

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
#include <string.h>
#include "ssd1306.h"
#include "ssd1306_fonts.h"
// ===== USER CODE END Includes =====


// ===== USER CODE BEGIN PD (Private Defines) =====
/* BMP280 I2C address — SDO to GND = 0x76, SDO to VCC = 0x77 */
#define BMP280_ADDR             (0x76 << 1)  /* HAL uses 8-bit shifted address */

/* BMP280 register addresses */
#define BMP280_REG_CALIB_START  0x88U   /* Calibration data start (26 bytes) */
#define BMP280_REG_CTRL_MEAS    0xF4U
#define BMP280_REG_CONFIG       0xF5U
#define BMP280_REG_PRESS_MSB    0xF7U   /* 6 bytes: press[3] + temp[3] burst */
#define BMP280_REG_CHIP_ID      0xD0U
#define BMP280_CHIP_ID          0x60U   /* BMP280 chip ID; BME280 = 0x60, BMP280 = 0x58 */

/* SSD1306 I2C address (typical) */
#define SSD1306_ADDR            0x3CU

/* Failure threshold before watchdog is withheld */
#define MAX_SENSOR_FAILURES     3U

/* LED */
#define LED_PIN                 GPIO_PIN_13
#define LED_PORT                GPIOC

/* Main loop cycle period in ms */
#define LOOP_PERIOD_MS          1000U
// ===== USER CODE END PD =====


// ===== USER CODE BEGIN PV (Private Variables) =====
extern I2C_HandleTypeDef  hi2c1;
extern IWDG_HandleTypeDef hiwdg;

/* BMP280 calibration coefficients (loaded once at startup) */
typedef struct
{
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;
} BMP280_Calib_t;

static BMP280_Calib_t bmp280_cal;
static uint8_t        bmp280_ready    = 0;
static uint32_t       consecutive_failures = 0;
// ===== USER CODE END PV =====


// ===== USER CODE BEGIN 0 (Functions defined before main) =====

/* ------------------------------------------------------------------ */
/*  BMP280 low-level HAL helpers                                        */
/* ------------------------------------------------------------------ */

/**
 * @brief  Write one byte to a BMP280 register.
 */
static HAL_StatusTypeDef bmp280_write_reg(uint8_t reg, uint8_t value)
{
    uint8_t buf[2] = { reg, value };
    return HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, buf, 2, 10);
}

/**
 * @brief  Read `len` bytes from BMP280 starting at `reg`.
 */
static HAL_StatusTypeDef bmp280_read_regs(uint8_t reg, uint8_t *data, uint16_t len)
{
    return HAL_I2C_Mem_Read(&hi2c1,
                            BMP280_ADDR,
                            reg,
                            I2C_MEMADD_SIZE_8BIT,
                            data,
                            len,
                            20);
}

/**
 * @brief  Initialize BMP280: verify chip ID, load calibration, set normal mode.
 * @return 1 on success, 0 on failure.
 */
static uint8_t bmp280_init(void)
{
    uint8_t chip_id = 0;

    /* Verify chip ID — BMP280 = 0x58, BME280 = 0x60 */
    if (bmp280_read_regs(BMP280_REG_CHIP_ID, &chip_id, 1) != HAL_OK)
    {
        return 0;
    }
    if (chip_id != 0x58 && chip_id != 0x60)
    {
        return 0;  /* Unexpected chip ID */
    }

    /* Read 24 bytes of calibration data starting at 0x88 */
    uint8_t raw[24];
    if (bmp280_read_regs(BMP280_REG_CALIB_START, raw, 24) != HAL_OK)
    {
        return 0;
    }

    /* Unpack little-endian calibration words */
    bmp280_cal.dig_T1 = (uint16_t)(raw[1]  << 8 | raw[0]);
    bmp280_cal.dig_T2 = (int16_t) (raw[3]  << 8 | raw[2]);
    bmp280_cal.dig_T3 = (int16_t) (raw[5]  << 8 | raw[4]);
    bmp280_cal.dig_P1 = (uint16_t)(raw[7]  << 8 | raw[6]);
    bmp280_cal.dig_P2 = (int16_t) (raw[9]  << 8 | raw[8]);
    bmp280_cal.dig_P3 = (int16_t) (raw[11] << 8 | raw[10]);
    bmp280_cal.dig_P4 = (int16_t) (raw[13] << 8 | raw[12]);
    bmp280_cal.dig_P5 = (int16_t) (raw[15] << 8 | raw[14]);
    bmp280_cal.dig_P6 = (int16_t) (raw[17] << 8 | raw[16]);
    bmp280_cal.dig_P7 = (int16_t) (raw[19] << 8 | raw[18]);
    bmp280_cal.dig_P8 = (int16_t) (raw[21] << 8 | raw[20]);
    bmp280_cal.dig_P9 = (int16_t) (raw[23] << 8 | raw[22]);

    /*
     * Configure BMP280:
     *   ctrl_meas: osrs_t = ×1 (001), osrs_p = ×1 (001), mode = Normal (11)
     *   Byte value: 0b00100111 = 0x27
     */
    if (bmp280_write_reg(BMP280_REG_CTRL_MEAS, 0x27) != HAL_OK)
    {
        return 0;
    }

    /* config: t_sb = 1000ms (101), filter = off (000), spi3w_en = 0 → 0xA0 */
    if (bmp280_write_reg(BMP280_REG_CONFIG, 0xA0) != HAL_OK)
    {
        return 0;
    }

    return 1;
}

/**
 * @brief  Read temperature and pressure from BMP280 using Bosch integer
 *         compensation formulas (simplified — see official API for full precision).
 *
 * @param[out] temp_x10   Temperature in 0.1 °C units (e.g., 245 = 24.5 °C)
 * @param[out] press_x10  Pressure in 0.1 hPa units (e.g., 10132 = 1013.2 hPa)
 * @return HAL_OK on success, HAL_ERROR on I2C failure.
 */
static HAL_StatusTypeDef bmp280_read(int32_t *temp_x10, int32_t *press_x10)
{
    uint8_t raw[6];

    /* Burst read: press_msb, press_lsb, press_xlsb, temp_msb, temp_lsb, temp_xlsb */
    if (bmp280_read_regs(BMP280_REG_PRESS_MSB, raw, 6) != HAL_OK)
    {
        return HAL_ERROR;
    }

    int32_t adc_T = (int32_t)(((uint32_t)raw[3] << 12) |
                               ((uint32_t)raw[4] << 4)  |
                               ((uint32_t)raw[5] >> 4));

    int32_t adc_P = (int32_t)(((uint32_t)raw[0] << 12) |
                               ((uint32_t)raw[1] << 4)  |
                               ((uint32_t)raw[2] >> 4));

    /* --- Temperature compensation (Bosch BMP280 datasheet, section 4.2.3) --- */
    int32_t var1, var2, t_fine;

    var1 = ((((adc_T >> 3) - ((int32_t)bmp280_cal.dig_T1 << 1))) *
             ((int32_t)bmp280_cal.dig_T2)) >> 11;

    var2 = (((((adc_T >> 4) - ((int32_t)bmp280_cal.dig_T1)) *
              ((adc_T >> 4) - ((int32_t)bmp280_cal.dig_T1))) >> 12) *
             ((int32_t)bmp280_cal.dig_T3)) >> 14;

    t_fine = var1 + var2;

    /* Temperature in degrees C × 100 */
    int32_t T = (t_fine * 5 + 128) >> 8;  /* in 0.01 °C */
    *temp_x10 = T / 10;                    /* Convert to 0.1 °C */

    /* --- Pressure compensation (Bosch BMP280 datasheet, section 4.2.3) --- */
    int64_t p_var1, p_var2, p;

    p_var1 = ((int64_t)t_fine) - 128000;
    p_var2 = p_var1 * p_var1 * (int64_t)bmp280_cal.dig_P6;
    p_var2 = p_var2 + ((p_var1 * (int64_t)bmp280_cal.dig_P5) << 17);
    p_var2 = p_var2 + (((int64_t)bmp280_cal.dig_P4) << 35);
    p_var1 = ((p_var1 * p_var1 * (int64_t)bmp280_cal.dig_P3) >> 8) +
             ((p_var1 * (int64_t)bmp280_cal.dig_P2) << 12);
    p_var1 = (((((int64_t)1) << 47) + p_var1)) *
             ((int64_t)bmp280_cal.dig_P1) >> 33;

    if (p_var1 == 0)
    {
        *press_x10 = 0;
        return HAL_OK;  /* Avoid division by zero */
    }

    p = 1048576 - adc_P;
    p = (((p << 31) - p_var2) * 3125) / p_var1;
    p_var1 = (((int64_t)bmp280_cal.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    p_var2 = (((int64_t)bmp280_cal.dig_P8) * p) >> 19;
    p = ((p + p_var1 + p_var2) >> 8) + (((int64_t)bmp280_cal.dig_P7) << 4);

    /* p is now in Pa × 256; convert to 0.1 hPa */
    /* 1 Pa = 0.01 hPa, so p/256 Pa = p/256/100 hPa = p/25600 hPa */
    /* We want 0.1 hPa units: p/256/100*10 = p/2560 */
    *press_x10 = (int32_t)((p / 2560));

    return HAL_OK;
}

/* ------------------------------------------------------------------ */
/*  OLED display helpers                                                */
/* ------------------------------------------------------------------ */

/**
 * @brief  Update OLED with temperature and pressure readings.
 * @param  temp_x10   Temperature in 0.1 °C (e.g., 245 = 24.5 °C)
 * @param  press_x10  Pressure in 0.1 hPa (e.g., 10132 = 1013.2 hPa)
 */
static void oled_show_readings(int32_t temp_x10, int32_t press_x10)
{
    char line[32];

    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);

    /* Line 1: Temperature */
    snprintf(line, sizeof(line), "Temp: %ld.%ld C",
             (long)(temp_x10 / 10),
             (long)(temp_x10 % 10 < 0 ? -(temp_x10 % 10) : temp_x10 % 10));
    ssd1306_WriteString(line, Font_7x10, White);

    ssd1306_SetCursor(0, 18);

    /* Line 2: Pressure */
    snprintf(line, sizeof(line), "Press: %ld.%ld hPa",
             (long)(press_x10 / 10),
             (long)(press_x10 % 10));
    ssd1306_WriteString(line, Font_7x10, White);

    ssd1306_UpdateScreen();
}

/**
 * @brief  Display a sensor error message on the OLED.
 * @param  fail_count  Current consecutive failure count.
 */
static void oled_show_error(uint32_t fail_count)
{
    char line[32];

    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString("SENSOR ERROR", Font_7x10, White);

    ssd1306_SetCursor(0, 18);
    snprintf(line, sizeof(line), "Fails: %lu/%u",
             (unsigned long)fail_count,
             MAX_SENSOR_FAILURES);
    ssd1306_WriteString(line, Font_7x10, White);

    ssd1306_UpdateScreen();
}

// ===== USER CODE END 0 =====


// ===== USER CODE BEGIN 2 (inside main(), after MX_xxx_Init calls) =====
{
    /* Brief delay to let I2C devices stabilize at power-on */
    HAL_Delay(100);

    /* Initialize OLED */
    ssd1306_Init();
    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString("Initializing...", Font_7x10, White);
    ssd1306_UpdateScreen();

    /* Initialize BMP280 */
    bmp280_ready = bmp280_init();

    if (!bmp280_ready)
    {
        ssd1306_Fill(Black);
        ssd1306_SetCursor(0, 0);
        ssd1306_WriteString("BMP280 INIT", Font_7x10, White);
        ssd1306_SetCursor(0, 18);
        ssd1306_WriteString("FAILED", Font_7x10, White);
        ssd1306_UpdateScreen();
        /* Do not enter main loop normally; stay here and let IWDG reset */
        while (1)
        {
            /* IWDG is running — if BMP280 never initializes, IWDG resets board */
            HAL_Delay(500);
        }
    }

    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString("BMP280 OK", Font_7x10, White);
    ssd1306_UpdateScreen();
    HAL_Delay(500);

    /* LED off at startup */
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);

    consecutive_failures = 0;
}
// ===== USER CODE END 2 =====


// ===== USER CODE BEGIN WHILE (inside the while(1) loop) =====
{
    uint32_t cycle_start = HAL_GetTick();

    int32_t temp_x10  = 0;
    int32_t press_x10 = 0;

    HAL_StatusTypeDef read_result = bmp280_read(&temp_x10, &press_x10);

    if (read_result == HAL_OK)
    {
        /* --- Successful read --- */
        consecutive_failures = 0;

        /* Update OLED with fresh data */
        oled_show_readings(temp_x10, press_x10);

        /* Blink LED once to show activity */
        HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);
        HAL_Delay(50);
        HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);

        /* Refresh watchdog — board is healthy */
        HAL_IWDG_Refresh(&hiwdg);
    }
    else
    {
        /* --- Sensor read failed --- */
        consecutive_failures++;

        oled_show_error(consecutive_failures);

        if (consecutive_failures < MAX_SENSOR_FAILURES)
        {
            /*
             * Still within tolerance — refresh the watchdog so the board
             * stays alive for the next attempt.
             */
            HAL_IWDG_Refresh(&hiwdg);
        }
        else
        {
            /*
             * 3rd consecutive failure — intentionally do NOT refresh IWDG.
             * The watchdog will time out and issue a system reset.
             * This is the designed error-recovery behavior.
             * No code needed here — just fall through without HAL_IWDG_Refresh.
             */
        }
    }

    /* Wait for the remainder of the 1-second cycle */
    uint32_t elapsed = HAL_GetTick() - cycle_start;
    if (elapsed < LOOP_PERIOD_MS)
    {
        HAL_Delay(LOOP_PERIOD_MS - elapsed);
    }
}
// ===== USER CODE END WHILE =====
