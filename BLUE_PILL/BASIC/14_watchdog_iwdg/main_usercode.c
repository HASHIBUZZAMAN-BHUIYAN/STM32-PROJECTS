/**
 * @file    main_usercode.c
 * @brief   Independent Watchdog (IWDG) demo with reset-source detection.
 *
 * Paste each labelled block into the corresponding USER CODE BEGIN/END region
 * inside the CubeMX-generated main.c.
 *
 * CubeMX config required:
 *   - IWDG: Enabled, Prescaler /32, Reload 2499
 *     → Timeout ≈ (2500 × 32) / 40000 Hz = 2.0 s
 *   - USART1: 115200 8N1, TX=PA9
 *   - SysTick as HAL timebase
 *
 * To demonstrate watchdog reset: comment out HAL_IWDG_Refresh() in the loop,
 * rebuild and flash.  The MCU will reset every ~2 seconds; the UART will show
 * "** IWDG RESET **" each time it boots.
 *
 * NOT TESTED ON HARDWARE — reference template only.
 */

/* ===== USER CODE BEGIN Includes ===== */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
/* ===== USER CODE END Includes ===== */


/* ===== USER CODE BEGIN PD (Private Defines) ===== */
/*
 * LED blink periods (ms).
 * Active-low: GPIO_PIN_RESET = LED ON.
 */
#define LED_BLINK_NORMAL_MS   500U   /* 1 Hz blink when watchdog is being fed */
/* ===== USER CODE END PD ===== */


/* ===== USER CODE BEGIN PV (Private Variables) ===== */
static char uart_buf[80];
/* ===== USER CODE END PV ===== */


/* ===== USER CODE BEGIN 0 (Private function prototypes & implementations) ===== */

extern IWDG_HandleTypeDef hiwdg;
extern UART_HandleTypeDef huart1;

/**
 * @brief Read and decode the RCC reset-source flags.
 *        Prints a human-readable reason to UART1.
 *        IMPORTANT: Call this BEFORE clearing the flags.
 */
static void PrintResetSource(void)
{
    const char *reason = "Unknown";

    if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST))
    {
        reason = "** IWDG RESET **";
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST))
    {
        reason = "Software reset";
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST))
    {
        reason = "Power-on reset (POR)";
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST))
    {
        reason = "NRST pin reset";
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST))
    {
        reason = "Window watchdog reset";
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST))
    {
        reason = "Low-power reset";
    }

    snprintf(uart_buf, sizeof(uart_buf), "Reset cause: %s\r\n", reason);
    HAL_UART_Transmit(&huart1, (uint8_t *)uart_buf, strlen(uart_buf), 200);

    /* Clear all reset flags for the next boot */
    __HAL_RCC_CLEAR_RESET_FLAGS();
}

/* ===== USER CODE END 0 ===== */


/* ===== USER CODE BEGIN 2 (inside main(), after MX_IWDG_Init() & MX_USART1_UART_Init()) ===== */
/*
    // Print the cause of the most recent reset before doing anything else
    PrintResetSource();

    snprintf(uart_buf, sizeof(uart_buf),
             "IWDG demo started. Timeout ~2 s. Refreshing every %lu ms.\r\n",
             (unsigned long)LED_BLINK_NORMAL_MS);
    HAL_UART_Transmit(&huart1, (uint8_t *)uart_buf, strlen(uart_buf), 200);
*/
/* ===== USER CODE END 2 ===== */


/* ===== USER CODE BEGIN WHILE (main loop) ===== */
/*
    uint32_t loop_count = 0;

    while (1)
    {
        // ---- Refresh the IWDG ----
        // Comment out the next line to allow the watchdog to expire (~2 s).
        HAL_IWDG_Refresh(&hiwdg);

        // Heartbeat blink on PC13 (active-low)
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

        // Print status to UART
        loop_count++;
        snprintf(uart_buf, sizeof(uart_buf),
                 "IWDG OK  loop=%lu\r\n", (unsigned long)loop_count);
        HAL_UART_Transmit(&huart1, (uint8_t *)uart_buf, strlen(uart_buf), 100);

        HAL_Delay(LED_BLINK_NORMAL_MS);
    }
*/
/* ===== USER CODE END WHILE ===== */
