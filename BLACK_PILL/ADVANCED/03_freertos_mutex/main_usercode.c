// ============================================================
// Black Pill ADVANCED/03 — FreeRTOS Mutex & Priority Inheritance
// STM32F411CEU6  |  HAL + CubeMX 6.16 + FreeRTOS CMSIS-V2
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include "cmsis_os2.h"
#include "task.h"
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PV =====
static osMutexId_t sharedMtx, uartMtx;
static volatile uint32_t med_count = 0;
static volatile uint32_t high_runs = 0, low_runs = 0;
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static void uart_print(const char *s) {
    osMutexAcquire(uartMtx, osWaitForever);
    HAL_UART_Transmit(&huart1, (const uint8_t*)s, strlen(s), 200);
    osMutexRelease(uartMtx);
}

static void HighTask(void *arg) {
    (void)arg; char buf[96];
    for (;;) {
        osDelay(1000);
        uint32_t t0 = osKernelGetTickCount();
        osMutexAcquire(sharedMtx, osWaitForever);
        uint32_t waited = osKernelGetTickCount() - t0;
        /* critical section */
        volatile uint32_t x = 0;
        for (uint32_t i=0;i<10000;i++) x+=i;
        (void)x;
        osMutexRelease(sharedMtx);
        high_runs++;
        snprintf(buf,sizeof(buf),"[HIGH] run=%lu waited=%lums\r\n",high_runs,waited);
        uart_print(buf);
    }
}

static void MedTask(void *arg) {
    (void)arg;
    for (;;) {
        med_count++;
        osDelay(500);
    }
}

static void LowTask(void *arg) {
    (void)arg; char buf[80];
    for (;;) {
        osMutexAcquire(sharedMtx, osWaitForever);
        /* simulate slow I/O — hold mutex 300ms so High blocks */
        osDelay(300);
        osMutexRelease(sharedMtx);
        low_runs++;
        snprintf(buf,sizeof(buf),"[LOW]  run=%lu\r\n",low_runs);
        uart_print(buf);
        osDelay(2000);
    }
}

static void LogTask(void *arg) {
    (void)arg; char buf[128];
    for (;;) {
        osDelay(2000);
        /* Read effective priorities to observe inheritance */
        TaskHandle_t hHigh = xTaskGetHandle("High");
        TaskHandle_t hLow  = xTaskGetHandle("Low");
        UBaseType_t pH = hHigh ? uxTaskPriorityGet(hHigh) : 0;
        UBaseType_t pL = hLow  ? uxTaskPriorityGet(hLow)  : 0;
        snprintf(buf,sizeof(buf),
            "[LOG] med_cnt=%lu high_P=%u low_P=%u\r\n",
            med_count,(unsigned)pH,(unsigned)pL);
        uart_print(buf);
    }
}

void vApplicationStackOverflowHook(TaskHandle_t x,char*n){(void)x;(void)n;__disable_irq();while(1);}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    sharedMtx = osMutexNew(NULL);
    uartMtx   = osMutexNew(NULL);
    const osThreadAttr_t ha={.name="High",.stack_size=256*4,.priority=osPriorityAboveNormal2};
    const osThreadAttr_t ma={.name="Med", .stack_size=128*4,.priority=osPriorityAboveNormal};
    const osThreadAttr_t la={.name="Low", .stack_size=256*4,.priority=osPriorityNormal};
    const osThreadAttr_t ga={.name="Log", .stack_size=256*4,.priority=osPriorityLow};
    osThreadNew(HighTask,NULL,&ha); osThreadNew(MedTask,NULL,&ma);
    osThreadNew(LowTask,NULL,&la); osThreadNew(LogTask,NULL,&ga);
    osKernelStart();
// ===== USER CODE END 2 =====
// ===== USER CODE BEGIN WHILE =====
    while(1){}
// ===== USER CODE END WHILE =====
