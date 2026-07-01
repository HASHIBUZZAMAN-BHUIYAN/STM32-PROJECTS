// ============================================================
// Black Pill BASIC/07 — UART Basics
// STM32F411CEU6  |  HAL + CubeMX 6.16  |  100 MHz
// USART1: PA9=TX, PA10=RX, 115200 8N1
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PV =====
static uint8_t rx_byte;
static char echo_buf[32];
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 2 =====
    const char *banner = "Black Pill UART ready. Type something!\r\n";
    HAL_UART_Transmit(&huart1, (const uint8_t*)banner, strlen(banner), 200);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        // Blocking receive: waits up to 100ms for one byte
        if (HAL_UART_Receive(&huart1, &rx_byte, 1, 100) == HAL_OK) {
            int n = snprintf(echo_buf, sizeof(echo_buf), "Echo: %c (0x%02X)\r\n",
                             (rx_byte >= 32 && rx_byte < 127) ? rx_byte : '.',
                             rx_byte);
            HAL_UART_Transmit(&huart1, (uint8_t*)echo_buf, n, 100);

            // Toggle LED on each received byte
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        }
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====
