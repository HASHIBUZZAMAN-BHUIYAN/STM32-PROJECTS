/**
 * @file    main_usercode.c
 * @brief   02 — Push Button Input with Software Debounce
 *          Blue Pill STM32F103C8T6
 *
 * PA0: momentary push button, internal pull-up enabled.
 *      Button connects PA0 to GND → pin reads LOW when pressed.
 * PC13: onboard LED, active-low.
 *
 * Debounce strategy: 50 ms delay after first falling edge detected,
 * re-read pin, act only if still LOW, then wait for release.
 *
 * Copy each block into the matching USER CODE BEGIN/END region
 * inside the CubeMX-generated Core/Src/main.c.
 *
 * NOTE: This code has NOT been tested on hardware. Review before flashing.
 *
 * CubeMX requirements:
 *   - PA0:  GPIO_Input, Pull-up
 *   - PC13: GPIO_Output, Push-Pull, No Pull, Speed Low
 */

/* ===== USER CODE BEGIN Includes ===== */
/* No extra includes needed */
/* ===== USER CODE END Includes ===== */


/* ===== USER CODE BEGIN PD (Private Defines) ===== */
#define DEBOUNCE_DELAY_MS   50U   /* Settle time after edge detection */
/* ===== USER CODE END PD ===== */


/* ===== USER CODE BEGIN PV (Private Variables) ===== */
/* None */
/* ===== USER CODE END PV ===== */


/* ===== USER CODE BEGIN 2 ===== */
/*
 * Initial state:
 *   PA0 reads HIGH (button open, pull-up active) — no action.
 *   PC13 starts HIGH (LED off) per CubeMX default.
 */
/* ===== USER CODE END 2 ===== */


/* ===== USER CODE BEGIN WHILE ===== */

    /* Step 1: detect falling edge (pin goes LOW = button pressed) */
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET)
    {
        /* Step 2: wait for contact bounce to settle */
        HAL_Delay(DEBOUNCE_DELAY_MS);

        /* Step 3: confirm pin is still LOW (genuine press, not noise) */
        if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET)
        {
            /*
             * Confirmed press — toggle the LED.
             * PC13 is active-low:
             *   GPIO_PIN_RESET -> LED ON
             *   GPIO_PIN_SET   -> LED OFF
             */
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

            /* Step 4: wait for button release before accepting next press */
            while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET)
            {
                /* Spin here until button is released.
                 * No delay needed; release is a slow human action. */
            }

            /* Brief settle delay after release */
            HAL_Delay(DEBOUNCE_DELAY_MS);
        }
    }

/* ===== USER CODE END WHILE ===== */
