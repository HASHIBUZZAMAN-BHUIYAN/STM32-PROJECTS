/**
 * @file    main_usercode.c
 * @brief   Servo PWM Sweep — Blue Pill (STM32F103C8T6)
 *
 * Paste each labeled section into the matching USER CODE block inside the
 * CubeMX-generated main.c. Do NOT replace the entire main.c with this file.
 *
 * CubeMX peripherals required:
 *   - TIM2: CH2 PWM, PSC=71, ARR=19999 → 50 Hz on PA1
 *
 * Timer math:
 *   System clock = 72 MHz
 *   PSC = 71  →  Timer clock = 72 MHz / 72 = 1 MHz
 *   ARR = 19999  →  Period = 20 000 / 1 MHz = 20 ms (50 Hz)
 *   CCR2 = 1000 → 1 ms pulse = 0°
 *   CCR2 = 1500 → 1.5 ms pulse = 90°
 *   CCR2 = 2000 → 2 ms pulse = 180°
 *
 * NOT tested on hardware — provided as a reference.
 */

// ===== USER CODE BEGIN Includes =====
/* No extra includes required beyond what CubeMX generates */
// ===== USER CODE END Includes =====


// ===== USER CODE BEGIN PD (Private Defines) =====
#define SERVO_MIN_PULSE   1000U   /* 1000 µs → 0°   */
#define SERVO_MID_PULSE   1500U   /* 1500 µs → 90°  */
#define SERVO_MAX_PULSE   2000U   /* 2000 µs → 180° */
#define SERVO_STEP        10U     /* step size in µs */
#define SERVO_STEP_DELAY  20U     /* ms delay between steps */
// ===== USER CODE END PD =====


// ===== USER CODE BEGIN PV (Private Variables) =====
/* Helper: set servo pulse width in microseconds (1000–2000 µs) */
static void Servo_SetPulse(TIM_HandleTypeDef *htim, uint32_t channel, uint32_t pulse_us)
{
    /* Clamp to safe range */
    if (pulse_us < SERVO_MIN_PULSE) pulse_us = SERVO_MIN_PULSE;
    if (pulse_us > SERVO_MAX_PULSE) pulse_us = SERVO_MAX_PULSE;

    __HAL_TIM_SET_COMPARE(htim, channel, pulse_us);
}
// ===== USER CODE END PV =====


// ===== USER CODE BEGIN 2 (after MX_xxx_Init calls, before while loop) =====
    /* Start TIM2 CH2 PWM output on PA1 */
    if (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2) != HAL_OK)
    {
        /* Initialization error — halt */
        Error_Handler();
    }

    /* Move to centre position at startup */
    Servo_SetPulse(&htim2, TIM_CHANNEL_2, SERVO_MID_PULSE);
    HAL_Delay(500);
// ===== USER CODE END 2 =====


// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        uint32_t pulse;

        /* Sweep 0° → 180° */
        for (pulse = SERVO_MIN_PULSE; pulse <= SERVO_MAX_PULSE; pulse += SERVO_STEP)
        {
            Servo_SetPulse(&htim2, TIM_CHANNEL_2, pulse);
            HAL_Delay(SERVO_STEP_DELAY);
        }

        /* Brief hold at 180° */
        HAL_Delay(300);

        /* Sweep 180° → 0° */
        for (pulse = SERVO_MAX_PULSE; pulse >= SERVO_MIN_PULSE + SERVO_STEP; pulse -= SERVO_STEP)
        {
            Servo_SetPulse(&htim2, TIM_CHANNEL_2, pulse);
            HAL_Delay(SERVO_STEP_DELAY);
        }

        /* Brief hold at 0° */
        HAL_Delay(300);
    }
// ===== USER CODE END WHILE =====
