/**
 * @file    main_usercode.c
 * @brief   PID Controller — LED brightness regulation for Blue Pill (STM32F103C8T6)
 *
 * Paste each labeled block into the matching USER CODE section of the
 * CubeMX-generated main.c.  Do NOT replace CubeMX-generated init calls.
 *
 * Prerequisites (CubeMX):
 *   - TIM1 CH1 PWM: PSC=0, ARR=719  (100 kHz PWM)
 *   - TIM4 period interrupt: PSC=7199, ARR=99  (100 Hz)
 *   - ADC1 Ch1 (PA1), software trigger
 *   - USART1 115200 8N1, RX interrupt enabled
 *
 * NOTE: This code is not hardware-tested. Verify on your board.
 */

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
// ===== USER CODE END Includes =====


// ===== USER CODE BEGIN PD =====
#define PWM_PERIOD          719U      /* ARR value for TIM1 — max duty count */
#define PID_DT              0.01f     /* seconds — 100 Hz tick                */
#define WINDUP_LIMIT        4095.0f   /* clamp integral accumulation           */
#define UART_RX_BUF_LEN    32U
// ===== USER CODE END PD =====


// ===== USER CODE BEGIN PV =====
/* PID state */
typedef struct {
    float kp;
    float ki;
    float kd;
    float setpoint;
    float integral;
    float prev_error;
} PID_t;

static PID_t g_pid = {
    .kp         = 1.0f,
    .ki         = 0.1f,
    .kd         = 0.05f,
    .setpoint   = 2048.0f,
    .integral   = 0.0f,
    .prev_error = 0.0f,
};

/* ADC reading — written by TIM4 ISR callback, read by UART task */
volatile uint32_t g_adcValue  = 0;
/* PWM output — written by TIM4 ISR callback */
volatile uint32_t g_pwmOutput = 0;

/* UART RX ring buffer (single-byte DMA or interrupt driven) */
static uint8_t  g_rxByte  = 0;
static char     g_rxLine[UART_RX_BUF_LEN];
static uint8_t  g_rxIdx   = 0;
// ===== USER CODE END PV =====


// ===== USER CODE BEGIN PFP =====
static float PID_Update(PID_t *pid, float measured);
static void  UART_ProcessLine(const char *line);
static void  UART_Print(const char *msg);
// ===== USER CODE END PFP =====


// ===== USER CODE BEGIN 2 =====
/* Start TIM1 PWM on Channel 1 (PA8) */
HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);

/* Start TIM4 in interrupt mode — triggers PID at 100 Hz */
HAL_TIM_Base_Start_IT(&htim4);

/* Start UART RX interrupt for one byte at a time */
HAL_UART_Receive_IT(&huart1, &g_rxByte, 1);

UART_Print("PID Controller ready. Commands: S<sp> P<kp> I<ki> D<kd> R\r\n");
// ===== USER CODE END 2 =====


/* -----------------------------------------------------------------------
   Callbacks and Helper Functions
   ----------------------------------------------------------------------- */

// ===== USER CODE BEGIN 4 =====

/**
 * @brief PID_Update — compute one PID iteration.
 * @param pid      Pointer to PID state struct.
 * @param measured Current process variable (ADC reading).
 * @return         Control output clamped to [0, PWM_PERIOD].
 */
static float PID_Update(PID_t *pid, float measured)
{
    float error      = pid->setpoint - measured;
    float derivative = (error - pid->prev_error) / PID_DT;

    /* Accumulate integral with anti-windup clamp */
    pid->integral += error * PID_DT;
    if      (pid->integral >  WINDUP_LIMIT) pid->integral =  WINDUP_LIMIT;
    else if (pid->integral < -WINDUP_LIMIT) pid->integral = -WINDUP_LIMIT;

    pid->prev_error = error;

    float output = (pid->kp * error)
                 + (pid->ki * pid->integral)
                 + (pid->kd * derivative);

    /* Clamp to valid PWM range */
    if      (output > (float)PWM_PERIOD) output = (float)PWM_PERIOD;
    else if (output < 0.0f)              output = 0.0f;

    return output;
}

/**
 * @brief TIM period elapsed callback — runs PID at 100 Hz.
 *        Triggered by TIM4 overflow interrupt.
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM4)
    {
        /* Read ADC in blocking mode (short — ~1 µs at 72 MHz, 12-cycle conversion) */
        if (HAL_ADC_Start(&hadc1) == HAL_OK)
        {
            if (HAL_ADC_PollForConversion(&hadc1, 2) == HAL_OK)
            {
                g_adcValue = HAL_ADC_GetValue(&hadc1);
            }
            HAL_ADC_Stop(&hadc1);
        }

        /* Run PID */
        float output = PID_Update(&g_pid, (float)g_adcValue);
        g_pwmOutput  = (uint32_t)output;

        /* Apply output to TIM1 CH1 PWM compare register */
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, g_pwmOutput);
    }
}

/**
 * @brief UART RX complete callback — accumulates characters into a line buffer.
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        char c = (char)g_rxByte;

        if (c == '\n' || c == '\r')
        {
            if (g_rxIdx > 0)
            {
                g_rxLine[g_rxIdx] = '\0';
                UART_ProcessLine(g_rxLine);
                g_rxIdx = 0;
            }
        }
        else if (g_rxIdx < (UART_RX_BUF_LEN - 1))
        {
            g_rxLine[g_rxIdx++] = c;
        }

        /* Re-arm for next byte */
        HAL_UART_Receive_IT(&huart1, &g_rxByte, 1);
    }
}

/**
 * @brief Parse and execute a UART command line.
 *        Commands: S<setpoint>, P<kp>, I<ki>, D<kd>, R (report)
 */
static void UART_ProcessLine(const char *line)
{
    char txBuf[128];

    if (line[0] == 'S' || line[0] == 's')
    {
        float sp = (float)atof(&line[1]);
        if (sp >= 0.0f && sp <= 4095.0f)
        {
            g_pid.setpoint = sp;
            snprintf(txBuf, sizeof(txBuf), "Setpoint = %.1f\r\n", g_pid.setpoint);
            UART_Print(txBuf);
        }
    }
    else if (line[0] == 'P' || line[0] == 'p')
    {
        g_pid.kp = (float)atof(&line[1]);
        snprintf(txBuf, sizeof(txBuf), "Kp = %.4f\r\n", g_pid.kp);
        UART_Print(txBuf);
    }
    else if (line[0] == 'I' || line[0] == 'i')
    {
        g_pid.ki = (float)atof(&line[1]);
        g_pid.integral = 0.0f;  /* reset integral on gain change */
        snprintf(txBuf, sizeof(txBuf), "Ki = %.4f (integral reset)\r\n", g_pid.ki);
        UART_Print(txBuf);
    }
    else if (line[0] == 'D' || line[0] == 'd')
    {
        g_pid.kd = (float)atof(&line[1]);
        snprintf(txBuf, sizeof(txBuf), "Kd = %.4f\r\n", g_pid.kd);
        UART_Print(txBuf);
    }
    else if (line[0] == 'R' || line[0] == 'r')
    {
        snprintf(txBuf, sizeof(txBuf),
                 "SP=%.1f PV=%lu OUT=%lu Kp=%.3f Ki=%.3f Kd=%.3f I=%.2f\r\n",
                 g_pid.setpoint, g_adcValue, g_pwmOutput,
                 g_pid.kp, g_pid.ki, g_pid.kd, g_pid.integral);
        UART_Print(txBuf);
    }
}

/**
 * @brief Blocking UART transmit helper.
 */
static void UART_Print(const char *msg)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)msg, (uint16_t)strlen(msg), 200);
}

// ===== USER CODE END 4 =====
