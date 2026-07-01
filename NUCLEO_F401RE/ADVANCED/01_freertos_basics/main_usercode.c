// ============================================================
// Nucleo-F401RE ADVANCED/01 — FreeRTOS Basics
// STM32F401RETx  |  HAL + CubeMX 6.16 + FreeRTOS CMSIS-V2
// Three tasks communicating via queue
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os2.h"
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
typedef struct {
    uint32_t count;
    uint32_t tick;
} CountMsg_t;
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
static osThreadId_t blinkTaskHandle;
static osThreadId_t counterTaskHandle;
static osThreadId_t printTaskHandle;
static osMessageQueueId_t countQueueHandle;
static osMutexId_t uartMutexHandle;
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static void uart_print(const char *s) {
    osMutexAcquire(uartMutexHandle, osWaitForever);
    HAL_UART_Transmit(&huart2, (const uint8_t*)s, strlen(s), 200);
    osMutexRelease(uartMutexHandle);
}

static void BlinkTask(void *arg) {
    (void)arg;
    for (;;) {
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        osDelay(500);
    }
}

static void CounterTask(void *arg) {
    (void)arg;
    uint32_t count = 0;
    for (;;) {
        count++;
        CountMsg_t msg = { .count = count, .tick = osKernelGetTickCount() };
        osMessageQueuePut(countQueueHandle, &msg, 0, 0);
        osDelay(250);
    }
}

static void PrintTask(void *arg) {
    (void)arg;
    char buf[64];
    CountMsg_t msg;
    uart_print("[RTOS] Task demo started\r\n");
    for (;;) {
        if (osMessageQueueGet(countQueueHandle, &msg, NULL, osWaitForever) == osOK) {
            int n = snprintf(buf, sizeof(buf), "count=%lu tick=%lu\r\n",
                             msg.count, msg.tick);
            uart_print(buf);
            (void)n;
        }
    }
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    (void)xTask; (void)pcTaskName;
    // Trap stack overflow — inspect pcTaskName in debugger
    __disable_irq();
    while (1);
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    // Create RTOS objects before osKernelStart()
    uartMutexHandle   = osMutexNew(NULL);
    countQueueHandle  = osMessageQueueNew(8, sizeof(CountMsg_t), NULL);

    const osThreadAttr_t blink_attr   = { .name="Blink",   .stack_size=128*4, .priority=osPriorityNormal };
    const osThreadAttr_t counter_attr = { .name="Counter", .stack_size=128*4, .priority=osPriorityAboveNormal };
    const osThreadAttr_t print_attr   = { .name="Print",   .stack_size=256*4, .priority=osPriorityNormal };

    blinkTaskHandle   = osThreadNew(BlinkTask,   NULL, &blink_attr);
    counterTaskHandle = osThreadNew(CounterTask, NULL, &counter_attr);
    printTaskHandle   = osThreadNew(PrintTask,   NULL, &print_attr);

    osKernelStart();
    // Never reached
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    // Unreachable after osKernelStart()
    while (1) { }
// ===== USER CODE END WHILE =====
