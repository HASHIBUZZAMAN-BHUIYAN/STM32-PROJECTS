/*
 * main_usercode.c — 25: Multi-Peripheral Integration
 *                       BMP280 (I2C1) + USART1 + TIM1 CH1 PWM LED
 *
 * Board  : Blue Pill (STM32F103C8T6), 72 MHz
 * I2C1   : PB6 SCL / PB7 SDA — BMP280 temperature sensor
 * UART1  : PA9 TX / PA10 RX  — 115200 baud
 * TIM1 CH1: PA8               — PWM output to LED
 *
 * Behaviour:
 *   Every 500 ms: read BMP280 temperature, print over UART1, update PWM
 *   duty on TIM1 CH1 (20 °C → 0 %, 40 °C → 100 %).
 *
 * BMP280 I2C address: 0x76 (SDO tied to GND) or 0x77 (SDO tied to VCC).
 *   Change BMP280_I2C_ADDR accordingly.
 *
 * CubeMX:
 *   I2C1 Standard Mode 100 kHz; USART1 115200 8N1;
 *   TIM1 CH1 PWM, PSC=71, ARR=999 (1 kHz).
 *
 * NOTE: This code has NOT been tested on hardware.
 *       Paste each section into the matching USER CODE block in main.c.
 */

/* ===== USER CODE BEGIN Includes ===== */
#include <stdio.h>
#include <string.h>
/* ===== USER CODE END Includes ===== */


/* ===== USER CODE BEGIN PD (Private Defines) ===== */
/* BMP280 I2C address (7-bit, left-shifted by HAL) */
#define BMP280_I2C_ADDR         (0x76U << 1)

/* BMP280 register addresses */
#define BMP280_REG_CHIP_ID      0xD0U
#define BMP280_REG_RESET        0xE0U
#define BMP280_REG_CTRL_MEAS    0xF4U
#define BMP280_REG_CONFIG       0xF5U
#define BMP280_REG_PRESS_MSB    0xF7U   /* 0xF7–0xF9: pressure raw */
#define BMP280_REG_TEMP_MSB     0xFAU   /* 0xFA–0xFC: temperature raw */
#define BMP280_REG_CALIB_START  0x88U   /* 0x88–0x9F: 24 bytes of trim */

#define BMP280_CHIP_ID          0x60U   /* Expected value from 0xD0 */
#define BMP280_RESET_VALUE      0xB6U

/* Oversampling x1 + Normal mode: ctrl_meas = 0b 001 001 11 = 0x27 */
#define BMP280_CTRL_MEAS_VAL    0x27U
/* Standby 500 ms, filter off: config = 0b 100 000 00 = 0x80 */
#define BMP280_CONFIG_VAL       0x80U

/* PWM mapping: [TEMP_MIN_C, TEMP_MAX_C] → [0 %, 100 %] */
#define TEMP_MIN_C              20.0f
#define TEMP_MAX_C              40.0f

/* TIM1 ARR value (must match CubeMX setting) */
#define TIM1_ARR                999U

#define I2C_TIMEOUT_MS          100U
/* ===== USER CODE END PD ===== */


/* ===== USER CODE BEGIN PV (Private Variables) ===== */

/* BMP280 calibration trimming parameters (from sensor OTP) */
typedef struct {
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
} BMP280_CalibData_t;

static BMP280_CalibData_t bmp_calib;
static char uart_buf[128];
/* ===== USER CODE END PV ===== */


/* ===== USER CODE BEGIN 0 (Helper Functions) ===== */

/* ---------- I2C helpers ---------- */

static HAL_StatusTypeDef BMP280_WriteReg(I2C_HandleTypeDef *hi2c,
                                          uint8_t reg, uint8_t value)
{
    uint8_t buf[2] = { reg, value };
    return HAL_I2C_Master_Transmit(hi2c, BMP280_I2C_ADDR, buf, 2U, I2C_TIMEOUT_MS);
}

static HAL_StatusTypeDef BMP280_ReadRegs(I2C_HandleTypeDef *hi2c,
                                          uint8_t reg, uint8_t *data, uint16_t len)
{
    HAL_StatusTypeDef s;
    s = HAL_I2C_Master_Transmit(hi2c, BMP280_I2C_ADDR, &reg, 1U, I2C_TIMEOUT_MS);
    if (s != HAL_OK) return s;
    return HAL_I2C_Master_Receive(hi2c, BMP280_I2C_ADDR, data, len, I2C_TIMEOUT_MS);
}

/* ---------- BMP280 init ---------- */

static HAL_StatusTypeDef BMP280_Init(I2C_HandleTypeDef *hi2c)
{
    uint8_t chip_id = 0U;
    HAL_StatusTypeDef s;

    /* Verify chip ID */
    s = BMP280_ReadRegs(hi2c, BMP280_REG_CHIP_ID, &chip_id, 1U);
    if (s != HAL_OK) return s;
    if (chip_id != BMP280_CHIP_ID) return HAL_ERROR;

    /* Soft-reset */
    s = BMP280_WriteReg(hi2c, BMP280_REG_RESET, BMP280_RESET_VALUE);
    if (s != HAL_OK) return s;
    HAL_Delay(10);   /* wait for reset to complete */

    /* Read 24-byte calibration block */
    uint8_t calib_raw[24];
    s = BMP280_ReadRegs(hi2c, BMP280_REG_CALIB_START, calib_raw, 24U);
    if (s != HAL_OK) return s;

    bmp_calib.dig_T1 = (uint16_t)(calib_raw[1]  << 8) | calib_raw[0];
    bmp_calib.dig_T2 = (int16_t) ((calib_raw[3]  << 8) | calib_raw[2]);
    bmp_calib.dig_T3 = (int16_t) ((calib_raw[5]  << 8) | calib_raw[4]);
    bmp_calib.dig_P1 = (uint16_t)(calib_raw[7]  << 8) | calib_raw[6];
    bmp_calib.dig_P2 = (int16_t) ((calib_raw[9]  << 8) | calib_raw[8]);
    bmp_calib.dig_P3 = (int16_t) ((calib_raw[11] << 8) | calib_raw[10]);
    bmp_calib.dig_P4 = (int16_t) ((calib_raw[13] << 8) | calib_raw[12]);
    bmp_calib.dig_P5 = (int16_t) ((calib_raw[15] << 8) | calib_raw[14]);
    bmp_calib.dig_P6 = (int16_t) ((calib_raw[17] << 8) | calib_raw[16]);
    bmp_calib.dig_P7 = (int16_t) ((calib_raw[19] << 8) | calib_raw[18]);
    bmp_calib.dig_P8 = (int16_t) ((calib_raw[21] << 8) | calib_raw[20]);
    bmp_calib.dig_P9 = (int16_t) ((calib_raw[23] << 8) | calib_raw[22]);

    /* Configure: standby 500 ms, filter off */
    s = BMP280_WriteReg(hi2c, BMP280_REG_CONFIG, BMP280_CONFIG_VAL);
    if (s != HAL_OK) return s;

    /* ctrl_meas: temp x1, pressure x1, normal mode */
    s = BMP280_WriteReg(hi2c, BMP280_REG_CTRL_MEAS, BMP280_CTRL_MEAS_VAL);
    return s;
}

/* ---------- BMP280 temperature read ----------
 * Returns temperature in degrees Celsius (float).
 * Uses the integer compensation formula from BMP280 datasheet Appendix.
 */
static float BMP280_ReadTemperature(I2C_HandleTypeDef *hi2c)
{
    uint8_t raw[3];
    if (BMP280_ReadRegs(hi2c, BMP280_REG_TEMP_MSB, raw, 3U) != HAL_OK)
    {
        return -999.0f;   /* error sentinel */
    }

    int32_t adc_T = ((int32_t)raw[0] << 12) |
                    ((int32_t)raw[1] << 4)  |
                    ((int32_t)raw[2] >> 4);

    /* BMP280 compensation formula (integer, from datasheet Section 4.2.3) */
    int32_t var1, var2, t_fine;
    var1 = ((((adc_T >> 3) - ((int32_t)bmp_calib.dig_T1 << 1))) *
             ((int32_t)bmp_calib.dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)bmp_calib.dig_T1)) *
              ((adc_T >> 4) - ((int32_t)bmp_calib.dig_T1))) >> 12) *
             ((int32_t)bmp_calib.dig_T3)) >> 14;
    t_fine = var1 + var2;

    /* t_fine in units of 1/5120 °C; divide by 5120 to get °C as float */
    return (float)((t_fine * 5 + 128) >> 8) / 100.0f;
}

/* ---------- PWM duty update ---------- */

/**
 * @brief  Map temperature to PWM duty and update TIM1 CH1 CCR.
 * @param  htim  Pointer to TIM1 handle.
 * @param  temp  Temperature in °C.
 */
static void PWM_UpdateDutyFromTemp(TIM_HandleTypeDef *htim, float temp)
{
    /* Clamp to [TEMP_MIN_C, TEMP_MAX_C] */
    if (temp < TEMP_MIN_C) temp = TEMP_MIN_C;
    if (temp > TEMP_MAX_C) temp = TEMP_MAX_C;

    float duty_frac = (temp - TEMP_MIN_C) / (TEMP_MAX_C - TEMP_MIN_C);
    uint32_t ccr = (uint32_t)(duty_frac * (float)(TIM1_ARR + 1U));
    if (ccr > TIM1_ARR) ccr = TIM1_ARR;

    __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, ccr);
}

/* ===== USER CODE END 0 ===== */


/* ===== USER CODE BEGIN 2 (Init, after MX_xxx_Init calls) ===== */
/*
    // ----------------------------------------------------------------
    // Banner
    // ----------------------------------------------------------------
    const char *banner = "\r\n=== Multi-Peripheral Integration ===\r\n"
                         "BMP280 (I2C1) + UART1 + TIM1 CH1 PWM\r\n";
    HAL_UART_Transmit(&huart1, (uint8_t *)banner, strlen(banner), 200);

    // ----------------------------------------------------------------
    // Init BMP280
    // ----------------------------------------------------------------
    if (BMP280_Init(&hi2c1) != HAL_OK)
    {
        const char *err = "ERROR: BMP280 init failed! Check wiring & address.\r\n";
        HAL_UART_Transmit(&huart1, (uint8_t *)err, strlen(err), 200);
        // LED ON to signal error
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
        Error_Handler();
    }
    HAL_UART_Transmit(&huart1, (uint8_t *)"BMP280 OK\r\n", 11, 100);

    // ----------------------------------------------------------------
    // Start TIM1 CH1 PWM
    // TIM1 is advanced-control; MOE bit must be set.
    // HAL_TIM_PWM_Start sets MOE via TIM_CCxChannelCmd + __HAL_TIM_MOE_ENABLE.
    // ----------------------------------------------------------------
    if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1) != HAL_OK)
    {
        const char *err = "ERROR: TIM1 PWM start failed!\r\n";
        HAL_UART_Transmit(&huart1, (uint8_t *)err, strlen(err), 200);
        Error_Handler();
    }
    // Ensure Main Output Enable (MOE) is set for TIM1
    __HAL_TIM_MOE_ENABLE(&htim1);

    HAL_UART_Transmit(&huart1, (uint8_t *)"TIM1 PWM OK\r\n", 13, 100);
    HAL_UART_Transmit(&huart1, (uint8_t *)"Starting measurement loop...\r\n", 30, 100);
*/
/* ===== USER CODE END 2 ===== */


/* ===== USER CODE BEGIN WHILE (Main loop) ===== */
/*
    uint32_t last_tick = HAL_GetTick();

    while (1)
    {
        if ((HAL_GetTick() - last_tick) >= 500U)
        {
            last_tick = HAL_GetTick();

            // --- Read temperature from BMP280 ---
            float temp_c = BMP280_ReadTemperature(&hi2c1);

            if (temp_c < -998.0f)
            {
                HAL_UART_Transmit(&huart1,
                    (uint8_t *)"I2C read error\r\n", 16, 100);
            }
            else
            {
                // --- Print over UART1 ---
                int32_t temp_int  = (int32_t)temp_c;
                int32_t temp_frac = (int32_t)((temp_c - (float)temp_int) * 100.0f);
                if (temp_frac < 0) temp_frac = -temp_frac;

                int len = snprintf(uart_buf, sizeof(uart_buf),
                                   "Temp: %ld.%02ld C\r\n",
                                   (long)temp_int, (long)temp_frac);
                HAL_UART_Transmit(&huart1, (uint8_t *)uart_buf, (uint16_t)len, 100);

                // --- Update PWM duty on TIM1 CH1 ---
                PWM_UpdateDutyFromTemp(&htim1, temp_c);

                // --- Blink onboard LED to signal activity ---
                HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
            }
        }
    }
*/
/* ===== USER CODE END WHILE ===== */
