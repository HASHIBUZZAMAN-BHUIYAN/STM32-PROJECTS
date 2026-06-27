/**
 ******************************************************************************
 * @file    main_usercode.c
 * @project 02_push_button
 * @board   STM32F411CEU6 Black Pill
 * @brief   User code sections to paste into CubeMX-generated main.c
 *
 * INSTRUCTIONS:
 *   Copy each section into the matching USER CODE BEGIN/END block inside
 *   the CubeMX-generated Core/Src/main.c file.
 *
 * HARDWARE:
 *   PA0  — Push button, one side to PA0, other side to GND.
 *           Internal pull-up enabled in CubeMX (PA0 reads HIGH when open,
 *           LOW when pressed).
 *   PC13 — Onboard LED, ACTIVE-LOW (GPIO_PIN_RESET = LED ON).
 *
 * DEBOUNCE STRATEGY:
 *   Edge-detect: compare current button state to last known stable state.
 *   If they differ AND 50 ms have elapsed since the last edge, accept the
 *   new state. This filters mechanical bounce without a timer interrupt.
 *
 * NOTE: This code has not been tested on hardware. Verify before use.
 ******************************************************************************
 */

/* ===== USER CODE BEGIN Includes ===== */
/* No additional includes needed */
/* ===== USER CODE END Includes ===== */


/* ===== USER CODE BEGIN PD ===== */
#define DEBOUNCE_MS  50U   /* Minimum milliseconds between accepted edges */
/* ===== USER CODE END PD ===== */


/* ===== USER CODE BEGIN PV ===== */
/* Track the last accepted (stable) button state */
static GPIO_PinState lastButtonState  = GPIO_PIN_SET; /* Pull-up → HIGH when released */
/* Timestamp of the last edge we saw (not necessarily accepted) */
static uint32_t      lastDebounceTime = 0U;
/* ===== USER CODE END PV ===== */


/* ===== USER CODE BEGIN PFP ===== */
/* No private function prototypes needed */
/* ===== USER CODE END PFP ===== */


/* ===== USER CODE BEGIN 0 ===== */
/* ===== USER CODE END 0 ===== */


/* ===== USER CODE BEGIN 2 ===== */
/*
 * MX_GPIO_Init() has already configured:
 *   PA0  → Input, Pull-up
 *   PC13 → Output Push-Pull, initial HIGH (LED OFF)
 *
 * Seed lastButtonState to the actual pin level so we don't fire
 * a spurious toggle at startup.
 */
lastButtonState = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
/* ===== USER CODE END 2 ===== */


/* ===== USER CODE BEGIN WHILE ===== */
while (1)
{
    GPIO_PinState currentState = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);

    /* Detect any change from last stable state */
    if (currentState != lastButtonState)
    {
        uint32_t now = HAL_GetTick(); /* ms since power-on */

        /* Only accept the new state if enough time has elapsed */
        if ((now - lastDebounceTime) >= DEBOUNCE_MS)
        {
            lastDebounceTime = now;
            lastButtonState  = currentState;

            /*
             * Act only on the PRESS edge (PA0 going LOW).
             * Ignore the RELEASE edge (PA0 going HIGH).
             * With internal pull-up:
             *   GPIO_PIN_RESET = button pressed  → act
             *   GPIO_PIN_SET   = button released → ignore
             */
            if (currentState == GPIO_PIN_RESET)
            {
                /* Toggle the onboard LED (PC13 is active-low) */
                HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
            }
        }
    }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
}
/* ===== USER CODE END 3 ===== */
