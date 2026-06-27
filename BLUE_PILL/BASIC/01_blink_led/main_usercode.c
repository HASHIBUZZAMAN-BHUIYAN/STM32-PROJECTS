/**
 * @file    main_usercode.c
 * @brief   01 — Blink LED (PC13, Active-Low)
 *          Blue Pill STM32F103C8T6
 *
 * This file contains ONLY the user code sections.
 * Copy each block into the matching USER CODE BEGIN/END region
 * inside the CubeMX-generated Core/Src/main.c.
 *
 * NOTE: This code has NOT been tested on hardware. Review before flashing.
 *
 * CubeMX requirements:
 *   - PC13: GPIO_Output, Push-Pull, No Pull, Speed Low
 */

/* ===== USER CODE BEGIN Includes ===== */
/* No extra includes needed for this project */
/* ===== USER CODE END Includes ===== */


/* ===== USER CODE BEGIN PD (Private Defines) ===== */
#define BLINK_DELAY_MS   500U   /* Half-period: 500 ms ON, 500 ms OFF */
/* ===== USER CODE END PD ===== */


/* ===== USER CODE BEGIN PV (Private Variables) ===== */
/* None */
/* ===== USER CODE END PV ===== */


/* ===== USER CODE BEGIN 2 (after MX_GPIO_Init inside main) ===== */

/*
 * PC13 is ACTIVE-LOW:
 *   GPIO_PIN_RESET (0) -> LED ON
 *   GPIO_PIN_SET   (1) -> LED OFF
 *
 * CubeMX initialises the pin HIGH by default (LED off).
 * The first HAL_GPIO_TogglePin call will drive it LOW -> LED turns ON.
 */

/* ===== USER CODE END 2 ===== */


/* ===== USER CODE BEGIN WHILE (inside the infinite while loop) ===== */

    /* Toggle PC13 — because the LED is active-low, each toggle
     * alternates between LED ON (pin LOW) and LED OFF (pin HIGH). */
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    HAL_Delay(BLINK_DELAY_MS);

/* ===== USER CODE END WHILE ===== */
