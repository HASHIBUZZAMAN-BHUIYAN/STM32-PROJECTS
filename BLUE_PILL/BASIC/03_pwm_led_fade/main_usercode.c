/**
 * @file    main_usercode.c
 * @brief   03 — PWM LED Fade (TIM1 CH1 on PA8)
 *          Blue Pill STM32F103C8T6
 *
 * PWM configuration (set in CubeMX):
 *   TIM1 PSC = 71  -> Timer clock = 72 MHz / 72 = 1 MHz
 *   TIM1 ARR = 999 -> PWM period  = 1000 ticks = 1 ms -> 1 kHz
 *   Duty cycle (%) = CCR1 / 1000 * 100
 *
 * The sweep loop changes CCR1 from 0 to ARR (0–100%) then back to 0.
 * 2 ms delay per step -> ~4-second full sweep cycle.
 *
 * Copy each block into the matching USER CODE BEGIN/END region
 * inside the CubeMX-generated Core/Src/main.c.
 *
 * NOTE: This code has NOT been tested on hardware. Review before flashing.
 *
 * CubeMX requirements:
 *   - TIM1 CH1: PWM Generation, PSC=71, ARR=999, Pulse=0
 *   - PA8: TIM1_CH1 alternate function (auto-set by CubeMX)
 */

/* ===== USER CODE BEGIN Includes ===== */
/* No extra includes needed */
/* ===== USER CODE END Includes ===== */


/* ===== USER CODE BEGIN PD (Private Defines) ===== */
#define PWM_MAX_CCR     999U    /* Matches ARR — 100% duty cycle */
#define FADE_STEP_MS    2U      /* Delay between CCR increments (ms) */
/* ===== USER CODE END PD ===== */


/* ===== USER CODE BEGIN PV (Private Variables) ===== */
/* None */
/* ===== USER CODE END PV ===== */


/* ===== USER CODE BEGIN 2 ===== */

    /* Start PWM output on TIM1 Channel 1 (PA8).
     * htim1 is declared by the generated code. */
    if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1) != HAL_OK)
    {
        /* PWM start failed — trap here for debugging */
        Error_Handler();
    }

/* ===== USER CODE END 2 ===== */


/* ===== USER CODE BEGIN WHILE ===== */

    /* Fade UP: sweep duty from 0% to 100% */
    for (uint32_t ccr = 0; ccr <= PWM_MAX_CCR; ccr++)
    {
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, ccr);
        HAL_Delay(FADE_STEP_MS);
    }

    /* Fade DOWN: sweep duty from 100% back to 0% */
    for (uint32_t ccr = PWM_MAX_CCR; ccr > 0; ccr--)
    {
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, ccr);
        HAL_Delay(FADE_STEP_MS);
    }

    /* Brief pause at 0% before repeating */
    HAL_Delay(200U);

/* ===== USER CODE END WHILE ===== */
