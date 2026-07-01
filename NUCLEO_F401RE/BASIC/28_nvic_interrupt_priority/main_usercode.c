// ============================================================
// Nucleo-F401RE BASIC/28 — NVIC Interrupt Priority Demo
// STM32F401RETx  |  HAL + CubeMX 6.16
// TIM2 (P0) > USART2 RX (P1) > EXTI13 B1 (P2)
// ============================================================

// ===== USER CODE BEGIN PV =====
static volatile uint32_t tim2_count = 0;   // incremented in TIM2 ISR
static volatile uint32_t u2_tim_during = 0; // TIM2 fires count during UART ISR
static uint8_t rx_byte;
static uint32_t tim_at_uart_enter = 0;
static char buf[64];
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 2 =====
    HAL_TIM_Base_Start_IT(&htim2);  // start 1kHz interrupt
    HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
    HAL_UART_Transmit(&huart2,
        (uint8_t*)"NVIC priority demo: TIM2>UART>B1\r\n"
                   "Type chars + press B1\r\n", 57, 100);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        // Print TIM2 count every 2 seconds
        HAL_Delay(2000);
        int n = snprintf(buf, sizeof(buf), "TIM2 count=%lu\r\n", tim2_count);
        HAL_UART_Transmit(&huart2, (uint8_t*)buf, n, 100);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====

// ===== USER CODE BEGIN 4 =====
/* TIM2 Period Elapsed (1kHz, priority 0 — preempts everything) */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        tim2_count++;
    }
}

/* USART2 RX (priority 1) — TIM2 can preempt this ISR */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        tim_at_uart_enter = tim2_count;
        // Simulate some work — TIM2 fires multiple times during this
        for (volatile int i = 0; i < 1000; i++);
        u2_tim_during = tim2_count - tim_at_uart_enter;

        // Echo received byte + show TIM2 preemption count
        HAL_UART_Transmit(&huart2, &rx_byte, 1, 5);
        if (rx_byte == '\r') {
            int n = snprintf(buf, sizeof(buf),
                "\r\nTIM2 fired %lu times during UART ISR\r\n", u2_tim_during);
            HAL_UART_Transmit(&huart2, (uint8_t*)buf, n, 100);
        }
        HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
    }
}

/* B1 EXTI13 (priority 2 — lowest, cannot preempt UART or TIM2) */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_13) {
        int n = snprintf(buf, sizeof(buf),
            "B1 pressed (P2, TIM2=%lu)\r\n", tim2_count);
        HAL_UART_Transmit(&huart2, (uint8_t*)buf, n, 100);
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    }
}
// ===== USER CODE END 4 =====
