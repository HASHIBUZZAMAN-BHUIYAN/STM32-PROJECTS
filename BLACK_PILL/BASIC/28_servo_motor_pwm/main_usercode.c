// ============================================================
// Black Pill BASIC/28 — Servo Motor PWM
// STM32F411CEU6  |  HAL + CubeMX 6.16  |  100 MHz
// TIM2_CH1=PA0 (50Hz, PSC=99, ARR=19999), USART1 command
// NOTE: PA0 = servo output (conflicts with user button in this project)
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
#define SERVO_MIN_US  1000  // 0 degrees
#define SERVO_MID_US  1500  // 90 degrees
#define SERVO_MAX_US  2000  // 180 degrees
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
static volatile uint8_t rx_byte;
static char cmd_buf[32];
static uint8_t cmd_idx = 0;
static volatile uint8_t cmd_ready = 0;
static uint8_t sweep_mode = 0;
static char buf[48];
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static void servo_set_us(uint32_t pulse_us) {
    if (pulse_us < SERVO_MIN_US) pulse_us = SERVO_MIN_US;
    if (pulse_us > SERVO_MAX_US) pulse_us = SERVO_MAX_US;
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, pulse_us);
}

static void servo_set_angle(int angle) {
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;
    uint32_t pulse = SERVO_MIN_US + (uint32_t)(angle * (SERVO_MAX_US - SERVO_MIN_US) / 180);
    servo_set_us(pulse);
    int n = snprintf(buf, sizeof(buf), "Servo: %d° (%lu us)\r\n", angle, pulse);
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, n, 100);
    (void)n;
}

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
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    servo_set_us(SERVO_MID_US); // center position
    HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
    HAL_UART_Transmit(&huart1,
        (uint8_t*)"Servo control. Cmds: angle <0-180>, sweep\r\n", 44, 100);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        if (cmd_ready) {
            cmd_ready = 0;
            sweep_mode = 0;
            if (strncmp(cmd_buf, "angle ", 6) == 0) {
                int a = atoi(cmd_buf + 6);
                servo_set_angle(a);
            } else if (strcmp(cmd_buf, "sweep") == 0) {
                HAL_UART_Transmit(&huart1, (uint8_t*)"Sweep mode\r\n", 12, 50);
                sweep_mode = 1;
            } else {
                HAL_UART_Transmit(&huart1, (uint8_t*)"Cmds: angle <0-180>, sweep\r\n", 28, 50);
            }
            cmd_idx = 0;
            memset(cmd_buf, 0, sizeof(cmd_buf));
        }

        if (sweep_mode) {
            // Sweep 0→180→0
            for (int a = 0; a <= 180 && sweep_mode; a += 5) {
                servo_set_angle(a); HAL_Delay(30);
            }
            for (int a = 180; a >= 0 && sweep_mode; a -= 5) {
                servo_set_angle(a); HAL_Delay(30);
            }
        } else {
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
            HAL_Delay(500);
        }
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====
