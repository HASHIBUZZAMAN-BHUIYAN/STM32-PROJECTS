// ============================================================
// Nucleo-F401RE BASIC/24 — Multi-UART Simultaneous
// USART1 (PA9/PA10) + USART2 (PA2/PA3 / ST-Link VCP)
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <string.h>
#include <stdio.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PV =====
static uint8_t u1_rx, u2_rx;
static char u2_line[64]; static uint8_t u2_idx = 0;
static char u1_line[64]; static uint8_t u1_idx = 0;
static volatile uint8_t u1_ready = 0, u2_ready = 0;
static uint32_t u1_count = 0, u2_count = 0;
static char buf[80];
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 2 =====
    HAL_UART_Transmit(&huart2, (uint8_t*)"Multi-UART ready\r\n", 18, 100);
    HAL_UART_Receive_IT(&huart1, &u1_rx, 1);
    HAL_UART_Receive_IT(&huart2, &u2_rx, 1);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        if (u2_ready) {
            u2_ready = 0; u2_count++;
            if (strncmp(u2_line, "send:", 5) == 0) {
                // Forward to USART1
                HAL_UART_Transmit(&huart1, (uint8_t*)&u2_line[5],
                                  strlen(&u2_line[5]), 50);
                HAL_UART_Transmit(&huart2, (uint8_t*)"Sent to U1\r\n", 12, 50);
            } else if (strcmp(u2_line, "status") == 0) {
                int n = snprintf(buf, sizeof(buf), "U1rx=%lu U2rx=%lu\r\n", u1_count, u2_count);
                HAL_UART_Transmit(&huart2, (uint8_t*)buf, n, 100);
            }
        }
        if (u1_ready) {
            u1_ready = 0; u1_count++;
            int n = snprintf(buf, sizeof(buf), "U1: %s\r\n", u1_line);
            HAL_UART_Transmit(&huart2, (uint8_t*)buf, n, 100);
        }
        HAL_Delay(5);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====

// ===== USER CODE BEGIN 4 =====
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        if (u1_rx == '\n' || u1_rx == '\r') {
            if (u1_idx > 0) { u1_line[u1_idx] = '\0'; u1_idx = 0; u1_ready = 1; }
        } else if (u1_idx < 63) u1_line[u1_idx++] = u1_rx;
        HAL_UART_Receive_IT(&huart1, &u1_rx, 1);
    } else if (huart->Instance == USART2) {
        if (u2_rx == '\n' || u2_rx == '\r') {
            if (u2_idx > 0) { u2_line[u2_idx] = '\0'; u2_idx = 0; u2_ready = 1; }
        } else if (u2_idx < 63) u2_line[u2_idx++] = u2_rx;
        HAL_UART_Receive_IT(&huart2, &u2_rx, 1);
    }
}
// ===== USER CODE END 4 =====
