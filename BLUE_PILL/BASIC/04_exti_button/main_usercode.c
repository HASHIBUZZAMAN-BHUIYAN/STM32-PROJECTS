/**
 * @file    main_usercode.c
 * @brief   04 — External Interrupt Button (PA0 EXTI0)
 *          Blue Pill STM32F103C8T6
 *
 * PA0:  push button, internal pull-up, falling-edge EXTI trigger.
 *       Button connects PA0 to GND when pressed.
 * PC13: onboard LED, active-low.
 *
 * Debounce: timestamp comparison in HAL_GPIO_EXTI_Callback using
 * HAL_GetTick(). A 50 ms guard window rejects bounce events.
 *
 * Copy each block into the matching USER CODE BEGIN/END region
 * inside the CubeMX-generated Core/Src/main.c.
 *
 * NOTE: This code has NOT been tested on hardware. Review before flashing.
 *
 * CubeMX requirements:
 *   - PA0:  GPIO_EXTI0, Pull-up, Falling edge
 *   - PC13: GPIO_Output, Push-Pull, No Pull, Speed Low
 *   - NVIC: EXTI line0 interrupt enabled
 */

/* ===== USER CODE BEGIN Includes ===== */
/* No extra includes needed */
/* ===== USER CODE END Includes ===== */


/* ===== USER CODE BEGIN PD (Private Defines) ===== */
#define DEBOUNCE_MS   50U   /* Minimum ms between accepted interrupts */
/* ===== USER CODE END PD ===== */


/* ===== USER CODE BEGIN PV (Private Variables) ===== */

/*
 * volatile: modified in ISR context, read in main context.
 * The compiler must not cache these in registers.
 */
static volatile uint8_t  buttonFlag     = 0U;
static volatile uint32_t lastPressTick  = 0U;

/* ===== USER CODE END PV ===== */


/* ===== USER CODE BEGIN 2 ===== */
/* Nothing to initialise beyond what CubeMX generates */
/* ===== USER CODE END 2 ===== */


/* ===== USER CODE BEGIN WHILE ===== */

    if (buttonFlag != 0U)
    {
        /* Clear flag immediately to avoid missing a subsequent press */
        buttonFlag = 0U;

        /*
         * Check debounce window.
         * HAL_GetTick() wraps at ~49 days; subtraction handles wrap correctly
         * for periods < 2^32 ms (which 50 ms certainly is).
         */
        if ((HAL_GetTick() - lastPressTick) >= DEBOUNCE_MS)
        {
            lastPressTick = HAL_GetTick();

            /*
             * Toggle LED on PC13 (active-low):
             *   GPIO_PIN_RESET -> LED ON
             *   GPIO_PIN_SET   -> LED OFF
             */
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        }
    }

/* ===== USER CODE END WHILE ===== */


/* ===== USER CODE BEGIN 4 (External function implementations) ===== */

/**
 * @brief  EXTI line detection callback — called by HAL from EXTI0_IRQHandler.
 * @param  GPIO_Pin: pin that triggered the interrupt
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_0)
    {
        /*
         * Record approximate press time and set flag.
         * Do NOT toggle the LED here — LED manipulation inside an ISR
         * is poor practice and can cause timing issues.
         */
        lastPressTick = HAL_GetTick();
        buttonFlag    = 1U;
    }
}

/* ===== USER CODE END 4 ===== */
