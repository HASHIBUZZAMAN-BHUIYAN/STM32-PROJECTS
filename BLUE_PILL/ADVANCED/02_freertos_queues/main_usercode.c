/**
 * @file    main_usercode.c
 * @brief   FreeRTOS Queues — user code sections for Blue Pill (STM32F103C8T6)
 *
 * Paste each labeled block into the matching USER CODE section of the
 * CubeMX-generated main.c.  Do NOT replace CubeMX-generated init calls.
 *
 * Prerequisites (CubeMX):
 *   - FreeRTOS CMSIS_V2, heap >= 4096 bytes
 *   - ADC1 Ch1 (PA1), USART1 115200 8N1
 *
 * NOTE: This code is not hardware-tested. Verify on your board.
 */

// ===== USER CODE BEGIN Includes =====
#include "cmsis_os2.h"
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====


// ===== USER CODE BEGIN PD =====
#define ADC_QUEUE_DEPTH   8U
#define SENSOR_PERIOD_MS  200U
// ===== USER CODE END PD =====


// ===== USER CODE BEGIN PV =====
/* Queue handle */
osMessageQueueId_t adcQueueHandle;

/* Dropped-sample counter (informational) */
volatile uint32_t g_queueOverflows = 0;

/* Task handles */
osThreadId_t sensorTaskHandle;
osThreadId_t displayTaskHandle;

const osThreadAttr_t sensorTask_attr = {
    .name       = "SensorTask",
    .stack_size = 128 * 4,
    .priority   = osPriorityHigh,
};
const osThreadAttr_t displayTask_attr = {
    .name       = "DisplayTask",
    .stack_size = 256 * 4,
    .priority   = osPriorityNormal,
};
// ===== USER CODE END PV =====


// ===== USER CODE BEGIN PFP =====
void SensorTask_Entry(void *argument);
void DisplayTask_Entry(void *argument);
// ===== USER CODE END PFP =====


// ===== USER CODE BEGIN RTOS_QUEUES =====
/* Create the ADC value queue: 8 slots of uint16_t */
adcQueueHandle = osMessageQueueNew(ADC_QUEUE_DEPTH, sizeof(uint16_t), NULL);
// ===== USER CODE END RTOS_QUEUES =====


// ===== USER CODE BEGIN RTOS_THREADS =====
sensorTaskHandle  = osThreadNew(SensorTask_Entry,  NULL, &sensorTask_attr);
displayTaskHandle = osThreadNew(DisplayTask_Entry, NULL, &displayTask_attr);
// ===== USER CODE END RTOS_THREADS =====


/* -----------------------------------------------------------------------
   Task Implementations
   ----------------------------------------------------------------------- */

// ===== USER CODE BEGIN 4 =====

/**
 * @brief SensorTask — reads ADC1 Ch1 (PA1) every 200 ms and posts to queue.
 *        If the queue is full, the sample is discarded and overflow counted.
 */
void SensorTask_Entry(void *argument)
{
    (void)argument;
    uint16_t        adcRaw;
    osStatus_t      status;

    for (;;)
    {
        /* Blocking single-conversion polling */
        if (HAL_ADC_Start(&hadc1) == HAL_OK)
        {
            if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK)
            {
                adcRaw = (uint16_t)HAL_ADC_GetValue(&hadc1);

                /* Post to queue — timeout 0 means non-blocking */
                status = osMessageQueuePut(adcQueueHandle,
                                           &adcRaw,
                                           0U,   /* message priority */
                                           0U);  /* timeout: no wait */
                if (status != osOK)
                {
                    g_queueOverflows++;
                }
            }
            HAL_ADC_Stop(&hadc1);
        }

        osDelay(SENSOR_PERIOD_MS);
    }
}

/**
 * @brief DisplayTask — blocks on queue, formats ADC value, sends via UART1.
 */
void DisplayTask_Entry(void *argument)
{
    (void)argument;
    uint16_t adcValue;
    char     txBuf[64];

    for (;;)
    {
        /* Block indefinitely until a message arrives */
        if (osMessageQueueGet(adcQueueHandle,
                              &adcValue,
                              NULL,          /* message priority (out) */
                              osWaitForever) == osOK)
        {
            int len = snprintf(txBuf, sizeof(txBuf),
                               "[ADC] PA1 = %4u  (0x%04X)\r\n",
                               adcValue, adcValue);
            if (len > 0)
            {
                HAL_UART_Transmit(&huart1, (uint8_t *)txBuf, (uint16_t)len, 100);
            }
        }
    }
}

// ===== USER CODE END 4 =====
