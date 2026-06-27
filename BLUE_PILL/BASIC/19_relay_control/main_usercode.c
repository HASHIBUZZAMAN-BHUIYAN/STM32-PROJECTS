/**
 * @file    main_usercode.c
 * @brief   Relay Control via GPIO — Blue Pill (STM32F103C8T6)
 *
 * Paste each labeled section into the matching USER CODE block inside the
 * CubeMX-generated main.c. Do NOT replace the entire main.c with this file.
 *
 * CubeMX peripherals required:
 *   - PB5: GPIO Output Push-Pull, initial LOW (relay OFF)
 *   - PC13: GPIO Output Push-Pull (onboard LED, active-low) — optional mirror
 *
 * SAFETY: The relay switches external circuits that may be at mains voltage.
 * Ensure proper isolation and rated components. See wiring_notes.md.
 *
 * NOT tested on hardware — provided as a reference.
 */

// ===== USER CODE BEGIN Includes =====
/* No extra includes required */
// ===== USER CODE END Includes =====


// ===== USER CODE BEGIN PD (Private Defines) =====
/* Relay module control: HIGH = relay energized (load ON).
 * If your module is active-LOW, swap GPIO_PIN_SET / GPIO_PIN_RESET below. */
#define RELAY_ON    GPIO_PIN_SET
#define RELAY_OFF   GPIO_PIN_RESET

#define RELAY_TOGGLE_MS  2000U   /* Toggle interval in milliseconds */
// ===== USER CODE END PD =====


// ===== USER CODE BEGIN PV (Private Variables) =====
/**
 * @brief Energize the relay (load ON). PC13 LED turns ON (active-low → RESET).
 */
static void Relay_On(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, RELAY_ON);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET); /* LED ON (active-low) */
}

/**
 * @brief De-energize the relay (load OFF). PC13 LED turns OFF.
 */
static void Relay_Off(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, RELAY_OFF);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);   /* LED OFF (active-low) */
}
// ===== USER CODE END PV =====


// ===== USER CODE BEGIN 2 (after MX_xxx_Init calls, before while loop) =====
    /* Start with relay off and LED off */
    Relay_Off();
    HAL_Delay(500);
// ===== USER CODE END 2 =====


// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        /* Relay ON — load energized */
        Relay_On();
        HAL_Delay(RELAY_TOGGLE_MS);

        /* Relay OFF — load de-energized */
        Relay_Off();
        HAL_Delay(RELAY_TOGGLE_MS);
    }
// ===== USER CODE END WHILE =====
