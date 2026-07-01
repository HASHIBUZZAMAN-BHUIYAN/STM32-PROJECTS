// ============================================================
// Black Pill BASIC/25 — NVIC Interrupt Priority & Preemption
// STM32F411CEU6  |  HAL + CubeMX 6.16  |  100 MHz
// TIM2(P0) > USART1_RX(P1) > EXTI0_PA0(P2)
// ============================================================

// ===== USER CODE BEGIN PV =====
static volatile uint32_t g_timestamp = 0;  // incremented by TIM2 at 1kHz
static volatile uint32_t u1_tim_during = 0; // TIM2 count during UART ISR
static uint8_t rx_byte;
static char echo_buf[48];
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 2 =====
    HAL_TIM_Base_Start_IT(&htim2); // 1 kHz, priority 0
    HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
    HAL_UART_Transmit(&huart1,
        (uint8_t*)"NVIC preemption: TIM2(P0)>UART(P1)>PA0(P2)\r\n"
                  "Type chars, press PA0 button\r\n", 77, 100);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        static uint32_t last_print = 0;
        if (HAL_GetTick() - last_print >= 2000) {
            last_print = HAL_GetTick();
            int n = snprintf(echo_buf, sizeof(echo_buf),
                             "TIM2 count=%lu\r\n", g_timestamp);
            HAL_UART_Transmit(&huart1, (uint8_t*)echo_buf, n, 100);
            (void)n;
        }
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====

// ===== USER CODE BEGIN 4 =====
/* TIM2 1kHz (priority 0) — preempts UART and EXTI ISRs */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) g_timestamp++;
}

/* USART1 RX (priority 1) — TIM2 can preempt this */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        uint32_t t0 = g_timestamp;
        // Simulate work — TIM2 fires N times during this loop
        for (volatile int i = 0; i < 2000; i++);
        u1_tim_during = g_timestamp - t0;
        HAL_UART_Transmit(&huart1, &rx_byte, 1, 5);
        if (rx_byte == '\r') {
            int n = snprintf(echo_buf, sizeof(echo_buf),
                "\r\nTIM2 fired %lu times during UART ISR\r\n", u1_tim_during);
            HAL_UART_Transmit(&huart1, (uint8_t*)echo_buf, n, 100);
            (void)n;
        }
        HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
    }
}

/* PA0 EXTI (priority 2 — lowest, cannot preempt others) */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_0) {
        int n = snprintf(echo_buf, sizeof(echo_buf),
                         "PA0 (P2, TIM2=%lu)\r\n", g_timestamp);
        HAL_UART_Transmit(&huart1, (uint8_t*)echo_buf, n, 50);
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        (void)n;
    }
}
// ===== USER CODE END 4 =====
