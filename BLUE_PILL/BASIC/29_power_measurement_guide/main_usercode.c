/*
 * main_usercode.c — Project 29: Power Measurement Guide — Sleep Mode Firmware
 * Target: STM32F103C8T6 (Blue Pill)
 *
 * Demonstrates: enter Sleep (WFI), wake on PA0 button EXTI, print state
 * via USART1, repeat. Use this as the test firmware while measuring current
 * with a multimeter in series with the 3.3 V supply rail.
 *
 * CubeMX requirements:
 *   - PC13: GPIO Output (onboard LED, active-low)
 *   - PA0:  GPIO_EXTI0, Pull-Up, Falling edge
 *   - NVIC: EXTI0 enabled
 *   - USART1: Asynchronous, 115200, 8N1
 *   - SysTick for HAL_GetTick / HAL_Delay
 *
 * NOTE: This code has not been tested on hardware.
 */

// ===== USER CODE BEGIN Includes =====
#include <string.h>
// ===== USER CODE END Includes =====


// ===== USER CODE BEGIN PD (Private Defines) =====
#define LED_PIN         GPIO_PIN_13
#define LED_PORT        GPIOC
#define BTN_PIN         GPIO_PIN_0
#define BTN_PORT        GPIOA
#define UART_TIMEOUT_MS 100U
// ===== USER CODE END PD =====


// ===== USER CODE BEGIN PV (Private Variables) =====
extern UART_HandleTypeDef huart1;

/* Volatile flag set by EXTI ISR so the main loop knows a wake event occurred */
static volatile uint8_t wake_event = 0;

static uint32_t sleep_count = 0;
static uint32_t wake_count  = 0;
// ===== USER CODE END PV =====


// ===== USER CODE BEGIN 0 (Functions defined before main) =====

/**
 * @brief  Send a null-terminated string over USART1 (blocking).
 *         Waits up to UART_TIMEOUT_MS ms for completion.
 */
static void uart_print(const char *msg)
{
    HAL_UART_Transmit(&huart1,
                      (uint8_t *)msg,
                      (uint16_t)strlen(msg),
                      UART_TIMEOUT_MS);
}

/**
 * @brief  Wait until USART1 TX shift register is empty before sleeping.
 *         Without this, entering sleep while the UART is still sending
 *         can corrupt the last byte of the outgoing message.
 */
static void uart_flush(void)
{
    /* Poll TXE (Transmit Data Register Empty) and TC (Transmission Complete) */
    while (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC) == RESET)
    {
        /* Busy-wait — typically only a few microseconds at 115200 baud */
    }
}

/**
 * @brief  EXTI0 callback — fires when PA0 button is pressed (falling edge).
 *         Sets wake_event flag; does NOT debounce here.
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == BTN_PIN)
    {
        wake_event = 1;
    }
}

/**
 * @brief  Blink LED once to give a visible confirmation of waking.
 * @param  on_ms   Duration LED is ON in milliseconds.
 */
static void led_blink_once(uint32_t on_ms)
{
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET); /* ON  */
    HAL_Delay(on_ms);
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);   /* OFF */
}

// ===== USER CODE END 0 =====


// ===== USER CODE BEGIN 2 (inside main(), after MX_xxx_Init calls) =====
{
    /* Ensure LED is off before entering sleep to minimize current draw */
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);

    char buf[80];

    uart_print("=== Project 29: Power Measurement — Sleep Mode ===\r\n");
    uart_print("Press PA0 button to wake from sleep.\r\n");
    uart_print("Measure current at 3.3V rail with multimeter in series.\r\n\r\n");

    HAL_Delay(100); /* Brief pause so UART flushes before first sleep */
}
// ===== USER CODE END 2 =====


// ===== USER CODE BEGIN WHILE (inside the while(1) loop) =====
{
    char buf[80];

    /* --- Prepare to sleep --- */
    sleep_count++;

    /* LED off — reduce current draw during sleep measurement */
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);

    /* Format and send the pre-sleep message */
    snprintf(buf, sizeof(buf),
             "Sleep #%lu — entering sleep now. Measure current here.\r\n",
             (unsigned long)sleep_count);
    uart_print(buf);
    uart_flush();   /* Wait for TX to complete BEFORE sleeping */

    /* Clear any pending button flag from before we entered this iteration */
    wake_event = 0;

    /*
     * Enter Sleep mode (CPU halted, peripherals including SysTick still running).
     * HAL_PWR_EnterSLEEPMode with PWR_SLEEPENTRY_WFI waits for any interrupt.
     * EXTI0 (PA0 button) will wake the CPU.
     *
     * NOTE: SysTick is still active in Sleep mode, so HAL_Delay and HAL_GetTick
     * continue to work after wake. For deeper current reduction, disable SysTick
     * before WFI and re-enable after (this also blocks HAL_Delay during sleep).
     */
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

    /*
     * Execution resumes here after any interrupt wakes the CPU.
     * If it was not the button (e.g., SysTick), we loop back and sleep again
     * unless wake_event was set.
     */
    if (wake_event)
    {
        wake_event = 0;
        wake_count++;

        /* Simple debounce — ignore extra pulses within 100 ms */
        HAL_Delay(100);

        /* Blink to confirm wake */
        led_blink_once(200);

        snprintf(buf, sizeof(buf),
                 "Wake #%lu — now running at 72 MHz. Measure current here.\r\n",
                 (unsigned long)wake_count);
        uart_print(buf);

        /* Stay awake briefly so the user can observe active-mode current */
        HAL_Delay(2000);
    }
    /* Loop back to sleep */
}
// ===== USER CODE END WHILE =====
