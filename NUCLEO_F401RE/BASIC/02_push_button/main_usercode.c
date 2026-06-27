/**
 * BASIC/02 — Push Button with LED Toggle
 * Target: Nucleo-F401RE (STM32F401RETx)
 *
 * B1 = PC13 (active-LOW, internal pull-up)
 * LD2 = PA5  (active-HIGH)
 *
 * Paste each labeled block into the matching USER CODE section
 * in the CubeMX-generated main.c.
 */

// ===== USER CODE BEGIN PD =====
#define DEBOUNCE_MS   50U   // Debounce window in milliseconds
// ===== USER CODE END PD =====


// ===== USER CODE BEGIN PV =====
static uint32_t lastPressTime = 0;
static uint8_t  lastButtonState = GPIO_PIN_SET;  // Not pressed (pull-up HIGH)
// ===== USER CODE END PV =====


// ===== USER CODE BEGIN 2 =====
// Ensure LED starts OFF
HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
// ===== USER CODE END 2 =====


// ===== USER CODE BEGIN WHILE =====
while (1)
{
    uint8_t currentState = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);
    uint32_t now = HAL_GetTick();

    // Detect falling edge (press) with debounce
    if ((currentState == GPIO_PIN_RESET) &&
        (lastButtonState == GPIO_PIN_SET) &&
        ((now - lastPressTime) > DEBOUNCE_MS))
    {
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);  // Toggle LD2
        lastPressTime = now;
    }

    lastButtonState = currentState;
    HAL_Delay(10);  // Poll every 10 ms
}
// ===== USER CODE END WHILE =====
