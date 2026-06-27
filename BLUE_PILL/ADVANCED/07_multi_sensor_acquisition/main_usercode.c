/**
 * main_usercode.c — 07_multi_sensor_acquisition
 *
 * TIM3 at 1 kHz drives ADC DMA sampling. Every 100 samples, computes
 * min/max/avg, reads BMP280 temperature via I2C (in main loop, not ISR),
 * and transmits a summary via UART1 DMA.
 *
 * Requires CubeMX-generated handles: htim3, hadc1, hi2c1, huart1
 * Requires DMA1 configured for ADC1 (Ch1) and USART1 TX (Ch4).
 *
 * NOT tested on hardware. Reference implementation only.
 */

// ===== USER CODE BEGIN Includes =====
#include <string.h>
#include <stdio.h>
#include <stdint.h>
// ===== USER CODE END Includes =====


// ===== USER CODE BEGIN PD =====
#define ADC_BUF_SIZE        100U    /* Number of ADC samples per batch        */
#define UART_TX_BUF_SIZE    64U     /* UART transmit buffer length            */

/* BMP280 */
#define BMP280_ADDR         (0x76U << 1)
#define BMP280_REG_TEMP_MSB 0xFAU
#define BMP280_REG_CTRL     0xF4U
#define BMP280_REG_CALIB    0x88U
// ===== USER CODE END PD =====


// ===== USER CODE BEGIN PV =====

/* ADC DMA double-buffer: DMA writes here continuously */
static volatile uint16_t g_adc_buf[ADC_BUF_SIZE];

/* Set by TIM3 ISR to schedule I2C read in main loop */
static volatile uint8_t  g_i2c_pending  = 0U;

/* Set by ADC DMA half/full-complete callback */
static volatile uint8_t  g_buf_ready    = 0U;

/* UART TX guard: 1 while a DMA TX is in progress */
static volatile uint8_t  g_uart_tx_busy = 0U;

/* UART output buffer (written by main, read by DMA) */
static char g_tx_buf[UART_TX_BUF_SIZE];

/* TIM3 tick counter (increments in ISR) */
static volatile uint32_t g_tim3_ticks = 0U;

/* Last BMP280 temperature (in 0.01 degC units) */
static volatile int32_t  g_last_temp_cdeg = 0;

/* BMP280 trimming coefficients */
static uint16_t g_dig_T1;
static int16_t  g_dig_T2;
static int16_t  g_dig_T3;
// ===== USER CODE END PV =====


// ===== USER CODE BEGIN 0 =====

/* ---- BMP280 helpers ---- */

static HAL_StatusTypeDef bmp280_read_calib(void)
{
    uint8_t calib[6];
    uint8_t reg = BMP280_REG_CALIB;

    HAL_StatusTypeDef st = HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, &reg, 1U, 10U);
    if (st != HAL_OK) { return st; }
    st = HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, calib, 6U, 10U);
    if (st != HAL_OK) { return st; }

    g_dig_T1 = (uint16_t)(calib[0] | ((uint16_t)calib[1] << 8U));
    g_dig_T2 = (int16_t)(calib[2] | ((int16_t)calib[3] << 8U));
    g_dig_T3 = (int16_t)(calib[4] | ((int16_t)calib[5] << 8U));
    return HAL_OK;
}

static int32_t bmp280_read_temp_cdeg(void)
{
    uint8_t ctrl[2] = { BMP280_REG_CTRL, 0x25U }; /* forced mode, osrs_t=1 */
    if (HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, ctrl, 2U, 10U) != HAL_OK) {
        return INT32_MIN;
    }
    HAL_Delay(10U);

    uint8_t reg = BMP280_REG_TEMP_MSB;
    uint8_t raw[3];
    if (HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, &reg, 1U, 10U) != HAL_OK) {
        return INT32_MIN;
    }
    if (HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, raw, 3U, 10U) != HAL_OK) {
        return INT32_MIN;
    }

    int32_t adc_T = (int32_t)(((uint32_t)raw[0] << 12U) |
                               ((uint32_t)raw[1] << 4U)  |
                               ((uint32_t)raw[2] >> 4U));

    int32_t var1 = (int32_t)((adc_T / 8) - ((int32_t)g_dig_T1 * 2));
    var1 = (var1 * (int32_t)g_dig_T2) / 2048;
    int32_t var2 = (int32_t)((adc_T / 16) - (int32_t)g_dig_T1);
    var2 = (((var2 * var2) / 4096) * (int32_t)g_dig_T3) / 16384;
    int32_t t_fine = var1 + var2;
    return (t_fine * 5 + 128) / 256;
}

/* ---- ADC statistics ---- */

typedef struct {
    uint16_t min;
    uint16_t max;
    uint32_t avg;
} AdcStats_t;

static AdcStats_t compute_adc_stats(const volatile uint16_t *buf, uint32_t count)
{
    AdcStats_t s;
    s.min = buf[0];
    s.max = buf[0];
    uint32_t sum = 0U;

    for (uint32_t i = 0U; i < count; i++) {
        uint16_t v = buf[i];
        if (v < s.min) { s.min = v; }
        if (v > s.max) { s.max = v; }
        sum += v;
    }
    s.avg = sum / count;
    return s;
}
// ===== USER CODE END 0 =====


/* --------------------------------------------------------------------------
 * main() user code sections
 * -------------------------------------------------------------------------- */

// ===== USER CODE BEGIN 2 =====
/*
    // Read BMP280 calibration (non-fatal)
    bmp280_read_calib();

    // Start ADC in DMA circular mode — fills g_adc_buf continuously
    if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)g_adc_buf, ADC_BUF_SIZE) != HAL_OK) {
        Error_Handler();
    }

    // Start TIM3 to generate 1 kHz interrupt
    if (HAL_TIM_Base_Start_IT(&htim3) != HAL_OK) {
        Error_Handler();
    }
*/
// ===== USER CODE END 2 =====


// ===== USER CODE BEGIN WHILE =====
/*
    while (1) {
        // --- I2C read (scheduled, not in ISR) ---
        if (g_i2c_pending) {
            g_i2c_pending = 0U;
            int32_t t = bmp280_read_temp_cdeg();
            if (t != INT32_MIN) {
                g_last_temp_cdeg = t;
            }
        }

        // --- ADC stats + UART report ---
        if (g_buf_ready) {
            g_buf_ready = 0U;

            AdcStats_t stats = compute_adc_stats(g_adc_buf, ADC_BUF_SIZE);

            // Format report only if UART DMA is idle
            if (!g_uart_tx_busy) {
                int32_t temp = g_last_temp_cdeg;
                int tx_len = snprintf(g_tx_buf, UART_TX_BUF_SIZE,
                    "ADC: min=%u max=%u avg=%lu | BMP280: %ld.%02ld C\r\n",
                    (unsigned)stats.min,
                    (unsigned)stats.max,
                    (unsigned long)stats.avg,
                    (long)(temp / 100),
                    (long)(temp < 0 ? -(temp % 100) : (temp % 100)));

                if (tx_len > 0 && tx_len < (int)UART_TX_BUF_SIZE) {
                    g_uart_tx_busy = 1U;
                    HAL_UART_Transmit_DMA(&huart1, (uint8_t *)g_tx_buf, (uint16_t)tx_len);
                }
            }
        }
    }
*/
// ===== USER CODE END WHILE =====


// ===== USER CODE BEGIN 4 =====
/*
// TIM3 Period Elapsed Callback — fires at 1 kHz
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3) {
        g_tim3_ticks++;
        // Schedule I2C read every 100 ticks (100 ms)
        if ((g_tim3_ticks % 100U) == 0U) {
            g_i2c_pending = 1U;
        }
        // NOTE: ADC DMA runs continuously in circular mode.
        // The half/full complete callbacks handle batch-ready notification.
    }
}

// ADC DMA Half-Complete Callback
// Called when first 50 samples written; we choose to act on the full buffer only.
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
    (void)hadc;
    // Intentionally empty — we wait for full buffer
}

// ADC DMA Full-Complete Callback — 100 samples ready
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1) {
        g_buf_ready = 1U;
    }
}

// UART TX DMA Complete Callback
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        g_uart_tx_busy = 0U;
    }
}
*/
// ===== USER CODE END 4 =====
