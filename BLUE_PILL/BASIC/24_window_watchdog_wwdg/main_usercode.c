/*
 * main_usercode.c — 24: Window Watchdog (WWDG)
 *
 * Board  : Blue Pill (STM32F103C8T6), 72 MHz
 * UART1  : PA9 TX / PA10 RX — 115200 baud
 * LED    : PC13 (active-low, onboard)
 *
 * CubeMX WWDG settings (assumed):
 *   Prescaler  : WWDG_PRESCALER_8
 *   Window     : 0x50  (refresh allowed when counter is BELOW this)
 *   Counter    : 0x7F  (maximum; written to WWDG_CR on start)
 *
 * At PCLK1 = 36 MHz:
 *   WWDG clock  = 36 MHz / 4096 / 8 ≈ 1098 Hz  (period ≈ 0.91 ms per count)
 *   Counts 0x7F→0x40 = 63 steps → total timeout ≈ 57 ms
 *   Window 0x50→0x40 = 16 steps → valid refresh window ≈ 14.6 ms
 *   Forbidden zone (too early): counter > 0x50, i.e. first ~38 ms after reload
 *
 * Demonstration order:
 *   Boot 1 (DEMO_TOO_EARLY = 1): refresh at ~1 ms (too early) → WWDG reset.
 *   Boot 2 (DEMO_TOO_EARLY = 0): refresh at ~45 ms (in window) → OK, loops.
 *
 * The demo switches behaviour by reading a flag stored in Backup Register DR1
 * so no user interaction is needed between resets.
 *
 * NOTE: This code has NOT been tested on hardware.
 *       Paste each section into the matching USER CODE block in main.c.
 */

/* ===== USER CODE BEGIN Includes ===== */
#include <stdio.h>
#include <string.h>
/* ===== USER CODE END Includes ===== */


/* ===== USER CODE BEGIN PD (Private Defines) ===== */
/* WWDG counter / window values — must match CubeMX settings */
#define WWDG_COUNTER_RELOAD     0x7FU
#define WWDG_WINDOW_VALUE       0x50U

/*
 * Backup register used to alternate demo mode across resets.
 * RTC backup register DR1 survives a system reset (but not a power-off
 * without VBAT).  Enable the "Backup Domain" (PWR + BKP) in CubeMX
 * if you want to use this approach.  Alternatively hard-code DEMO_TOO_EARLY.
 */
#define BKP_DEMO_MODE_REG       BKP_DR1

/* Delay to wait before refreshing in the "good" scenario (ms).
   Must land inside the valid window (>38 ms and <57 ms at the settings above).
   45 ms is chosen as a comfortable mid-window value. */
#define GOOD_REFRESH_DELAY_MS   45U

/* Delay in "too-early" scenario — refreshes while counter is still > window */
#define EARLY_REFRESH_DELAY_MS  1U
/* ===== USER CODE END PD ===== */


/* ===== USER CODE BEGIN PV (Private Variables) ===== */
static char uart_buf[128];
static uint8_t demo_too_early = 0U;
/* ===== USER CODE END PV ===== */


/* ===== USER CODE BEGIN 0 (Helper Functions) ===== */

static void UART_Print(UART_HandleTypeDef *huart, const char *str)
{
    HAL_UART_Transmit(huart, (const uint8_t *)str, (uint16_t)strlen(str), 200);
}

/* ===== USER CODE END 0 ===== */


/* ===== USER CODE BEGIN 2 (Init, after MX_xxx_Init calls) ===== */
/*
    // ----------------------------------------------------------------
    // Enable access to backup domain registers (requires PWR + BKP clocks
    // enabled via CubeMX: RCC → Peripheral Clocks → Enable BKP / PWR).
    // ----------------------------------------------------------------
    HAL_PWR_EnableBkUpAccess();

    // ----------------------------------------------------------------
    // Determine which demo scenario to run by reading backup register.
    // 0x0001 in DR1 → run "too-early" scenario this boot.
    // Anything else → run "good refresh" scenario.
    // Toggle for the NEXT boot.
    // ----------------------------------------------------------------
    uint16_t bkp_val = HAL_RTCEx_BKUPRead(NULL, RTC_BKP_DR1);
    // Note: on STM32F1, BKP registers are accessed directly:
    // uint16_t bkp_val = BKP->DR1;
    // We use the raw register access here for clarity.
    bkp_val = BKP->DR1;

    if (bkp_val == 0x0001U)
    {
        demo_too_early = 1U;
        BKP->DR1 = 0x0000U;   // next boot will do the good scenario
    }
    else
    {
        demo_too_early = 0U;
        BKP->DR1 = 0x0001U;   // next boot will do the too-early scenario
    }

    // ----------------------------------------------------------------
    // Check reset source: was it a WWDG reset?
    // ----------------------------------------------------------------
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST))
    {
        UART_Print(&huart1, "\r\n*** WWDG Reset detected! ***\r\n");
        __HAL_RCC_CLEAR_RESET_FLAGS();
    }
    else
    {
        UART_Print(&huart1, "\r\n=== WWDG Demo — Power-on / SW Reset ===\r\n");
    }

    if (demo_too_early)
    {
        UART_Print(&huart1, "Mode: EARLY refresh (expect WWDG reset in ~1 ms)\r\n");
    }
    else
    {
        UART_Print(&huart1, "Mode: CORRECT refresh (expect normal operation)\r\n");
    }

    int len = snprintf(uart_buf, sizeof(uart_buf),
                       "Window=0x%02X  Counter=0x%02X\r\n",
                       WWDG_WINDOW_VALUE, WWDG_COUNTER_RELOAD);
    HAL_UART_Transmit(&huart1, (uint8_t *)uart_buf, (uint16_t)len, 100);

    // ----------------------------------------------------------------
    // Start WWDG — CubeMX-generated MX_WWDG_Init() already called.
    // HAL_WWDG_Init() writes the counter to WWDG_CR and enables it.
    // The WWDG cannot be stopped after this point.
    // ----------------------------------------------------------------
    // (MX_WWDG_Init already called above by CubeMX generated code)

    UART_Print(&huart1, "WWDG started.\r\n");
*/
/* ===== USER CODE END 2 ===== */


/* ===== USER CODE BEGIN WHILE (Main loop) ===== */
/*
    while (1)
    {
        if (demo_too_early)
        {
            // --------------------------------------------------------
            // BAD: Refresh after only 1 ms — counter still above window.
            // The WWDG will reset the MCU because we wrote to CR while
            // T[6:0] >= Window value (0x50).
            // --------------------------------------------------------
            HAL_Delay(EARLY_REFRESH_DELAY_MS);

            UART_Print(&huart1, "Refreshing WWDG too early...\r\n");
            HAL_WWDG_Refresh(&hwwdg);   // This triggers an immediate reset

            // Code should NOT reach here
            UART_Print(&huart1, "ERROR: Should have reset!\r\n");
        }
        else
        {
            // --------------------------------------------------------
            // GOOD: Wait until counter falls into the valid window,
            // then refresh.  45 ms is within [38 ms, 57 ms] at these
            // WWDG settings.
            // --------------------------------------------------------
            HAL_Delay(GOOD_REFRESH_DELAY_MS);

            HAL_WWDG_Refresh(&hwwdg);

            // Toggle LED to show we are alive
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

            UART_Print(&huart1, "WWDG refreshed in window. System OK.\r\n");
        }
    }
*/
/* ===== USER CODE END WHILE ===== */
