// ============================================================
// Nucleo-F401RE BASIC/25 — Servo + DC Motor Combined PWM
// STM32F401RETx  |  HAL + CubeMX 6.16
// TIM1 CH1 → Servo (50 Hz) | TIM3 CH1 → Motor speed
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PV =====
static uint8_t rx_byte;
static char cmd_buf[16]; static uint8_t cmd_idx = 0;
static volatile uint8_t cmd_ready = 0;
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
/* Servo: TIM1 CH1, ARR=19999 (50 Hz at PSC=83, 84 MHz)
   1 ms pulse = CCR=1000, 1.5 ms = 1500, 2 ms = 2000 */
static void servo_set_angle(uint8_t degrees) {
    if (degrees > 180) degrees = 180;
    uint32_t ccr = 1000 + (degrees * 1000) / 180;
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, ccr);
}

/* Motor: TIM3 CH1 speed (0-100%), PB0/PB1 direction */
static void motor_set(int8_t speed) { // -100 to +100; 0 = stop
    if (speed > 0) {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (speed * 999) / 100);
    } else if (speed < 0) {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, ((-speed) * 999) / 100);
    } else {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
    }
}

static void process_cmd(const char *cmd) {
    char resp[32];
    if (cmd[0] == 'S') {
        uint8_t angle = (uint8_t)atoi(&cmd[1]);
        servo_set_angle(angle);
        snprintf(resp, sizeof(resp), "Servo->%d deg\r\n", angle);
    } else if (cmd[0] == 'M') {
        if (cmd[1] == 'F') { motor_set(50); snprintf(resp, sizeof(resp), "Motor Fwd 50%%\r\n"); }
        else if (cmd[1] == 'R') { motor_set(-50); snprintf(resp, sizeof(resp), "Motor Rev 50%%\r\n"); }
        else if (cmd[1] == 'S') { motor_set(0); snprintf(resp, sizeof(resp), "Motor Stop\r\n"); }
        else { int s = atoi(&cmd[1]); motor_set(s); snprintf(resp, sizeof(resp), "Motor %d%%\r\n", s); }
    } else { snprintf(resp, sizeof(resp), "?\r\n"); }
    HAL_UART_Transmit(&huart2, (uint8_t*)resp, strlen(resp), 100);
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); // servo
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1); // motor speed
    servo_set_angle(90); // center
    motor_set(0);
    HAL_UART_Transmit(&huart2,
        (uint8_t*)"Commands: S<0-180> M<-100 to 100> MF MR MS\r\n", 44, 100);
    HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        if (cmd_ready) {
            cmd_ready = 0;
            process_cmd(cmd_buf);
        }
        HAL_Delay(10);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====

// ===== USER CODE BEGIN 4 =====
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        if (rx_byte == '\n' || rx_byte == '\r') {
            if (cmd_idx > 0) { cmd_buf[cmd_idx] = '\0'; cmd_idx = 0; cmd_ready = 1; }
        } else if (cmd_idx < 15) cmd_buf[cmd_idx++] = rx_byte;
        HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
    }
}
// ===== USER CODE END 4 =====
