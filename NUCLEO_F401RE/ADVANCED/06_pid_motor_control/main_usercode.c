// ============================================================
// Nucleo-F401RE ADVANCED/06 — PID Motor Speed Control
// STM32F401RETx  |  HAL + CubeMX 6.16
// TIM1_CH1=PWM, TIM4=Encoder, TIM3=100Hz PID, USART2=CLI
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
#define ENCODER_CPR   1200   // counts per revolution (after gearbox)
#define PID_HZ        100    // must match TIM3 rate
#define PWM_MAX       839    // TIM1 ARR value = 100% duty
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
// PID state
static float Kp = 1.5f, Ki = 0.2f, Kd = 0.05f;
static float setpoint_rpm = 0.0f;
static float integral = 0.0f;
static float prev_error = 0.0f;
static float measured_rpm = 0.0f;
static float pid_output = 0.0f;
static const float MAX_INT = 200.0f;
static const float DT = 1.0f / PID_HZ;

// Encoder
static int16_t prev_enc = 0;

// UART CLI
static volatile uint8_t rx_byte;
static char cmd_buf[48];
static uint8_t cmd_idx = 0;
static volatile uint8_t cmd_ready = 0;
static char uart_buf[80];
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static void set_motor(float duty_pct, uint8_t forward) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, forward ? GPIO_PIN_SET : GPIO_PIN_RESET);
    uint32_t ccr = (uint32_t)((duty_pct / 100.0f) * PWM_MAX);
    if (ccr > PWM_MAX) ccr = PWM_MAX;
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, ccr);
}

static float clampf(float v, float lo, float hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

/* TIM3 100Hz interrupt — runs PID */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance != TIM3) return;

    // Read encoder
    int16_t enc = (int16_t)__HAL_TIM_GET_COUNTER(&htim4);
    int16_t delta = enc - prev_enc;
    prev_enc = enc;

    // RPM: delta ticks per DT seconds
    measured_rpm = ((float)delta / ENCODER_CPR) * 60.0f / DT;

    float error = setpoint_rpm - measured_rpm;
    integral += error * DT;
    integral = clampf(integral, -MAX_INT, MAX_INT);
    float derivative = (error - prev_error) / DT;
    prev_error = error;

    pid_output = Kp*error + Ki*integral + Kd*derivative;
    pid_output = clampf(pid_output, 0.0f, 100.0f);

    set_motor(pid_output, 1);
}

static void process_cmd(void) {
    if (strncmp(cmd_buf, "set ", 4) == 0) {
        setpoint_rpm = (float)atoi(cmd_buf + 4);
        integral = 0; prev_error = 0;
        snprintf(uart_buf, sizeof(uart_buf), "Setpoint: %.1f RPM\r\n", (double)setpoint_rpm);
    } else if (strncmp(cmd_buf, "pid ", 4) == 0) {
        float kp, ki, kd;
        if (sscanf(cmd_buf + 4, "%f %f %f", &kp, &ki, &kd) == 3) {
            Kp = kp; Ki = ki; Kd = kd;
            snprintf(uart_buf, sizeof(uart_buf),
                     "PID: Kp=%.2f Ki=%.2f Kd=%.2f\r\n", (double)Kp, (double)Ki, (double)Kd);
        } else {
            snprintf(uart_buf, sizeof(uart_buf), "Usage: pid Kp Ki Kd\r\n");
        }
    } else if (strncmp(cmd_buf, "stop", 4) == 0) {
        setpoint_rpm = 0; integral = 0; pid_output = 0;
        set_motor(0, 1);
        snprintf(uart_buf, sizeof(uart_buf), "Motor stopped\r\n");
    } else if (strncmp(cmd_buf, "status", 6) == 0) {
        snprintf(uart_buf, sizeof(uart_buf),
                 "SP=%.1f RPM=%.1f err=%.1f out=%.1f%%\r\n",
                 (double)setpoint_rpm, (double)measured_rpm,
                 (double)(setpoint_rpm-measured_rpm), (double)pid_output);
    } else {
        snprintf(uart_buf, sizeof(uart_buf), "Cmds: set <rpm>, pid <Kp Ki Kd>, stop, status\r\n");
    }
    HAL_UART_Transmit(&huart2, (uint8_t*)uart_buf, strlen(uart_buf), 200);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        char c = (char)rx_byte;
        if (c == '\r' || c == '\n') {
            cmd_buf[cmd_idx] = '\0';
            if (cmd_idx > 0) cmd_ready = 1;
        } else if (cmd_idx < sizeof(cmd_buf)-1) {
            cmd_buf[cmd_idx++] = c;
        }
        HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
    }
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
    HAL_TIM_Base_Start_IT(&htim3);
    HAL_UART_Receive_IT(&huart2, &rx_byte, 1);

    const char *banner = "PID motor ctrl. Commands: set <rpm>, pid Kp Ki Kd, stop, status\r\n";
    HAL_UART_Transmit(&huart2, (uint8_t*)banner, strlen(banner), 200);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        if (cmd_ready) {
            cmd_ready = 0;
            process_cmd();
            cmd_idx = 0;
            memset(cmd_buf, 0, sizeof(cmd_buf));
        }

        // Print live status every second
        static uint32_t print_tick = 0;
        if (HAL_GetTick() - print_tick >= 1000) {
            print_tick = HAL_GetTick();
            int n = snprintf(uart_buf, sizeof(uart_buf),
                             "RPM=%.1f SP=%.1f out=%.1f%%\r\n",
                             (double)measured_rpm, (double)setpoint_rpm, (double)pid_output);
            HAL_UART_Transmit(&huart2, (uint8_t*)uart_buf, n, 100);
        }
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====
