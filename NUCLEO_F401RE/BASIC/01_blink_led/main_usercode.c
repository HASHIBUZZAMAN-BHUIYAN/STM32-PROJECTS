/**
 * BASIC/01 — Blink LED
 * Target: Nucleo-F401RE (STM32F401RETx)
 *
 * Paste each labeled block into the matching USER CODE section
 * inside the CubeMX-generated main.c.
 *
 * NOTE: Code written following STM32 HAL / CubeMX 6.16 conventions.
 * Verify pin assignments match your CubeMX project before building.
 */

// ===== USER CODE BEGIN Includes =====
// No extra includes needed for this project
// ===== USER CODE END Includes =====


// ===== USER CODE BEGIN 2 =====
// (No initialization needed beyond what CubeMX generates)
// ===== USER CODE END 2 =====


// ===== USER CODE BEGIN WHILE =====
while (1)
{
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);  // Toggle LD2 (PA5)
    HAL_Delay(500);                          // 500 ms delay → 1 Hz blink
}
// ===== USER CODE END WHILE =====
