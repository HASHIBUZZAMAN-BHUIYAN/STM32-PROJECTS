/**
 * BASIC/04 — EXTI Button Interrupt
 * Target: Nucleo-F401RE (STM32F401RETx)
 *
 * PC13 → EXTI13 falling edge ISR increments counter
 * UART2 (PA2/PA3, 115200) prints counter every 1s
 *
 * Paste each labeled block into the matching USER CODE section
 * in the CubeMX-generated main.c.
 */

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====


// ===== USER CODE BEGIN PV =====
volatile uint32_t buttonCount = 0;
// ===== USER CODE END PV =====


// ===== USER CODE BEGIN 0 =====
// Retarget printf to UART2 (optional — can use HAL_UART_Transmit directly)
// If using printf retarget, add this function:
int __io_putchar(int ch)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}
// ===== USER CODE END 0 =====


// ===== USER CODE BEGIN 2 =====
// Print startup message
const char *startMsg = "BASIC/04 EXTI Button Demo\r\nPress B1 to increment counter.\r\n";
HAL_UART_Transmit(&huart2, (uint8_t *)startMsg, strlen(startMsg), HAL_MAX_DELAY);
// ===== USER CODE END 2 =====


// ===== USER CODE BEGIN WHILE =====
while (1)
{
    char buf[64];
    int len = snprintf(buf, sizeof(buf), "Button count: %lu\r\n", buttonCount);
    HAL_UART_Transmit(&huart2, (uint8_t *)buf, (uint16_t)len, HAL_MAX_DELAY);
    HAL_Delay(1000);
}
// ===== USER CODE END WHILE =====


// ===== USER CODE BEGIN 4 =====
/**
 * EXTI line[15:10] IRQ Handler callback.
 * CubeMX generates HAL_GPIO_EXTI_Callback — place this in USER CODE BEGIN 4.
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_13)
    {
        buttonCount++;
        // Toggle LED as visual confirmation
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    }
}
// ===== USER CODE END 4 =====
