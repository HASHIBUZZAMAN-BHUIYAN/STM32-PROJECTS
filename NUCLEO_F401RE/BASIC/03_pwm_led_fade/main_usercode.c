/**
 * BASIC/03 — PWM LED Fade
 * Target: Nucleo-F401RE (STM32F401RETx)
 *
 * TIM1 CH1 on PA8
 * PSC=83, ARR=999 → 1 kHz PWM at 84 MHz
 * Duty cycle 0–100% mapped to CCR1 0–999
 *
 * Paste each labeled block into the matching USER CODE section
 * in the CubeMX-generated main.c.
 */

// ===== USER CODE BEGIN 2 =====
// Start PWM on TIM1 Channel 1
if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1) != HAL_OK)
{
    Error_Handler();
}
// ===== USER CODE END 2 =====


// ===== USER CODE BEGIN WHILE =====
while (1)
{
    // Fade in: 0% → 100%
    for (uint32_t duty = 0; duty <= 999; duty++)
    {
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, duty);
        HAL_Delay(2);  // 2 ms per step → ~2 s total fade-in
    }

    // Fade out: 100% → 0%
    for (int32_t duty = 999; duty >= 0; duty--)
    {
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, (uint32_t)duty);
        HAL_Delay(2);  // 2 ms per step → ~2 s total fade-out
    }
}
// ===== USER CODE END WHILE =====
