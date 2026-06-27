/**
 * BASIC/05 — ADC Potentiometer (Poll Mode)
 * Target: Nucleo-F401RE (STM32F401RETx)
 *
 * ADC1 IN0 (PA0 = Arduino A0), 12-bit, poll mode
 * UART2 (115200) prints raw value and voltage every 500 ms
 *
 * Paste each labeled block into the matching USER CODE section
 * in the CubeMX-generated main.c.
 */

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====


// ===== USER CODE BEGIN 0 =====
int __io_putchar(int ch)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}
// ===== USER CODE END 0 =====


// ===== USER CODE BEGIN 2 =====
const char *startMsg = "BASIC/05 ADC Potentiometer Demo\r\n";
HAL_UART_Transmit(&huart2, (uint8_t *)startMsg, strlen(startMsg), HAL_MAX_DELAY);
// ===== USER CODE END 2 =====


// ===== USER CODE BEGIN WHILE =====
while (1)
{
    uint32_t adcValue = 0;

    // Start ADC conversion
    if (HAL_ADC_Start(&hadc1) != HAL_OK)
    {
        Error_Handler();
    }

    // Wait for conversion to complete (100 ms timeout)
    if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK)
    {
        adcValue = HAL_ADC_GetValue(&hadc1);
    }

    HAL_ADC_Stop(&hadc1);

    // Calculate voltage: Vref = 3.3V, 12-bit resolution (4095 steps)
    float voltage = (float)adcValue * 3.3f / 4095.0f;

    char buf[64];
    int len = snprintf(buf, sizeof(buf),
                       "ADC raw: %4lu  Voltage: %1.3f V\r\n",
                       adcValue, voltage);
    HAL_UART_Transmit(&huart2, (uint8_t *)buf, (uint16_t)len, HAL_MAX_DELAY);

    HAL_Delay(500);
}
// ===== USER CODE END WHILE =====
