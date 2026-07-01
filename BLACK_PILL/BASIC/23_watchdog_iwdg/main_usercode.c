// ============================================================
// Black Pill BASIC/23 — Independent Watchdog (IWDG)
// STM32F411CEU6  |  HAL + CubeMX 6.16  |  100 MHz
// IWDG 2s timeout, USART1 command interface
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PV =====
static volatile uint8_t rx_byte;
static char cmd_buf[16];
static uint8_t cmd_idx = 0;
static volatile uint8_t cmd_ready = 0;
static uint32_t kick_count = 0;
static char buf[64];
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static void uart_tx(const char *s) {
    HAL_UART_Transmit(&huart1, (const uint8_t*)s, strlen(s), 200);
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    // Detect if we woke from IWDG reset
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST)) {
        __HAL_RCC_CLEAR_RESET_FLAGS();
        uart_tx("*** Woke from IWDG RESET ***\r\n");
    } else {
        uart_tx("Black Pill IWDG demo. Commands: hang\r\n");
    }

    HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        if (cmd_ready) {
            cmd_ready = 0;
            if (strncmp(cmd_buf, "hang", 4) == 0) {
                uart_tx("Hanging now — IWDG will reset in 2s!\r\n");
                while (1); // stall — watchdog fires
            }
            cmd_idx = 0;
            memset(cmd_buf, 0, sizeof(cmd_buf));
        }

        // Kick IWDG every 1 second (timeout is 2s — comfortable margin)
        HAL_IWDG_Refresh(&hiwdg);
        kick_count++;

        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        int n = snprintf(buf, sizeof(buf), "WDG kick #%lu\r\n", kick_count);
        uart_tx(buf);
        (void)n;
        HAL_Delay(1000);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====

// ===== USER CODE BEGIN 4 =====
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        char c = (char)rx_byte;
        if (c == '\r' || c == '\n') {
            cmd_buf[cmd_idx] = '\0';
            if (cmd_idx > 0) cmd_ready = 1;
        } else if (cmd_idx < sizeof(cmd_buf)-1) {
            cmd_buf[cmd_idx++] = c;
        }
        HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
    }
}
// ===== USER CODE END 4 =====
