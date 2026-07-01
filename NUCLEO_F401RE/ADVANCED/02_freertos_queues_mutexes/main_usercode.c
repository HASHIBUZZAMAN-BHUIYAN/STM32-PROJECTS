// ============================================================
// Nucleo-F401RE ADVANCED/02 — FreeRTOS Queues & Mutexes
// STM32F401RETx  |  HAL + CubeMX 6.16 + FreeRTOS CMSIS-V2
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include "cmsis_os2.h"
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
typedef struct {
    char    source;   // 'A' or 'B'
    uint32_t seq;
    uint32_t val;
} DataMsg_t;
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
static osMessageQueueId_t dataQueueHandle;
static osMutexId_t        uartMutexHandle;
static osMutexId_t        statMutexHandle;

static volatile uint32_t total_consumed = 0;
static volatile uint32_t total_drops    = 0;
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static void uart_print(const char *s) {
    osMutexAcquire(uartMutexHandle, osWaitForever);
    HAL_UART_Transmit(&huart2, (const uint8_t*)s, strlen(s), 200);
    osMutexRelease(uartMutexHandle);
}

static void ProducerA(void *arg) {
    (void)arg;
    uint32_t seq = 0;
    for (;;) {
        DataMsg_t msg = { .source = 'A', .seq = ++seq, .val = (seq * 1023) & 0x3FF };
        if (osMessageQueuePut(dataQueueHandle, &msg, 0, 0) != osOK) {
            osMutexAcquire(statMutexHandle, osWaitForever);
            total_drops++;
            osMutexRelease(statMutexHandle);
        }
        osDelay(100); // 10 Hz
    }
}

static void ProducerB(void *arg) {
    (void)arg;
    uint32_t seq = 0;
    for (;;) {
        DataMsg_t msg = { .source = 'B', .seq = ++seq, .val = 512 };
        if (osMessageQueuePut(dataQueueHandle, &msg, 0, 10) != osOK) {
            osMutexAcquire(statMutexHandle, osWaitForever);
            total_drops++;
            osMutexRelease(statMutexHandle);
        }
        osDelay(200); // 5 Hz
    }
}

static void ConsumerTask(void *arg) {
    (void)arg;
    char buf[64];
    DataMsg_t msg;
    for (;;) {
        if (osMessageQueueGet(dataQueueHandle, &msg, NULL, osWaitForever) == osOK) {
            osMutexAcquire(statMutexHandle, osWaitForever);
            total_consumed++;
            osMutexRelease(statMutexHandle);
            int n = snprintf(buf, sizeof(buf), "[%c] seq=%lu val=%lu\r\n",
                             msg.source, msg.seq, msg.val);
            uart_print(buf);
            (void)n;
        }
    }
}

static void StatTask(void *arg) {
    (void)arg;
    char buf[80];
    for (;;) {
        osDelay(2000);
        uint32_t q, drops, cons;
        osMutexAcquire(statMutexHandle, osWaitForever);
        drops = total_drops;
        cons  = total_consumed;
        osMutexRelease(statMutexHandle);
        q = osMessageQueueGetCount(dataQueueHandle);
        int n = snprintf(buf, sizeof(buf),
                         "[STAT] q=%lu drops=%lu cons=%lu\r\n", q, drops, cons);
        uart_print(buf);
        (void)n;
    }
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    (void)xTask; (void)pcTaskName;
    __disable_irq();
    while (1);
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    uartMutexHandle = osMutexNew(NULL);
    statMutexHandle = osMutexNew(NULL);
    dataQueueHandle = osMessageQueueNew(10, sizeof(DataMsg_t), NULL);

    const osThreadAttr_t pa = { .name="ProdA",    .stack_size=256*4, .priority=osPriorityAboveNormal };
    const osThreadAttr_t pb = { .name="ProdB",    .stack_size=256*4, .priority=osPriorityNormal };
    const osThreadAttr_t co = { .name="Consumer", .stack_size=512*4, .priority=osPriorityHigh };
    const osThreadAttr_t st = { .name="Stat",     .stack_size=256*4, .priority=osPriorityLow };

    osThreadNew(ProducerA,    NULL, &pa);
    osThreadNew(ProducerB,    NULL, &pb);
    osThreadNew(ConsumerTask, NULL, &co);
    osThreadNew(StatTask,     NULL, &st);

    osKernelStart();
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1) { }
// ===== USER CODE END WHILE =====
