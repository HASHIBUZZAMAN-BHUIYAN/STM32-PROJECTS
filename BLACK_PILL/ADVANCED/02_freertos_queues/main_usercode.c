// ============================================================
// Black Pill ADVANCED/02 — FreeRTOS Queues (Multi-Producer)
// STM32F411CEU6  |  HAL + CubeMX 6.16 + FreeRTOS CMSIS-V2
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include "cmsis_os2.h"
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
typedef struct { char src; uint32_t seq; uint32_t val; } DataMsg_t;
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
static osMessageQueueId_t dataQ;
static osMutexId_t uartMtx, statMtx;
static volatile uint32_t g_drops = 0, g_consumed = 0;
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static void uart_print(const char *s) {
    osMutexAcquire(uartMtx, osWaitForever);
    HAL_UART_Transmit(&huart1, (const uint8_t*)s, strlen(s), 200);
    osMutexRelease(uartMtx);
}
static void SensorA(void *arg) {
    (void)arg; uint32_t seq=0; char buf[64];
    for(;;) {
        DataMsg_t m={'A',++seq,seq*100};
        if(osMessageQueuePut(dataQ,&m,0,0)!=osOK){osMutexAcquire(statMtx,osWaitForever);g_drops++;osMutexRelease(statMtx);}
        osDelay(100);
    }
}
static void SensorB(void *arg) {
    (void)arg; uint32_t seq=0;
    for(;;) {
        DataMsg_t m={'B',++seq,seq*250};
        if(osMessageQueuePut(dataQ,&m,0,10)!=osOK){osMutexAcquire(statMtx,osWaitForever);g_drops++;osMutexRelease(statMtx);}
        osDelay(250);
    }
}
static void Consumer(void *arg) {
    (void)arg; char buf[64]; DataMsg_t m;
    for(;;) {
        if(osMessageQueueGet(dataQ,&m,NULL,osWaitForever)==osOK){
            osMutexAcquire(statMtx,osWaitForever);g_consumed++;osMutexRelease(statMtx);
            int n=snprintf(buf,sizeof(buf),"[%c] seq=%lu val=%lu\r\n",m.src,m.seq,m.val);
            uart_print(buf);(void)n;
        }
    }
}
static void StatTask(void *arg) {
    (void)arg; char buf[80];
    for(;;) {
        osDelay(2000);
        uint32_t d,c,q;
        osMutexAcquire(statMtx,osWaitForever);d=g_drops;c=g_consumed;osMutexRelease(statMtx);
        q=osMessageQueueGetCount(dataQ);
        int n=snprintf(buf,sizeof(buf),"[STAT] q=%lu drops=%lu cons=%lu\r\n",q,d,c);
        uart_print(buf);(void)n;
    }
}
void vApplicationStackOverflowHook(TaskHandle_t x,char*n){(void)x;(void)n;__disable_irq();while(1);}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    uartMtx = osMutexNew(NULL); statMtx = osMutexNew(NULL);
    dataQ   = osMessageQueueNew(16, sizeof(DataMsg_t), NULL);
    const osThreadAttr_t aa={.name="SA",.stack_size=256*4,.priority=osPriorityAboveNormal};
    const osThreadAttr_t ba={.name="SB",.stack_size=256*4,.priority=osPriorityNormal};
    const osThreadAttr_t ca={.name="Co",.stack_size=512*4,.priority=osPriorityHigh};
    const osThreadAttr_t da={.name="St",.stack_size=256*4,.priority=osPriorityLow};
    osThreadNew(SensorA,NULL,&aa);osThreadNew(SensorB,NULL,&ba);
    osThreadNew(Consumer,NULL,&ca);osThreadNew(StatTask,NULL,&da);
    osKernelStart();
// ===== USER CODE END 2 =====
// ===== USER CODE BEGIN WHILE =====
    while(1){}
// ===== USER CODE END WHILE =====
