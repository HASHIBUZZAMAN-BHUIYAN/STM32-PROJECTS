/**
 * @file    main_usercode.c
 * @brief   Low Power Sleep Mode — Blue Pill (STM32F103C8T6)
 *
 * Paste each labeled section into the matching USER CODE block inside the
 * CubeMX-generated main.c. Do NOT replace the entire main.c with this file.
 *
 * CubeMX peripherals required:
 *   - USART1 (PA9 TX, PA10 RX), 115200 8N1
 *   - PA0: GPIO_EXTI0, falling-edge, pull-up
 *   - PC13: GPIO Output (onboard LED, active-low)
 *   - PWR peripheral enabled
 *   - NVIC: EXTI0_IRQn enabled
 *   - SysTick timebase reassigned to TIM1 (so SysTick doesn't wake sleep)
 *
 * NOT tested on hardware — provided as a reference.
 */

// ===== USER CODE BEGIN Includes =====
#include <string.h>
// ===== USER CODE END Includes =====


// ===== USER CODE BEGIN PV (Private Variables) =====
/* Flag set in EXTI callback; checked in main loop */
volatile uint8_t wakeFlag = 0;

/* UART transmit helper: send a null-terminated string */
static void UART_Print(UART_HandleTypeDef *huart, const char *msg)
{
    HAL_UART_Transmit(huart, (uint8_t *)msg, (uint16_t)strlen(msg), 100);
}
// ===== USER CODE END PV =====


// ===== USER CODE BEGIN 2 (after MX_xxx_Init calls, before while loop) =====
    UART_Print(&huart1, "\r\n=== Low Power Sleep Demo ===\r\n");
    UART_Print(&huart1, "Press PA0 button to wake from sleep.\r\n");

    /* Small delay so UART TX completes before sleeping */
    HAL_Delay(10);
// ===== USER CODE END 2 =====


// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        /* Announce intent to sleep */
        UART_Print(&huart1, "Sleeping...\r\n");

        /* Wait for UART TX buffer to drain before cutting the clock */
        HAL_Delay(5);

        /* Enter SLEEP mode — CPU halted, peripherals (UART, EXTI) stay on.
         * WFI: wake on any enabled interrupt (EXTI0 in this case).
         * MAINREGULATOR_ON keeps SRAM content valid. */
        HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

        /* ---- Execution resumes here after EXTI0 fires ---- */

        if (wakeFlag)
        {
            wakeFlag = 0;

            /* Toggle onboard LED (PC13, active-low) */
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

            UART_Print(&huart1, "Awake!\r\n");

            /* Brief visible delay before sleeping again */
            HAL_Delay(500);
        }
    }
// ===== USER CODE END WHILE =====


// ===== USER CODE BEGIN 4 (Callback implementations) =====
/**
 * @brief  EXTI line 0 interrupt callback (PA0 button press).
 *         Called by HAL_GPIO_EXTI_IRQHandler inside EXTI0_IRQHandler.
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_0)
    {
        wakeFlag = 1;
        /* No HAL_Delay here — we are inside an ISR */
    }
}
// ===== USER CODE END 4 =====
