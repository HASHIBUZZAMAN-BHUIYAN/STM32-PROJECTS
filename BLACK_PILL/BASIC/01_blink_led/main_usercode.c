/**
 ******************************************************************************
 * @file    main_usercode.c
 * @project 01_blink_led
 * @board   STM32F411CEU6 Black Pill
 * @brief   User code sections to paste into CubeMX-generated main.c
 *
 * INSTRUCTIONS:
 *   Copy each section into the matching USER CODE BEGIN/END block inside
 *   the CubeMX-generated Core/Src/main.c file.
 *   Do NOT replace the entire main.c — only insert within the marked regions.
 *
 * ACTIVE-LOW LED:
 *   PC13 LED is ACTIVE-LOW on the Black Pill.
 *   GPIO_PIN_RESET (LOW)  = LED ON
 *   GPIO_PIN_SET   (HIGH) = LED OFF
 *
 * NOTE: This code has not been tested on hardware. Verify before use.
 ******************************************************************************
 */

/* ===== USER CODE BEGIN Includes ===== */
/* No additional includes needed for blink */
/* ===== USER CODE END Includes ===== */


/* ===== USER CODE BEGIN PD ===== */
/* No private defines needed */
/* ===== USER CODE END PD ===== */


/* ===== USER CODE BEGIN PV ===== */
/* No private variables needed */
/* ===== USER CODE END PV ===== */


/* ===== USER CODE BEGIN PFP ===== */
/* No private function prototypes needed */
/* ===== USER CODE END PFP ===== */


/* ===== USER CODE BEGIN 0 ===== */
/* No code before main() needed */
/* ===== USER CODE END 0 ===== */


/* ===== USER CODE BEGIN 1 ===== */
/* (Inside main(), before MX init calls — nothing needed here) */
/* ===== USER CODE END 1 ===== */


/* ===== USER CODE BEGIN 2 ===== */
/*
 * At this point MX_GPIO_Init() has already run.
 * PC13 was initialised HIGH (LED OFF) by CubeMX.
 * Nothing extra to set up for a simple blink.
 *
 * PC13 is ACTIVE-LOW:
 *   GPIO_PIN_RESET => LED ON
 *   GPIO_PIN_SET   => LED OFF
 */
/* ===== USER CODE END 2 ===== */


/* ===== USER CODE BEGIN WHILE ===== */
while (1)
{
    /* Toggle PC13 — active-low, so each toggle flips the LED state */
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

    /*
     * Delay 500 ms.
     * Because we toggle once per iteration, the LED changes state every
     * 500 ms → ON for 500 ms, then OFF for 500 ms → 1 Hz blink rate.
     */
    HAL_Delay(500);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
}
/* ===== USER CODE END 3 ===== */
