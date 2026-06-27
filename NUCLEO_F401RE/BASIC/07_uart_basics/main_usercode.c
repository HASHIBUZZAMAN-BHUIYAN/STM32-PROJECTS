/**
 * BASIC/07 — UART Basics
 * Target: Nucleo-F401RE (STM32F401RETx)
 *
 * USART2 (PA2 TX, PA3 RX) = ST-Link VCP, 115200 8N1
 * TX: "Hello Nucleo\r\n" every 1 s
 * RX: 1 s timeout poll, echo back received char
 *
 * Paste each labeled block into the matching USER CODE section
 * in the CubeMX-generated main.c.
 */

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====


// ===== USER CODE BEGIN 2 =====
const char *startMsg = "BASIC/07 UART Demo — Hello Nucleo!\r\n";
HAL_UART_Transmit(&huart2, (uint8_t *)startMsg, strlen(startMsg), HAL_MAX_DELAY);
// ===== USER CODE END 2 =====


// ===== USER CODE BEGIN WHILE =====
while (1)
{
    // Transmit periodic message
    const char *txMsg = "Hello Nucleo\r\n";
    HAL_UART_Transmit(&huart2, (uint8_t *)txMsg, strlen(txMsg), HAL_MAX_DELAY);

    // Attempt to receive 1 byte (1000 ms timeout)
    uint8_t rxByte = 0;
    HAL_StatusTypeDef status = HAL_UART_Receive(&huart2, &rxByte, 1, 1000);

    if (status == HAL_OK)
    {
        // Echo back with "Echo: X\r\n"
        char echoBuf[32];
        int len = snprintf(echoBuf, sizeof(echoBuf), "Echo: %c\r\n", rxByte);
        HAL_UART_Transmit(&huart2, (uint8_t *)echoBuf, (uint16_t)len, HAL_MAX_DELAY);
    }
    // HAL_TIMEOUT is normal if no byte received within 1 s — continue loop
}
// ===== USER CODE END WHILE =====
