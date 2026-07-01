// ============================================================
// Black Pill BASIC/15 — Multiple Timer Channels (TIM1)
// STM32F411CEU6  |  HAL + CubeMX 6.16  |  100 MHz
// TIM1: PSC=99, ARR=999 (1kHz), CH1=PA8, CH4=PA11
// ============================================================

// ===== USER CODE BEGIN PV =====
static uint16_t duty1 = 100; // 10%
static uint16_t duty4 = 700; // 70%
static int8_t   dir1  = 1;
static int8_t   dir4  = -1;
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 2 =====
    // Start both PWM channels
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);

    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, duty1);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, duty4);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        HAL_Delay(20); // update every 20ms

        // CH1: sweep 0→100→0
        duty1 = (uint16_t)(duty1 + dir1 * 10);
        if (duty1 >= 1000) { duty1 = 1000; dir1 = -1; }
        if (duty1 == 0)    { duty1 = 0;    dir1 =  1; }
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, duty1);

        // CH4: sweep 0→100→0 in opposite direction
        if ((int)duty4 + dir4 * 10 < 0) { duty4 = 0; dir4 = 1; }
        else if (duty4 + dir4 * 10 > 1000) { duty4 = 1000; dir4 = -1; }
        else duty4 = (uint16_t)(duty4 + dir4 * 10);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, duty4);

        // PC13 blink every 50 cycles
        static uint8_t cnt = 0;
        if (++cnt >= 50) { cnt = 0; HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13); }
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====
