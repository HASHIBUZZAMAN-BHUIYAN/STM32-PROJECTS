/**
 * @file    main_usercode.c
 * @brief   DC Motor H-Bridge Control (L298N) — Blue Pill (STM32F103C8T6)
 *
 * Paste each labeled section into the matching USER CODE block inside the
 * CubeMX-generated main.c. Do NOT replace the entire main.c with this file.
 *
 * CubeMX peripherals required:
 *   - TIM1 CH1 PWM on PA8: PSC=71, ARR=999 → 1 kHz PWM
 *   - PB0: GPIO Output Push-Pull (L298N IN1 — direction)
 *   - PB1: GPIO Output Push-Pull (L298N IN2 — direction)
 *
 * L298N ENA pin connected to PA8 (PWM). IN1 = PB0, IN2 = PB1.
 *
 * NOT tested on hardware — provided as a reference.
 */

// ===== USER CODE BEGIN Includes =====
/* No extra includes required */
// ===== USER CODE END Includes =====


// ===== USER CODE BEGIN PD (Private Defines) =====
/* Motor speed as CCR1 value (0–999 for 0–100% duty at ARR=999) */
#define MOTOR_SPEED_70PCT   700U   /* 70% duty cycle */
#define MOTOR_SPEED_STOP    0U     /* 0% duty = no drive */
// ===== USER CODE END PD =====


// ===== USER CODE BEGIN PV (Private Variables) =====
/**
 * @brief Set motor speed by updating TIM1 CH1 compare register.
 * @param speed  0 (stop) to 999 (full speed), maps to 0–100% duty.
 */
static void Motor_SetSpeed(uint32_t speed)
{
    if (speed > 999U) speed = 999U;
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, speed);
}

/**
 * @brief Drive motor forward (PB0=HIGH, PB1=LOW).
 */
static void Motor_Forward(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);   /* IN1 HIGH */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET); /* IN2 LOW  */
}

/**
 * @brief Drive motor in reverse (PB0=LOW, PB1=HIGH).
 */
static void Motor_Reverse(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); /* IN1 LOW  */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);   /* IN2 HIGH */
}

/**
 * @brief Coast stop — both direction pins LOW, PWM can be left running.
 *        For hard brake set both IN1 and IN2 HIGH.
 */
static void Motor_Coast(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); /* IN1 LOW */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET); /* IN2 LOW */
}
// ===== USER CODE END PV =====


// ===== USER CODE BEGIN 2 (after MX_xxx_Init calls, before while loop) =====
    /* Start TIM1 CH1 PWM output on PA8 */
    if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }

    /* Ensure motor starts in coast state */
    Motor_Coast();
    Motor_SetSpeed(MOTOR_SPEED_STOP);
    HAL_Delay(200);
// ===== USER CODE END 2 =====


// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        /* --- Forward at 70% speed for 2 seconds --- */
        Motor_SetSpeed(MOTOR_SPEED_70PCT);
        Motor_Forward();
        HAL_Delay(2000);

        /* --- Coast stop for 1 second --- */
        Motor_Coast();
        Motor_SetSpeed(MOTOR_SPEED_STOP);
        HAL_Delay(1000);

        /* --- Reverse at 70% speed for 2 seconds --- */
        Motor_SetSpeed(MOTOR_SPEED_70PCT);
        Motor_Reverse();
        HAL_Delay(2000);

        /* --- Coast stop for 1 second --- */
        Motor_Coast();
        Motor_SetSpeed(MOTOR_SPEED_STOP);
        HAL_Delay(1000);
    }
// ===== USER CODE END WHILE =====
