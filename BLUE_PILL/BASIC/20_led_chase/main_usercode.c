/**
 * @file    main_usercode.c
 * @brief   LED Chase (Knight Rider) on PB8–PB15 — Blue Pill (STM32F103C8T6)
 *
 * Paste each labeled section into the matching USER CODE block inside the
 * CubeMX-generated main.c. Do NOT replace the entire main.c with this file.
 *
 * CubeMX peripherals required:
 *   - PB8–PB15: GPIO Output Push-Pull, initial LOW
 *
 * Wiring: Each LED → 330Ω resistor → GPIO pin. LED cathodes to GND.
 * GPIO HIGH = LED ON.
 *
 * NOT tested on hardware — provided as a reference.
 */

// ===== USER CODE BEGIN Includes =====
/* No extra includes required */
// ===== USER CODE END Includes =====


// ===== USER CODE BEGIN PD (Private Defines) =====
/* Bitmask covering all 8 LED pins PB8..PB15 */
#define LED_ALL_PINS  (GPIO_PIN_8  | GPIO_PIN_9  | GPIO_PIN_10 | GPIO_PIN_11 | \
                       GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15)

/* Delay between each chase step (milliseconds) */
#define CHASE_DELAY_MS   60U

/* Number of LEDs in the chain */
#define LED_COUNT        8U
// ===== USER CODE END PD =====


// ===== USER CODE BEGIN PV (Private Variables) =====
/* Ordered array of GPIO pin masks, index 0 = leftmost (PB8), index 7 = rightmost (PB15) */
static const uint16_t ledPins[LED_COUNT] = {
    GPIO_PIN_8,
    GPIO_PIN_9,
    GPIO_PIN_10,
    GPIO_PIN_11,
    GPIO_PIN_12,
    GPIO_PIN_13,
    GPIO_PIN_14,
    GPIO_PIN_15
};

/**
 * @brief Light a single LED by index (0–7); all others off.
 *        Uses two HAL_GPIO_WritePin calls:
 *          1. Clear all LED pins.
 *          2. Set the target pin.
 *
 * @param index  LED position 0 (PB8) to 7 (PB15).
 */
static void Chase_SetLED(uint8_t index)
{
    if (index >= LED_COUNT) return;

    /* Turn all LEDs off in one write */
    HAL_GPIO_WritePin(GPIOB, LED_ALL_PINS, GPIO_PIN_RESET);

    /* Turn on only the selected LED */
    HAL_GPIO_WritePin(GPIOB, ledPins[index], GPIO_PIN_SET);
}
// ===== USER CODE END PV =====


// ===== USER CODE BEGIN 2 (after MX_xxx_Init calls, before while loop) =====
    /* Ensure all LEDs start off */
    HAL_GPIO_WritePin(GPIOB, LED_ALL_PINS, GPIO_PIN_RESET);
    HAL_Delay(200);
// ===== USER CODE END 2 =====


// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        uint8_t i;

        /* Sweep left → right (index 0 to LED_COUNT-1) */
        for (i = 0; i < LED_COUNT; i++)
        {
            Chase_SetLED(i);
            HAL_Delay(CHASE_DELAY_MS);
        }

        /* Sweep right → left (index LED_COUNT-2 down to 1)
         * Skip the endpoints to avoid double-lighting them at the turnaround */
        for (i = LED_COUNT - 2U; i > 0U; i--)
        {
            Chase_SetLED(i);
            HAL_Delay(CHASE_DELAY_MS);
        }
    }
// ===== USER CODE END WHILE =====
