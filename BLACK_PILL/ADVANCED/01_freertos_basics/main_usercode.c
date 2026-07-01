// ============================================================
// Black Pill ADVANCED/01 — FreeRTOS Basics
// STM32F411CEU6  |  HAL + CubeMX 6.16 + FreeRTOS CMSIS-V2
// 100 MHz, 128KB RAM, USART1 PA9/PA10
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include "cmsis_os2.h"
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
typedef struct { uint32_t count; uint32_t tick; } CountMsg_t;
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
static osMessageQueueId_t countQ;
static osMutexId_t uartMtx;
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static void uart_print(const char *s) {
    osMutexAcquire(uartMtx, osWaitForever);
    HAL_UART_Transmit(&huart1, (const uint8_t*)s, strlen(s), 200);
    osMutexRelease(uartMtx);
}

static void BlinkTask(void *arg) {
    (void)arg;
    for (;;) { HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13); osDelay(500); }
}

static void CounterTask(void *arg) {
    (void)arg;
    uint32_t cnt = 0;
    for (;;) {
        CountMsg_t m = { ++cnt, osKernelGetTickCount() };
        osMessageQueuePut(countQ, &m, 0, 0);
        osDelay(250);
    }
}

static void PrintTask(void *arg) {
    (void)arg;
    char buf[64];
    CountMsg_t m;
    uart_print("[RTOS] F411 task demo started\r\n");
    for (;;) {
        if (osMessageQueueGet(countQ, &m, NULL, osWaitForever) == osOK) {
            int n = snprintf(buf, sizeof(buf), "count=%lu tick=%lu\r\n", m.count, m.tick);
            uart_print(buf);
            (void)n;
        }
    }
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    (void)xTask; (void)pcTaskName; __disable_irq(); while(1);
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    uartMtx = osMutexNew(NULL);
    countQ  = osMessageQueueNew(8, sizeof(CountMsg_t), NULL);
    const osThreadAttr_t ba={.name="Blink",  .stack_size=128*4,.priority=osPriorityNormal};
    const osThreadAttr_t ca={.name="Counter",.stack_size=128*4,.priority=osPriorityAboveNormal};
    const osThreadAttr_t pa={.name="Print",  .stack_size=256*4,.priority=osPriorityNormal};
    osThreadNew(BlinkTask,   NULL, &ba);
    osThreadNew(CounterTask, NULL, &ca);
    osThreadNew(PrintTask,   NULL, &pa);
    osKernelStart();
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1) { }
// ===== USER CODE END WHILE =====
