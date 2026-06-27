/**
 * @file    main_usercode.c
 * @brief   Internal RTC — set time/date, read every second, print via UART1.
 *
 * Paste each labelled block into the corresponding USER CODE BEGIN/END region
 * inside the CubeMX-generated main.c.
 *
 * CubeMX config required:
 *   - RTC: Activated
 *   - RTC Clock Source: LSI (internal ~40 kHz — Blue Pill typically has no LSE)
 *   - USART1: 115200 8N1, TX=PA9
 *   - SysTick as HAL timebase
 *
 * PC13 / BKP quirk:
 *   If PC13 behaves unexpectedly after enabling RTC, clear the ASOE bit in
 *   BKP->RTCCR inside USER CODE BEGIN 2 (see comment below).
 *
 * NOT TESTED ON HARDWARE — reference template only.
 */

/* ===== USER CODE BEGIN Includes ===== */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
/* ===== USER CODE END Includes ===== */


/* ===== USER CODE BEGIN PD (Private Defines) ===== */
/* Initial time and date to set at first boot */
#define RTC_INIT_HOURS    12
#define RTC_INIT_MINUTES   0
#define RTC_INIT_SECONDS   0

#define RTC_INIT_YEAR     25   /* 2-digit offset from 2000: 25 = 2025 */
#define RTC_INIT_MONTH    RTC_MONTH_JANUARY
#define RTC_INIT_DATE      1
#define RTC_INIT_WEEKDAY  RTC_WEEKDAY_WEDNESDAY
/* ===== USER CODE END PD ===== */


/* ===== USER CODE BEGIN PV (Private Variables) ===== */
static char         uart_buf[48];
static RTC_TimeTypeDef sTime = {0};
static RTC_DateTypeDef sDate = {0};
/* ===== USER CODE END PV ===== */


/* ===== USER CODE BEGIN 0 (Private function prototypes & implementations) ===== */

extern RTC_HandleTypeDef  hrtc;
extern UART_HandleTypeDef huart1;

/**
 * @brief Set the RTC time and date.
 *
 * Call once after MX_RTC_Init().  On STM32F1 the HAL RTC driver uses a
 * simplified date structure (only WeekDay is stored in hardware BKP registers
 * via the HAL layer; Year/Month/Date are emulated in software by the HAL).
 *
 * @return HAL_OK on success.
 */
static HAL_StatusTypeDef RTC_SetInitialDateTime(void)
{
    HAL_StatusTypeDef ret;

    sTime.Hours          = RTC_INIT_HOURS;
    sTime.Minutes        = RTC_INIT_MINUTES;
    sTime.Seconds        = RTC_INIT_SECONDS;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;

    ret = HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    if (ret != HAL_OK) return ret;

    sDate.Year    = RTC_INIT_YEAR;
    sDate.Month   = RTC_INIT_MONTH;
    sDate.Date    = RTC_INIT_DATE;
    sDate.WeekDay = RTC_INIT_WEEKDAY;

    ret = HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    return ret;
}

/* ===== USER CODE END 0 ===== */


/* ===== USER CODE BEGIN 2 (inside main(), after MX_RTC_Init()) ===== */
/*
    // ---- Optional: clear ASOE bit to stop RTC driving PC13 ----
    // If PC13 (onboard LED) misbehaves after enabling the RTC, uncomment:
    //
    // HAL_PWR_EnableBkUpAccess();
    // BKP->RTCCR &= ~BKP_RTCCR_ASOE;   // clear Anti-Squelch Output Enable
    // HAL_PWR_DisableBkUpAccess();

    // Set initial time/date — do this unconditionally for a simple demo.
    // In production, check a backup register flag to avoid resetting time
    // every power cycle.
    HAL_StatusTypeDef rtc_ret = RTC_SetInitialDateTime();
    if (rtc_ret != HAL_OK)
    {
        // Signal RTC init failure via fast LED blink
        while (1)
        {
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
            HAL_Delay(100);
        }
    }

    const char *banner = "RTC demo started\r\n";
    HAL_UART_Transmit(&huart1, (uint8_t *)banner, strlen(banner), 100);
*/
/* ===== USER CODE END 2 ===== */


/* ===== USER CODE BEGIN WHILE (main loop) ===== */
/*
    while (1)
    {
        HAL_Delay(1000);   // print once per second

        // IMPORTANT: on STM32F1 HAL, always read Time BEFORE Date.
        // Reading Date latches the shadow registers on F1 devices.
        HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

        // Format: "2025-01-01  12:00:01\r\n"
        snprintf(uart_buf, sizeof(uart_buf),
                 "20%02d-%02d-%02d  %02d:%02d:%02d\r\n",
                 sDate.Year,
                 sDate.Month,
                 sDate.Date,
                 sTime.Hours,
                 sTime.Minutes,
                 sTime.Seconds);

        HAL_UART_Transmit(&huart1, (uint8_t *)uart_buf, strlen(uart_buf), 100);

        // Heartbeat: toggle PC13 (active-low LED)
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    }
*/
/* ===== USER CODE END WHILE ===== */
