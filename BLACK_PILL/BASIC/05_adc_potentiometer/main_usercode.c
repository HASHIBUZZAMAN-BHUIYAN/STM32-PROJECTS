// ============================================================
// Black Pill BASIC/05 — ADC Potentiometer Reading
// STM32F411CEU6  |  HAL + CubeMX 6.16  |  100 MHz
// ADC1_IN1 on PA1, USART1 PA9/PA10
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PV =====
static char buf[64];
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 2 =====
    HAL_UART_Transmit(&huart1,
        (uint8_t*)"Black Pill ADC demo. Rotate pot on PA1.\r\n", 41, 100);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        HAL_ADC_Start(&hadc1);
        if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) {
            uint32_t raw = HAL_ADC_GetValue(&hadc1);
            uint32_t mv  = (raw * 3300U) / 4096U;
            uint32_t pct = (raw * 100U) / 4095U;
            int n = snprintf(buf, sizeof(buf),
                             "ADC: %lu = %lumV (%lu%%)\r\n", raw, mv, pct);
            HAL_UART_Transmit(&huart1, (uint8_t*)buf, n, 100);

            // Blink LED proportional to ADC value
            if (raw > 2048) {
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET); // LED on
            } else {
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);   // LED off
            }
        }
        HAL_ADC_Stop(&hadc1);
        HAL_Delay(500);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====
