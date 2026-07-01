// ============================================================
// Nucleo-F401RE BASIC/16 — Multiple Timer Channels
// STM32F401RETx  |  HAL + CubeMX 6.16
// TIM2 CH1/CH2 (PA0/PA1) + TIM3 CH1/CH2 (PA6/PA7)
// ============================================================

// ===== USER CODE BEGIN 2 =====
    // Start all 4 PWM channels simultaneously
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);  // PA0 — 1 Hz
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);  // PA1 — 2 Hz
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);  // PA6 — 4 Hz
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);  // PA7 — 8 Hz

    // Set 50% duty on each channel
    // TIM2 CH1: ARR=999, CCR=500 → 50%
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 500);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 250);  // ARR=499, CCR=250 → 50%
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 125);  // ARR=249, CCR=125 → 50%
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 62);   // ARR=124, CCR=62  → ~50%

    HAL_UART_Transmit(&huart2,
        (uint8_t*)"4 PWM channels: PA0=1Hz PA1=2Hz PA6=4Hz PA7=8Hz\r\n", 51, 100);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        // Nothing to do — timers run in hardware
        HAL_Delay(5000);
        HAL_UART_Transmit(&huart2, (uint8_t*)"Running...\r\n", 12, 100);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====
