// ============================================================
// Black Pill BASIC/04 — EXTI Button Interrupt
// STM32F411CEU6  |  HAL + CubeMX 6.16  |  100 MHz
// PA0=Button(rising edge EXTI), PC13=LED(active-LOW), USART1
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PV =====
static volatile uint32_t button_count = 0;
static char buf[48];
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 2 =====
    // LED off initially (PC13 active-LOW: SET = off)
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
    HAL_UART_Transmit(&huart1,
        (uint8_t*)"Black Pill EXTI demo. Press PA0 button.\r\n", 41, 100);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        HAL_Delay(100); // CPU free while waiting for interrupt
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====

// ===== USER CODE BEGIN 4 =====
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_0) {
        button_count++;
        // PC13 active-LOW: toggle
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        int n = snprintf(buf, sizeof(buf), "Button pressed! count=%lu\r\n", button_count);
        HAL_UART_Transmit(&huart1, (uint8_t*)buf, n, 50);
    }
}
// ===== USER CODE END 4 =====
