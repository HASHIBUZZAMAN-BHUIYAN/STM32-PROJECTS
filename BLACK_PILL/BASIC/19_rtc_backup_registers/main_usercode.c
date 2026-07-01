// ============================================================
// Black Pill BASIC/19 — RTC + Backup Registers
// STM32F411CEU6  |  HAL + CubeMX 6.16  |  100 MHz
// RTC with LSI, 20 backup registers, USART1
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
#define RTC_MAGIC    0x12345678UL
#define BKP_MAGIC    RTC_BKP_DR0
#define BKP_RESETS   RTC_BKP_DR1
#define BKP_USER_VAL RTC_BKP_DR2
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
static char buf[80];
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static void uart_tx(const char *s) {
    HAL_UART_Transmit(&huart1, (const uint8_t*)s, strlen(s), 200);
}

static void rtc_set_initial(void) {
    RTC_TimeTypeDef t = {0};
    RTC_DateTypeDef d = {0};
    t.Hours = 12; t.Minutes = 0; t.Seconds = 0;
    d.Year  = 25; d.Month = 1; d.Date = 1; d.WeekDay = RTC_WEEKDAY_WEDNESDAY;
    HAL_RTC_SetTime(&hrtc, &t, RTC_FORMAT_BIN);
    HAL_RTC_SetDate(&hrtc, &d, RTC_FORMAT_BIN);
}

static void rtc_print_time(void) {
    RTC_TimeTypeDef t = {0};
    RTC_DateTypeDef d = {0};
    HAL_RTC_GetTime(&hrtc, &t, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &d, RTC_FORMAT_BIN); // MANDATORY shadow unlock
    int n = snprintf(buf, sizeof(buf),
                     "20%02u-%02u-%02u %02u:%02u:%02u\r\n",
                     d.Year, d.Month, d.Date,
                     t.Hours, t.Minutes, t.Seconds);
    uart_tx(buf);
    (void)n;
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    uart_tx("Black Pill RTC + Backup Registers\r\n");

    if (HAL_RTCEx_BKUPRead(&hrtc, BKP_MAGIC) != RTC_MAGIC) {
        // First boot
        uart_tx("First boot: setting RTC to 2025-01-01 12:00:00\r\n");
        rtc_set_initial();
        HAL_RTCEx_BKUPWrite(&hrtc, BKP_MAGIC,  RTC_MAGIC);
        HAL_RTCEx_BKUPWrite(&hrtc, BKP_RESETS, 0);
        HAL_RTCEx_BKUPWrite(&hrtc, BKP_USER_VAL, 42);
    } else {
        uint32_t resets = HAL_RTCEx_BKUPRead(&hrtc, BKP_RESETS) + 1;
        HAL_RTCEx_BKUPWrite(&hrtc, BKP_RESETS, resets);
        uint32_t user_val = HAL_RTCEx_BKUPRead(&hrtc, BKP_USER_VAL);
        int n = snprintf(buf, sizeof(buf),
                         "Reset #%lu  user_val=%lu  time=", resets, user_val);
        uart_tx(buf);
        (void)n;
    }
    rtc_print_time();
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        // Print RTC time every 5 seconds
        rtc_print_time();
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        HAL_Delay(5000);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====
