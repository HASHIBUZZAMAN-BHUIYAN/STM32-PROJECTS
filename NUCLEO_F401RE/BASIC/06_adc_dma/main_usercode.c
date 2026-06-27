/**
 * BASIC/06 — ADC DMA Circular Buffer
 * Target: Nucleo-F401RE (STM32F401RETx)
 *
 * ADC1 IN0 (PA0), DMA2 Stream0 Ch0, Circular, 32 samples
 * UART2 prints average every 500 ms
 *
 * Paste each labeled block into the matching USER CODE section
 * in the CubeMX-generated main.c.
 */

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====


// ===== USER CODE BEGIN PD =====
#define ADC_BUF_SIZE  32U
// ===== USER CODE END PD =====


// ===== USER CODE BEGIN PV =====
static uint16_t adcDmaBuf[ADC_BUF_SIZE];
// ===== USER CODE END PV =====


// ===== USER CODE BEGIN 0 =====
int __io_putchar(int ch)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}
// ===== USER CODE END 0 =====


// ===== USER CODE BEGIN 2 =====
const char *startMsg = "BASIC/06 ADC DMA Demo\r\n";
HAL_UART_Transmit(&huart2, (uint8_t *)startMsg, strlen(startMsg), HAL_MAX_DELAY);

// Start ADC in DMA mode (continuous, circular)
if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adcDmaBuf, ADC_BUF_SIZE) != HAL_OK)
{
    Error_Handler();
}
// ===== USER CODE END 2 =====


// ===== USER CODE BEGIN WHILE =====
while (1)
{
    // Compute average of all 32 samples
    uint32_t sum = 0;
    for (uint32_t i = 0; i < ADC_BUF_SIZE; i++)
    {
        sum += adcDmaBuf[i];
    }
    uint32_t avg = sum / ADC_BUF_SIZE;

    float voltage = (float)avg * 3.3f / 4095.0f;

    char buf[64];
    int len = snprintf(buf, sizeof(buf),
                       "ADC avg: %4lu  Voltage: %1.3f V\r\n",
                       avg, voltage);
    HAL_UART_Transmit(&huart2, (uint8_t *)buf, (uint16_t)len, HAL_MAX_DELAY);

    HAL_Delay(500);
}
// ===== USER CODE END WHILE =====
