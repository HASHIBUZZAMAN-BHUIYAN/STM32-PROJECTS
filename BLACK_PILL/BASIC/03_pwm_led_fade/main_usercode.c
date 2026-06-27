/**
 ******************************************************************************
 * @file    main_usercode.c
 * @project 03_pwm_led_fade
 * @board   STM32F411CEU6 Black Pill
 * @brief   User code sections to paste into CubeMX-generated main.c
 *
 * INSTRUCTIONS:
 *   Copy each section into the matching USER CODE BEGIN/END block inside
 *   the CubeMX-generated Core/Src/main.c file.
 *
 * HARDWARE:
 *   PA8 — TIM1 CH1 PWM output (AF1). Connect to LED via 330Ω resistor to GND.
 *
 * PWM PARAMETERS (set in CubeMX):
 *   PSC = 99, ARR = 999
 *   f_PWM = 100MHz / 100 / 1000 = 1 kHz
 *   Duty  = CCR1 / 1000  (0 = 0%, 1000 = 100%)
 *
 * FADE TIMING:
 *   Steps: 0, 10, 20, ... 1000, 990, ... 0  (101 steps up + 101 steps down)
 *   Delay: 10 ms per step
 *   Total: ~1 s per direction → ~2 s per full breath cycle
 *
 * NOTE: This code has not been tested on hardware. Verify before use.
 ******************************************************************************
 */

/* ===== USER CODE BEGIN Includes ===== */
/* No additional includes needed */
/* ===== USER CODE END Includes ===== */


/* ===== USER CODE BEGIN PD ===== */
#define PWM_STEP_DELAY_MS  10U   /* Delay between each duty-cycle step (ms) */
#define PWM_MAX_CCR        1000U /* Matches ARR = 999, so CCR=1000 = 100% duty */
#define PWM_STEP_SIZE      10U   /* CCR increment/decrement per step (1% of range) */
/* ===== USER CODE END PD ===== */


/* ===== USER CODE BEGIN PV ===== */
/* No private variables needed at file scope */
/* ===== USER CODE END PV ===== */


/* ===== USER CODE BEGIN PFP ===== */
/* ===== USER CODE END PFP ===== */


/* ===== USER CODE BEGIN 0 ===== */
/* ===== USER CODE END 0 ===== */


/* ===== USER CODE BEGIN 2 ===== */
/*
 * Start PWM output on TIM1 Channel 1 (PA8).
 * HAL_TIM_PWM_Start enables the timer and sets the output compare register.
 * Must be called AFTER MX_TIM1_Init().
 */
if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1) != HAL_OK)
{
    /* PWM failed to start — halt here so the problem is obvious */
    Error_Handler();
}

/* Set initial duty cycle to 0% (LED off at startup) */
__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0U);
/* ===== USER CODE END 2 ===== */


/* ===== USER CODE BEGIN WHILE ===== */
while (1)
{
    /*
     * FADE UP: 0% → 100% duty cycle
     * CCR steps from 0 to PWM_MAX_CCR in increments of PWM_STEP_SIZE.
     * Each step increases brightness by 1% and waits 10 ms.
     */
    for (uint32_t ccr = 0U; ccr <= PWM_MAX_CCR; ccr += PWM_STEP_SIZE)
    {
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, ccr);
        HAL_Delay(PWM_STEP_DELAY_MS);
    }

    /*
     * FADE DOWN: 100% → 0% duty cycle
     * Use int32_t so the comparison (duty >= 0) works correctly when
     * the value reaches 0 and the subtraction would otherwise wrap.
     */
    for (int32_t ccr = (int32_t)PWM_MAX_CCR; ccr >= 0; ccr -= (int32_t)PWM_STEP_SIZE)
    {
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, (uint32_t)ccr);
        HAL_Delay(PWM_STEP_DELAY_MS);
    }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
}
/* ===== USER CODE END 3 ===== */
