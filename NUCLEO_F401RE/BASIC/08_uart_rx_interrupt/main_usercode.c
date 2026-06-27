/**
 * BASIC/08 — UART RX Interrupt with Line Accumulator
 * Target: Nucleo-F401RE (STM32F401RETx)
 *
 * USART2 (PA2/PA3, 115200 8N1) RX interrupt
 * Accumulates to line buffer, echoes on '\n'
 * Re-arms RX interrupt in callback
 *
 * Paste each labeled block into the matching USER CODE section
 * in the CubeMX-generated main.c.
 */

// ===== USER CODE BEGIN Includes =====
#include <string.h>
#include <stdio.h>
// ===== USER CODE END Includes =====


// ===== USER CODE BEGIN PD =====
#define RX_LINE_BUF_SIZE  128U
// ===== USER CODE END PD =====


// ===== USER CODE BEGIN PV =====
static uint8_t  rxByte       = 0;           // Single-byte RX buffer for interrupt
static char     lineBuf[RX_LINE_BUF_SIZE];  // Line accumulation buffer
static uint16_t lineIdx      = 0;           // Current position in lineBuf
static volatile uint8_t lineReady = 0;      // Flag: complete line received
// ===== USER CODE END PV =====


// ===== USER CODE BEGIN 2 =====
const char *startMsg = "BASIC/08 UART RX Interrupt Demo\r\nType a line and press Enter:\r\n";
HAL_UART_Transmit(&huart2, (uint8_t *)startMsg, strlen(startMsg), HAL_MAX_DELAY);

// Arm first RX interrupt (1 byte at a time)
HAL_UART_Receive_IT(&huart2, &rxByte, 1);
// ===== USER CODE END 2 =====


// ===== USER CODE BEGIN WHILE =====
while (1)
{
    if (lineReady)
    {
        lineReady = 0;

        // Echo the complete line
        char echoBuf[RX_LINE_BUF_SIZE + 16];
        int len = snprintf(echoBuf, sizeof(echoBuf), "Echo: %s\r\n", lineBuf);
        HAL_UART_Transmit(&huart2, (uint8_t *)echoBuf, (uint16_t)len, HAL_MAX_DELAY);

        // Clear line buffer
        memset(lineBuf, 0, sizeof(lineBuf));
        lineIdx = 0;
    }

    // Blink LED to show main loop is running (non-blocking)
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    HAL_Delay(500);
}
// ===== USER CODE END WHILE =====


// ===== USER CODE BEGIN 4 =====
/**
 * UART RX Complete Callback
 * Called each time HAL_UART_Receive_IT receives 1 byte.
 * Accumulate into lineBuf, set lineReady flag on '\n', re-arm.
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        if (rxByte == '\n' || rxByte == '\r')
        {
            if (lineIdx > 0)
            {
                lineBuf[lineIdx] = '\0';  // Null-terminate
                lineReady = 1;
            }
            // else: ignore empty lines
        }
        else if (lineIdx < (RX_LINE_BUF_SIZE - 1))
        {
            lineBuf[lineIdx++] = (char)rxByte;
        }
        // If buffer full, silently discard (lineIdx not incremented)

        // Re-arm interrupt for next byte
        HAL_UART_Receive_IT(&huart2, &rxByte, 1);
    }
}
// ===== USER CODE END 4 =====
