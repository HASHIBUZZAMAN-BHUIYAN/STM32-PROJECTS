// ============================================================
// Black Pill BASIC/06 — ADC DMA Continuous Sampling
// STM32F411CEU6  |  HAL + CubeMX 6.16  |  100 MHz
// ADC1: IN1(PA1) + IN18(internal temp), DMA2 Stream0 Circular
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PV =====
static uint16_t adc_buf[2]; // [0]=PA1, [1]=internal temp
static char buf[80];
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 2 =====
    // Enable internal temp sensor reference (required for IN18)
    ADC->CCR |= ADC_CCR_TSVREFE;
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buf, 2);
    HAL_UART_Transmit(&huart1,
        (uint8_t*)"ADC DMA demo: PA1 + internal temp\r\n", 36, 100);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        // adc_buf is always up-to-date — DMA fills it continuously
        uint32_t pa1_mv  = ((uint32_t)adc_buf[0] * 3300U) / 4096U;
        // Internal temp: V25=760mV, slope=2.5mV/°C
        float v_sense    = ((float)adc_buf[1] * 3300.0f) / 4096.0f;
        float mcu_temp   = ((v_sense - 760.0f) / 2.5f) + 25.0f;

        int n = snprintf(buf, sizeof(buf),
                         "PA1=%u(%lumV)  MCU_temp=%.1fC\r\n",
                         adc_buf[0], pa1_mv, (double)mcu_temp);
        HAL_UART_Transmit(&huart1, (uint8_t*)buf, n, 100);

        HAL_Delay(1000);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====
