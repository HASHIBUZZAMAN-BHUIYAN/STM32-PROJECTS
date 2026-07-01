// ============================================================
// Nucleo-F401RE BASIC/09 — I2C Bus Scan
// STM32F401RETx  |  HAL + CubeMX 6.16
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PV =====
static char uart_buf[64];
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 2 =====
    HAL_UART_Transmit(&huart2, (uint8_t*)"I2C Bus Scan starting...\r\n", 26, 100);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        HAL_UART_Transmit(&huart2, (uint8_t*)"\r\nI2C Bus Scan (0x01-0x7F):\r\n", 30, 100);
        int found = 0;
        for (uint8_t addr = 1; addr < 128; addr++) {
            if (HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(addr << 1), 3, 10) == HAL_OK) {
                int n = snprintf(uart_buf, sizeof(uart_buf), "  Found device at 0x%02X\r\n", addr);
                HAL_UART_Transmit(&huart2, (uint8_t*)uart_buf, n, 100);
                found++;
            }
        }
        int n = snprintf(uart_buf, sizeof(uart_buf), "Scan complete. %d device(s) found.\r\n", found);
        HAL_UART_Transmit(&huart2, (uint8_t*)uart_buf, n, 100);
        HAL_Delay(3000);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====
