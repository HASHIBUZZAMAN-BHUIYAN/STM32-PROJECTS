/*
 * main_usercode.c — 21: HC-SR04 Ultrasonic Distance via TIM2 Input Capture
 *
 * Board  : Blue Pill (STM32F103C8T6), 72 MHz
 * TRIG   : PA1  — GPIO Output, 10 µs pulse
 * ECHO   : PA0  — TIM2 CH1 Input Capture (1 µs tick, PSC = 71)
 * UART1  : PA9 TX / PA10 RX — 115200 baud
 *
 * Algorithm:
 *   1. Send 10 µs HIGH pulse on TRIG.
 *   2. Capture rising edge of ECHO → record t1.
 *   3. Switch polarity to falling edge → record t2.
 *   4. width_us = t2 - t1  (handle 16-bit overflow).
 *   5. distance_cm = width_us / 58.
 *
 * NOTE: This code has NOT been tested on hardware.
 *       Paste each section into the matching USER CODE block in main.c.
 */

/* ===== USER CODE BEGIN Includes ===== */
#include <stdio.h>
#include <string.h>
/* ===== USER CODE END Includes ===== */


/* ===== USER CODE BEGIN PD (Private Defines) ===== */
#define TRIG_PIN        GPIO_PIN_1
#define TRIG_PORT       GPIOA

/* HC-SR04: distance (cm) = pulse_width_us / 58 */
#define SOUND_DIVISOR   58U

/* Timeout: if no echo within 30 ms, sensor out of range (~5 m) */
#define ECHO_TIMEOUT_US 30000U
/* ===== USER CODE END PD ===== */


/* ===== USER CODE BEGIN PV (Private Variables) ===== */
/* Declared extern so the ISR and main() share them */
static volatile uint32_t ic_rising_val  = 0;
static volatile uint32_t ic_falling_val = 0;
static volatile uint8_t  ic_state       = 0;   /* 0 = waiting for rising, 1 = waiting for falling */
static volatile uint8_t  ic_done        = 0;   /* Set to 1 when both edges captured */

static char uart_buf[64];
/* ===== USER CODE END PV ===== */


/* ===== USER CODE BEGIN 0 (Function Prototypes / Helper Functions) ===== */

/**
 * @brief  Send a 10 µs HIGH pulse on the TRIG pin.
 *         Requires SysTick to be 1 ms (default HAL_Delay resolution).
 *         For µs delay we busy-wait using DWT or a calibrated loop.
 *         Here we use a simple CoreClock-based cycle count.
 */
static void HCSR04_SendTrigger(void)
{
    HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_SET);

    /* ~10 µs busy-wait at 72 MHz: 72 cycles/µs × 10 = 720 cycles */
    /* Using DWT_CYCCNT for accuracy (enable DWT in SystemInit or here) */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk;

    uint32_t start = DWT->CYCCNT;
    while ((DWT->CYCCNT - start) < (72U * 10U))
    {
        /* busy-wait */
    }

    HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);
}

/**
 * @brief  TIM2 Input Capture callback (called from HAL_TIM_IC_CaptureCallback).
 *         Paste this function body INTO the HAL_TIM_IC_CaptureCallback weak override
 *         in main.c (or a separate file), NOT directly here.
 */
/* NOTE: Place the following callback in main.c or a user callback file:

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
    {
        if (ic_state == 0)
        {
            // Rising edge captured
            ic_rising_val = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
            ic_state = 1;
            ic_done  = 0;

            // Reconfigure for falling edge
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1,
                                          TIM_INPUTCHANNELPOLARITY_FALLING);
        }
        else
        {
            // Falling edge captured
            ic_falling_val = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
            ic_state = 0;
            ic_done  = 1;

            // Restore to rising edge for next measurement
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1,
                                          TIM_INPUTCHANNELPOLARITY_RISING);
        }
    }
}
*/

/* ===== USER CODE END 0 ===== */


/* ===== USER CODE BEGIN 2 (Init, after MX_xxx_Init calls) ===== */
/*
    // Ensure TRIG starts LOW
    HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);

    // Start TIM2 CH1 input capture in interrupt mode
    if (HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }

    // Initial capture polarity: rising edge
    __HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_1,
                                  TIM_INPUTCHANNELPOLARITY_RISING);
    ic_state = 0;
    ic_done  = 0;

    // Print banner
    const char *banner = "\r\n=== HC-SR04 Ultrasonic Distance ===\r\n";
    HAL_UART_Transmit(&huart1, (uint8_t *)banner, strlen(banner), 100);
*/
/* ===== USER CODE END 2 ===== */


/* ===== USER CODE BEGIN WHILE (Main loop body) ===== */
/*
    while (1)
    {
        // --- Step 1: Send trigger pulse ---
        ic_done  = 0;
        ic_state = 0;
        __HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_1,
                                      TIM_INPUTCHANNELPOLARITY_RISING);

        HCSR04_SendTrigger();

        // --- Step 2: Wait for capture to complete (with timeout) ---
        uint32_t timeout_start = HAL_GetTick();
        while (!ic_done)
        {
            if ((HAL_GetTick() - timeout_start) > 35U)   // 35 ms > 30 ms echo timeout
            {
                break;
            }
        }

        // --- Step 3: Compute distance ---
        if (ic_done)
        {
            uint32_t width_us;

            // Handle 16-bit counter overflow (TIM2 on F103 is 16-bit, ARR = 0xFFFF)
            if (ic_falling_val >= ic_rising_val)
            {
                width_us = ic_falling_val - ic_rising_val;
            }
            else
            {
                width_us = (0xFFFFU - ic_rising_val) + ic_falling_val + 1U;
            }

            uint32_t distance_cm = width_us / SOUND_DIVISOR;

            int len = snprintf(uart_buf, sizeof(uart_buf),
                               "Width: %lu us  Distance: %lu cm\r\n",
                               (unsigned long)width_us,
                               (unsigned long)distance_cm);
            HAL_UART_Transmit(&huart1, (uint8_t *)uart_buf, (uint16_t)len, 50);

            // Blink onboard LED (PC13, active-low) to indicate valid reading
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        }
        else
        {
            const char *oor = "Out of range (no echo)\r\n";
            HAL_UART_Transmit(&huart1, (uint8_t *)oor, strlen(oor), 50);
        }

        HAL_Delay(100);   // 100 ms between measurements
    }
*/
/* ===== USER CODE END WHILE ===== */
