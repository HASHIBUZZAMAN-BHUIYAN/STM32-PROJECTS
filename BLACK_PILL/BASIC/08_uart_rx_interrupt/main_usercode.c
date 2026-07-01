// ============================================================
// Black Pill BASIC/08 — UART RX Interrupt + Command Parser
// STM32F411CEU6  |  HAL + CubeMX 6.16  |  100 MHz
// USART1 interrupt RX, PC13 LED (active-LOW)
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <string.h>
#include <stdio.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PV =====
static volatile uint8_t rx_byte;
static char cmd_buf[32];
static uint8_t cmd_idx = 0;
static volatile uint8_t cmd_ready = 0;
static uint8_t led_on = 0;
static char tx_buf[64];
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static void uart_tx(const char *s) {
    HAL_UART_Transmit(&huart1, (const uint8_t*)s, strlen(s), 200);
}

static void led_set(uint8_t on) {
    led_on = on;
    // PC13 active-LOW: RESET = on, SET = off
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, on ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

static void process_command(void) {
    if (strcmp(cmd_buf, "led on") == 0) {
        led_set(1);
        uart_tx("OK: LED on\r\n");
    } else if (strcmp(cmd_buf, "led off") == 0) {
        led_set(0);
        uart_tx("OK: LED off\r\n");
    } else if (strcmp(cmd_buf, "status") == 0) {
        int n = snprintf(tx_buf, sizeof(tx_buf), "Uptime: %lus  LED: %s\r\n",
                         HAL_GetTick()/1000, led_on ? "on" : "off");
        uart_tx(tx_buf);
        (void)n;
    } else if (strcmp(cmd_buf, "help") == 0) {
        uart_tx("Commands: led on, led off, status, help\r\n");
    } else {
        uart_tx("Unknown command. Type 'help'.\r\n");
    }
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    led_set(0); // LED off initially
    HAL_UART_Receive_IT(&huart1, (uint8_t*)&rx_byte, 1);
    uart_tx("Black Pill UART RX interrupt demo.\r\nType: led on/off, status, help\r\n");
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        if (cmd_ready) {
            cmd_ready = 0;
            process_command();
            cmd_idx = 0;
            memset(cmd_buf, 0, sizeof(cmd_buf));
        }

        static uint32_t last_print = 0;
        if (HAL_GetTick() - last_print >= 1000) {
            last_print = HAL_GetTick();
            int n = snprintf(tx_buf, sizeof(tx_buf), "Uptime: %lus  LED: %s\r\n",
                             HAL_GetTick()/1000, led_on ? "on" : "off");
            uart_tx(tx_buf);
            (void)n;
        }
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
        } else if (cmd_idx < sizeof(cmd_buf) - 1) {
            cmd_buf[cmd_idx++] = c;
        }
        HAL_UART_Receive_IT(&huart1, (uint8_t*)&rx_byte, 1); // re-arm
    }
}
// ===== USER CODE END 4 =====
