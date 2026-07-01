// ============================================================
// Nucleo-F401RE ADVANCED/10 — Capstone: UART Bridge Dashboard
// STM32F401RETx  |  HAL + CubeMX 6.16 + FreeRTOS CMSIS-V2 + FatFs
// BMP280(I2C1) + DHT11(PA8) + SD(SPI2) + USART2 dashboard
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include "cmsis_os2.h"
#include "fatfs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
#define BMP280_ADDR   (0x76 << 1)
#define DHT_PORT      GPIOA
#define DHT_PIN       GPIO_PIN_8
#define SD_CS_PORT    GPIOB
#define SD_CS_PIN     GPIO_PIN_12

typedef struct {
    float    bmp_temp;
    int32_t  bmp_press;
    int8_t   dht_temp;
    uint8_t  dht_hum;
    uint32_t timestamp;
} SensorData_t;
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
static osMessageQueueId_t xSensorQueue;
static osMutexId_t        xUartMutex;
static osMutexId_t        xDataMutex;
static osMutexId_t        xI2CMutex;

static SensorData_t latest_data = {0};
static volatile uint8_t logging_enabled = 0;
static volatile uint32_t log_count = 0;
static volatile uint32_t sample_period_ms = 2000;

// UART CLI
static volatile uint8_t uart_rx_byte;
static char cmd_buf[32];
static uint8_t cmd_idx = 0;
static volatile uint8_t cmd_ready = 0;

// BMP280 trim
static uint16_t dT1; static int16_t dT2, dT3;
static uint16_t dP1; static int16_t dP2,dP3,dP4,dP5,dP6,dP7,dP8,dP9;
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static void uart_print(const char *s) {
    osMutexAcquire(xUartMutex, osWaitForever);
    HAL_UART_Transmit(&huart2, (const uint8_t*)s, strlen(s), 200);
    osMutexRelease(xUartMutex);
}

/* ---- BMP280 ---- */
static void bmp_write(uint8_t reg, uint8_t val) {
    uint8_t d[2] = {reg, val};
    osMutexAcquire(xI2CMutex, osWaitForever);
    HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, d, 2, 10);
    osMutexRelease(xI2CMutex);
}
static void bmp_read(uint8_t reg, uint8_t *dst, uint8_t len) {
    osMutexAcquire(xI2CMutex, osWaitForever);
    HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, &reg, 1, 10);
    HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, dst, len, 10);
    osMutexRelease(xI2CMutex);
}
static void bmp280_init(void) {
    uint8_t c[24]; bmp_read(0x88, c, 24);
    dT1=(uint16_t)(c[1]<<8|c[0]); dT2=(int16_t)(c[3]<<8|c[2]); dT3=(int16_t)(c[5]<<8|c[4]);
    dP1=(uint16_t)(c[7]<<8|c[6]); dP2=(int16_t)(c[9]<<8|c[8]);
    dP3=(int16_t)(c[11]<<8|c[10]); dP4=(int16_t)(c[13]<<8|c[12]);
    dP5=(int16_t)(c[15]<<8|c[14]); dP6=(int16_t)(c[17]<<8|c[16]);
    dP7=(int16_t)(c[19]<<8|c[18]); dP8=(int16_t)(c[21]<<8|c[20]);
    dP9=(int16_t)(c[23]<<8|c[22]);
    bmp_write(0xF4, 0x57);
}
static void bmp280_read_data(float *temp, int32_t *press) {
    uint8_t raw[6]; bmp_read(0xF7, raw, 6);
    int32_t adc_P=(int32_t)((raw[0]<<12)|(raw[1]<<4)|(raw[2]>>4));
    int32_t adc_T=(int32_t)((raw[3]<<12)|(raw[4]<<4)|(raw[5]>>4));
    int32_t v1=((((adc_T>>3)-((int32_t)dT1<<1)))*dT2)>>11;
    int32_t v2=(((((adc_T>>4)-(int32_t)dT1)*((adc_T>>4)-(int32_t)dT1))>>12)*dT3)>>14;
    int32_t tf=v1+v2;
    *temp=(float)((tf*5+128)>>8)/100.0f;
    int64_t p1=(int64_t)tf-128000,p2=p1*p1*(int64_t)dP6;
    p2=p2+((p1*(int64_t)dP5)<<17); p2=p2+(((int64_t)dP4)<<35);
    p1=((p1*p1*(int64_t)dP3)>>8)+((p1*(int64_t)dP2)<<12);
    p1=(((((int64_t)1)<<47)+p1)*(int64_t)dP1)>>33;
    if(!p1){*press=0;return;}
    int64_t pp=1048576-adc_P; pp=(((pp<<31)-p2)*3125)/p1;
    p1=(((int64_t)dP9)*(pp>>13)*(pp>>13))>>25; p2=(((int64_t)dP8)*pp)>>19;
    pp=((pp+p1+p2)>>8)+(((int64_t)dP7)<<4);
    *press=(int32_t)(pp>>8);
}

/* ---- DHT11 ---- */
static void delay_us(uint16_t us) {
    __HAL_TIM_SET_COUNTER(&htim3, 0);
    while (__HAL_TIM_GET_COUNTER(&htim3) < us);
}
static void dht_output(void) {
    GPIO_InitTypeDef g={0}; g.Pin=DHT_PIN; g.Mode=GPIO_MODE_OUTPUT_PP; g.Speed=GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(DHT_PORT, &g);
}
static void dht_input(void) {
    GPIO_InitTypeDef g={0}; g.Pin=DHT_PIN; g.Mode=GPIO_MODE_INPUT; g.Pull=GPIO_NOPULL;
    HAL_GPIO_Init(DHT_PORT, &g);
}
static uint8_t dht11_read(int8_t *t, uint8_t *h) {
    uint8_t bytes[5]={0};
    dht_output();
    HAL_GPIO_WritePin(DHT_PORT, DHT_PIN, GPIO_PIN_RESET);
    osDelay(18);
    HAL_GPIO_WritePin(DHT_PORT, DHT_PIN, GPIO_PIN_SET);
    delay_us(40);
    dht_input();
    uint32_t tick = HAL_GetTick();
    while (HAL_GPIO_ReadPin(DHT_PORT,DHT_PIN)==GPIO_PIN_SET) if(HAL_GetTick()-tick>2) return 0;
    while (HAL_GPIO_ReadPin(DHT_PORT,DHT_PIN)==GPIO_PIN_RESET) if(HAL_GetTick()-tick>2) return 0;
    while (HAL_GPIO_ReadPin(DHT_PORT,DHT_PIN)==GPIO_PIN_SET) if(HAL_GetTick()-tick>2) return 0;
    for (uint8_t i=0;i<40;i++) {
        while(HAL_GPIO_ReadPin(DHT_PORT,DHT_PIN)==GPIO_PIN_RESET);
        delay_us(35);
        bytes[i/8]<<=1;
        if(HAL_GPIO_ReadPin(DHT_PORT,DHT_PIN)) bytes[i/8]|=1;
        while(HAL_GPIO_ReadPin(DHT_PORT,DHT_PIN)==GPIO_PIN_SET);
    }
    if(bytes[4]==(uint8_t)(bytes[0]+bytes[1]+bytes[2]+bytes[3])){
        *h=bytes[0]; *t=(int8_t)bytes[2]; return 1;
    }
    return 0;
}

/* ---- FatFs log ---- */
static void log_to_sd(const SensorData_t *d) {
    FATFS fs; FIL file;
    if(f_mount(&fs,"",1)!=FR_OK) return;
    if(f_open(&file,"LOG.CSV",FA_WRITE|FA_OPEN_APPEND)==FR_OK) {
        char line[80];
        int n=snprintf(line,sizeof(line),"%lu,%.2f,%ld,%d,%u\r\n",
            d->timestamp,(double)d->bmp_temp,d->bmp_press,d->dht_temp,d->dht_hum);
        UINT bw; f_write(&file,line,(UINT)n,&bw);
        f_close(&file);
        osMutexAcquire(xDataMutex,osWaitForever);
        log_count++;
        osMutexRelease(xDataMutex);
    }
    f_mount(NULL,"",0);
}

/* ---- FreeRTOS Tasks ---- */
static void SensorTask(void *arg) {
    (void)arg;
    osDelay(200);
    for (;;) {
        SensorData_t d = {0};
        d.timestamp = osKernelGetTickCount();
        bmp280_read_data(&d.bmp_temp, &d.bmp_press);
        dht11_read(&d.dht_temp, &d.dht_hum);
        osMutexAcquire(xDataMutex, osWaitForever);
        latest_data = d;
        osMutexRelease(xDataMutex);
        osMessageQueuePut(xSensorQueue, &d, 0, 0);
        osDelay(sample_period_ms);
    }
}

static void LogTask(void *arg) {
    (void)arg;
    SensorData_t d;
    for (;;) {
        if (osMessageQueueGet(xSensorQueue, &d, NULL, osWaitForever) == osOK) {
            if (logging_enabled) log_to_sd(&d);
        }
    }
}

static void DisplayTask(void *arg) {
    (void)arg;
    char dash[256];
    for (;;) {
        osDelay(1000);
        SensorData_t d;
        uint32_t lc;
        osMutexAcquire(xDataMutex, osWaitForever);
        d  = latest_data;
        lc = log_count;
        osMutexRelease(xDataMutex);
        uint32_t up = osKernelGetTickCount()/1000;
        int n=snprintf(dash,sizeof(dash),
            "\r\n=== Sensor Dashboard ===\r\n"
            "BMP280: T=%.1fC  P=%ldhPa\r\n"
            "DHT11:  T=%dC    H=%u%%\r\n"
            "Log:    %s  #%lu entries\r\n"
            "Uptime: %02lu:%02lu:%02lu\r\n"
            "========================\r\n",
            (double)d.bmp_temp, d.bmp_press/100L,
            d.dht_temp, d.dht_hum,
            logging_enabled?"enabled ":"disabled",
            lc,
            up/3600, (up%3600)/60, up%60);
        uart_print(dash);
        (void)n;
    }
}

static void CmdTask(void *arg) {
    (void)arg;
    char reply[80];
    for (;;) {
        osDelay(10);
        if (!cmd_ready) continue;
        cmd_ready = 0;
        if (strcmp(cmd_buf,"start")==0) {
            logging_enabled=1;
            uart_print("Logging started\r\n");
        } else if (strcmp(cmd_buf,"stop")==0) {
            logging_enabled=0;
            uart_print("Logging stopped\r\n");
        } else if (strcmp(cmd_buf,"status")==0) {
            SensorData_t d;
            osMutexAcquire(xDataMutex,osWaitForever); d=latest_data; osMutexRelease(xDataMutex);
            snprintf(reply,sizeof(reply),"T=%.1fC P=%ldhPa dht=%dC %u%% log=%s\r\n",
                (double)d.bmp_temp,d.bmp_press/100L,d.dht_temp,d.dht_hum,
                logging_enabled?"on":"off");
            uart_print(reply);
        } else if (strncmp(cmd_buf,"period ",7)==0) {
            uint32_t p=(uint32_t)atoi(cmd_buf+7)*1000;
            if(p>=1000&&p<=60000){sample_period_ms=p;uart_print("Period updated\r\n");}
            else uart_print("Range: 1-60s\r\n");
        } else if (strcmp(cmd_buf,"clear")==0) {
            FATFS fs; FIL f;
            if(f_mount(&fs,"",1)==FR_OK){f_open(&f,"LOG.CSV",FA_CREATE_ALWAYS|FA_WRITE);f_close(&f);f_mount(NULL,"",0);}
            osMutexAcquire(xDataMutex,osWaitForever); log_count=0; osMutexRelease(xDataMutex);
            uart_print("Log cleared\r\n");
        } else {
            uart_print("Cmds: start, stop, status, period <s>, clear\r\n");
        }
        cmd_idx=0; memset(cmd_buf,0,sizeof(cmd_buf));
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        char c=(char)uart_rx_byte;
        if (c=='\r'||c=='\n') { cmd_buf[cmd_idx]='\0'; if(cmd_idx>0) cmd_ready=1; }
        else if (cmd_idx<sizeof(cmd_buf)-1) cmd_buf[cmd_idx++]=c;
        HAL_UART_Receive_IT(&huart2,&uart_rx_byte,1);
    }
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    (void)xTask;(void)pcTaskName;
    __disable_irq(); while(1);
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    HAL_GPIO_WritePin(SD_CS_PORT, SD_CS_PIN, GPIO_PIN_SET);
    HAL_TIM_Base_Start(&htim3);
    bmp280_init();
    HAL_UART_Receive_IT(&huart2, &uart_rx_byte, 1);

    xUartMutex   = osMutexNew(NULL);
    xDataMutex   = osMutexNew(NULL);
    xI2CMutex    = osMutexNew(NULL);
    xSensorQueue = osMessageQueueNew(8, sizeof(SensorData_t), NULL);

    const osThreadAttr_t s_attr  = {.name="Sensor",  .stack_size=512*4, .priority=osPriorityHigh};
    const osThreadAttr_t l_attr  = {.name="Log",     .stack_size=512*4, .priority=osPriorityNormal};
    const osThreadAttr_t d_attr  = {.name="Display", .stack_size=256*4, .priority=osPriorityLow};
    const osThreadAttr_t c_attr  = {.name="Cmd",     .stack_size=256*4, .priority=osPriorityNormal};

    osThreadNew(SensorTask,  NULL, &s_attr);
    osThreadNew(LogTask,     NULL, &l_attr);
    osThreadNew(DisplayTask, NULL, &d_attr);
    osThreadNew(CmdTask,     NULL, &c_attr);

    uart_print("Capstone Dashboard ready. Cmds: start, stop, status, period <s>, clear\r\n");
    osKernelStart();
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1) { }
// ===== USER CODE END WHILE =====
