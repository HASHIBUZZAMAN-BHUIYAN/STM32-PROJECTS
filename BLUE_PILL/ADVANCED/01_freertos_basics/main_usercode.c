/**
 * @file    main_usercode.c
 * @brief   FreeRTOS Basics — user code sections for Blue Pill (STM32F103C8T6)
 *
 * Paste each labeled block into the matching USER CODE section of the
 * CubeMX-generated main.c.  Do NOT replace CubeMX-generated init calls.
 *
 * Prerequisites (CubeMX):
 *   - FreeRTOS CMSIS_V2, heap 8192 bytes
 *   - Tasks: LedTask (Low), UartTask (Normal), SensorTask (High)
 *   - ADC1 Ch0 (PA0), USART1 115200 8N1, PC13 GPIO_Output
 *
 * NOTE: This code is not hardware-tested. Verify on your board.
 */

// ===== USER CODE BEGIN Includes =====
#include "cmsis_os2.h"
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====


// ===== USER CODE BEGIN PD (Private Defines) =====
#define LED_PIN        GPIO_PIN_13
#define LED_PORT       GPIOC
#define HEARTBEAT_MS   1000U
#define SENSOR_MS      200U
#define LED_MS         500U
// ===== USER CODE END PD =====


// ===== USER CODE BEGIN PV (Private Variables) =====
/* Shared ADC result — written by SensorTask, read by UartTask.
   On Cortex-M3 a uint32_t read/write is atomic; no mutex needed here. */
volatile uint32_t g_adcValue = 0;

/* Task handles — assigned by osThreadNew() */
osThreadId_t ledTaskHandle;
osThreadId_t uartTaskHandle;
osThreadId_t sensorTaskHandle;

/* Task attribute structs */
const osThreadAttr_t ledTask_attr = {
    .name       = "LedTask",
    .stack_size = 128 * 4,
    .priority   = osPriorityLow,
};
const osThreadAttr_t uartTask_attr = {
    .name       = "UartTask",
    .stack_size = 128 * 4,
    .priority   = osPriorityNormal,
};
const osThreadAttr_t sensorTask_attr = {
    .name       = "SensorTask",
    .stack_size = 128 * 4,
    .priority   = osPriorityHigh,
};
// ===== USER CODE END PV =====


// ===== USER CODE BEGIN PFP (Private Function Prototypes) =====
void LedTask_Entry(void *argument);
void UartTask_Entry(void *argument);
void SensorTask_Entry(void *argument);
// ===== USER CODE END PFP =====


// ===== USER CODE BEGIN 2 (after peripheral init, before osKernelStart) =====
/*
 * Create tasks.  osKernelStart() is called by CubeMX immediately after
 * USER CODE BEGIN RTOS_THREADS — place task creation there instead if
 * CubeMX already generated that section.
 */
ledTaskHandle    = osThreadNew(LedTask_Entry,    NULL, &ledTask_attr);
uartTaskHandle   = osThreadNew(UartTask_Entry,   NULL, &uartTask_attr);
sensorTaskHandle = osThreadNew(SensorTask_Entry, NULL, &sensorTask_attr);
// ===== USER CODE END 2 =====


/* -----------------------------------------------------------------------
   Task Implementations
   ----------------------------------------------------------------------- */

// ===== USER CODE BEGIN 4 (function definitions) =====

/**
 * @brief LedTask — toggles PC13 onboard LED every 500 ms.
 *        PC13 is active-low: RESET = LED ON, SET = LED OFF.
 */
void LedTask_Entry(void *argument)
{
    (void)argument;
    for (;;)
    {
        HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
        osDelay(LED_MS);
    }
}

/**
 * @brief UartTask — sends a heartbeat message over UART1 every 1 s.
 */
void UartTask_Entry(void *argument)
{
    (void)argument;
    uint32_t count = 0;
    char     txBuf[64];

    for (;;)
    {
        count++;
        int len = snprintf(txBuf, sizeof(txBuf),
                           "[UART] Heartbeat #%lu  ADC=%lu\r\n",
                           count, g_adcValue);

        if (len > 0)
        {
            /* huart1 is declared extern in main.h by CubeMX */
            HAL_UART_Transmit(&huart1, (uint8_t *)txBuf, (uint16_t)len, 100);
        }

        osDelay(HEARTBEAT_MS);
    }
}

/**
 * @brief SensorTask — reads ADC1 Ch0 (PA0) every 200 ms.
 *        Stores result in g_adcValue for other tasks to read.
 */
void SensorTask_Entry(void *argument)
{
    (void)argument;
    HAL_StatusTypeDef status;

    for (;;)
    {
        /* Start single conversion */
        status = HAL_ADC_Start(&hadc1);
        if (status == HAL_OK)
        {
            /* Wait up to 10 ms for conversion */
            if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK)
            {
                g_adcValue = HAL_ADC_GetValue(&hadc1);
            }
            HAL_ADC_Stop(&hadc1);
        }

        osDelay(SENSOR_MS);
    }
}

// ===== USER CODE END 4 =====
