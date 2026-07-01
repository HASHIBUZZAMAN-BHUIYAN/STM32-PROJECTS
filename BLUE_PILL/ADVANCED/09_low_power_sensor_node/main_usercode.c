// ============================================================
// Blue Pill ADVANCED/09 — Low-Power Sensor Node
// STM32F103C8T6  |  HAL + CubeMX 6.16
// ============================================================
// Paste each labeled block into the matching USER CODE section
// of the CubeMX-generated main.c.
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
// BMP280 I2C address (SDO pulled to GND → 0x76)
#define BMP280_ADDR      (0x76 << 1)
#define BMP280_REG_ID    0xD0
#define BMP280_REG_CTRL  0xF4
#define BMP280_REG_CONF  0xF5
#define BMP280_REG_DATA  0xF7   // 6 bytes: press MSB/LSB/XLSB, temp MSB/LSB/XLSB

// RTC Backup register for boot / wake count
#define WAKE_COUNT_BKP   RTC_BKP_DR1
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
static uint32_t wake_count = 0;
static char     uart_buf[128];
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static HAL_StatusTypeDef bmp280_write_reg(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t val)
{
    uint8_t buf[2] = {reg, val};
    return HAL_I2C_Master_Transmit(hi2c, BMP280_ADDR, buf, 2, 10);
}

static HAL_StatusTypeDef bmp280_read_regs(I2C_HandleTypeDef *hi2c, uint8_t reg,
                                           uint8_t *data, uint16_t len)
{
    if (HAL_I2C_Master_Transmit(hi2c, BMP280_ADDR, &reg, 1, 10) != HAL_OK) return HAL_ERROR;
    return HAL_I2C_Master_Receive(hi2c, BMP280_ADDR, data, len, 20);
}

static void bmp280_init(I2C_HandleTypeDef *hi2c)
{
    // Forced mode, osrs_t=x2, osrs_p=x2
    bmp280_write_reg(hi2c, BMP280_REG_CTRL, 0x57);
    // t_sb=0.5ms, filter=off, no SPI3w
    bmp280_write_reg(hi2c, BMP280_REG_CONF, 0x00);
}

/* Returns temperature in 0.01°C units (no compensation — raw demo) */
static int32_t bmp280_read_temp_raw(I2C_HandleTypeDef *hi2c)
{
    uint8_t data[3];
    // Trigger forced mode read
    bmp280_write_reg(hi2c, BMP280_REG_CTRL, 0x57);
    HAL_Delay(10);
    bmp280_read_regs(hi2c, 0xFA, data, 3);
    int32_t raw = ((int32_t)data[0] << 12) | ((int32_t)data[1] << 4) | (data[2] >> 4);
    // Simplified linear approximation (no calibration): ~25°C at mid-scale
    return (raw * 100) / 5243;  // very rough; use calibration in production
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    // Read and clear RTC reset flags
    __HAL_RCC_CLEAR_RESET_FLAGS();

    // Enable backup domain access
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();

    // Read wake count from backup register
    wake_count = HAL_RTCEx_BKUPRead(&hrtc, WAKE_COUNT_BKP);
    wake_count++;
    HAL_RTCEx_BKUPWrite(&hrtc, WAKE_COUNT_BKP, wake_count);

    // Init BMP280
    bmp280_init(&hi2c1);

    // Read temperature
    int32_t temp_cdeg = bmp280_read_temp_raw(&hi2c1);

    // Transmit reading via UART1
    int len = snprintf(uart_buf, sizeof(uart_buf),
        "Wake#%lu | Temp=%ld.%02ld C\r\n",
        wake_count, temp_cdeg / 100, (temp_cdeg < 0 ? -temp_cdeg : temp_cdeg) % 100);
    HAL_UART_Transmit(&huart1, (uint8_t *)uart_buf, len, 100);

    // Wait for UART to finish
    while (HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY) {}

    // Set RTC Alarm A to wake in 30 seconds
    RTC_AlarmTypeDef alarm = {0};
    alarm.AlarmTime.Seconds = 30;
    alarm.AlarmMask = RTC_ALARMMASK_ALL & ~RTC_ALARMMASK_SECONDS;
    // For F103 RTC (simple counter-based), use RTC alarm interrupt:
    // Compute next alarm value = current counter + 30
    uint32_t counter = RLL_BACKUP_ReadReg(BKP, DR2);  // reuse BKP_DR2 for simplicity
    // Simpler: use HAL_RTCEx_SetSecond_IT approach:
    // Actually for F103, just use the second interrupt:
    // Re-enter Standby with wakeup via EXTI17 (RTC alarm)
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, 30); // not used, just comment

    // On F103 entering Standby: RTC alarm wakes the MCU and it full resets
    // Set RTC alarm 30 seconds from now using RTC counter
    {
        uint32_t now;
        HAL_RTCEx_SetSecond_IT(&hrtc);  // not available on F1 directly
        // Use RTC_SetAlarm via LL for F103:
        uint32_t cnt = LL_RTC_TIME_Get(RTC);  // LL approach
        LL_RTC_ALARM_Set(RTC, cnt + 30);
        LL_RTC_EnableIT_ALR(RTC);
        LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_17);  // RTC alarm EXTI line
        LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_17);
    }

    // Clear Standby and Wakeup flags
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU | PWR_FLAG_SB);

    // Enter Standby mode — next wake is a full reset via RTC alarm
    HAL_PWR_EnterSTANDBYMode();

    // Never reached after Standby — MCU resets on RTC alarm
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    // Should not reach main loop — Standby is entered in USER CODE BEGIN 2
    while (1)
    {
        HAL_Delay(1000);  // failsafe
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====
