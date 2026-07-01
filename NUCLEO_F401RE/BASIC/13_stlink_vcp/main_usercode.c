// ============================================================
// Nucleo-F401RE BASIC/13 — ST-Link VCP Showcase
// STM32F401RETx  |  HAL + CubeMX 6.16
// USART2 (PA2/PA3) → onboard ST-Link → PC USB Virtual COM
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PV =====
static char json_buf[96];
static uint8_t led_state = 0;
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 2 =====
    HAL_UART_Transmit(&huart2, (uint8_t*)"Nucleo VCP ready\r\n", 18, 100);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        // Read ADC (PA0 / Arduino A0)
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, 10);
        uint32_t adc_val = HAL_ADC_GetValue(&hadc1);
        HAL_ADC_Stop(&hadc1);

        // Calculate voltage (3300 mV reference, 12-bit ADC)
        uint32_t volt_mv = (adc_val * 3300) / 4095;

        // Toggle LED every cycle
        led_state ^= 1;
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, led_state ? GPIO_PIN_SET : GPIO_PIN_RESET);

        // Format and transmit JSON
        int n = snprintf(json_buf, sizeof(json_buf),
            "{\"tick\":%lu,\"led\":%u,\"adc\":%lu,\"volt_mv\":%lu}\r\n",
            HAL_GetTick(), led_state, adc_val, volt_mv);
        HAL_UART_Transmit(&huart2, (uint8_t*)json_buf, n, 100);

        HAL_Delay(500);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====
