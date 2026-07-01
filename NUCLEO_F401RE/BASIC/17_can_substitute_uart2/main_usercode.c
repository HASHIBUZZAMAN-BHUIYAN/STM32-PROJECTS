// ============================================================
// Nucleo-F401RE BASIC/17 — Dual UART Bridge (CAN not on F401)
// STM32F401RETx  |  HAL + CubeMX 6.16
// USART1 (PA9/PA10) <--> USART2 (PA2/PA3, ST-Link VCP)
// ============================================================

// ===== USER CODE BEGIN PV =====
static uint8_t u1_rx, u2_rx;
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 2 =====
    HAL_UART_Transmit(&huart2,
        (uint8_t*)"Dual-UART bridge ready (F401 has no CAN)\r\n", 42, 100);
    HAL_UART_Receive_IT(&huart1, &u1_rx, 1);
    HAL_UART_Receive_IT(&huart2, &u2_rx, 1);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        HAL_Delay(1000);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====

// ===== USER CODE BEGIN 4 =====
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        // Relay USART1 → USART2 (PC)
        HAL_UART_Transmit(&huart2, &u1_rx, 1, 5);
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        HAL_UART_Receive_IT(&huart1, &u1_rx, 1);
    } else if (huart->Instance == USART2) {
        // Relay USART2 (PC) → USART1 (external)
        HAL_UART_Transmit(&huart1, &u2_rx, 1, 5);
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        HAL_UART_Receive_IT(&huart2, &u2_rx, 1);
    }
}
// ===== USER CODE END 4 =====
