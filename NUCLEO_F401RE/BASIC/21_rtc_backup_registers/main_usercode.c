// ============================================================
// Nucleo-F401RE BASIC/21 — RTC Backup Registers
// STM32F401RETx  |  HAL + CubeMX 6.16
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
#define BOOT_COUNT_REG  RTC_BKP_DR1
#define RTC_INIT_MAGIC  0xDEAD
#define MAGIC_REG       RTC_BKP_DR0
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
static char buf[64];
static uint32_t boot_count = 0;
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 2 =====
    // Enable backup domain access
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();

    if (HAL_RTCEx_BKUPRead(&hrtc, MAGIC_REG) != RTC_INIT_MAGIC) {
        // First boot — initialize RTC time to 12:00:00
        RTC_TimeTypeDef sTime = {0};
        sTime.Hours   = 12; sTime.Minutes = 0; sTime.Seconds = 0;
        HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
        RTC_DateTypeDef sDate = {0};
        sDate.Year = 25; sDate.Month = 1; sDate.Date = 1;
        HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
        HAL_RTCEx_BKUPWrite(&hrtc, MAGIC_REG, RTC_INIT_MAGIC);
        HAL_RTCEx_BKUPWrite(&hrtc, BOOT_COUNT_REG, 0);
        HAL_UART_Transmit(&huart2, (uint8_t*)"RTC initialized\r\n", 17, 100);
    }

    // Read and increment boot counter
    boot_count = HAL_RTCEx_BKUPRead(&hrtc, BOOT_COUNT_REG) + 1;
    HAL_RTCEx_BKUPWrite(&hrtc, BOOT_COUNT_REG, boot_count);
    int n = snprintf(buf, sizeof(buf), "Boot #%lu\r\n", boot_count);
    HAL_UART_Transmit(&huart2, (uint8_t*)buf, n, 100);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        RTC_TimeTypeDef t = {0};
        HAL_RTC_GetTime(&hrtc, &t, RTC_FORMAT_BIN);
        // Must also call GetDate to unlock the shadow registers
        RTC_DateTypeDef d = {0};
        HAL_RTC_GetDate(&hrtc, &d, RTC_FORMAT_BIN);

        int n = snprintf(buf, sizeof(buf), "Boot=%lu | Time=%02d:%02d:%02d\r\n",
                         boot_count, t.Hours, t.Minutes, t.Seconds);
        HAL_UART_Transmit(&huart2, (uint8_t*)buf, n, 100);
        HAL_Delay(5000);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====
