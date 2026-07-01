// ============================================================
// Black Pill ADVANCED/10 — Capstone: USB Sensor Device
// STM32F411CEU6  |  HAL + CubeMX 6.16 + FreeRTOS + USB CDC
// USB-C PA11/PA12 direct, BMP280 I2C1, MPU6050 I2C1,
// W25Q32 SPI1, SSD1306 I2C1, FreeRTOS 4 tasks
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include "cmsis_os2.h"
#include "usbd_cdc_if.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
typedef struct {
    float bmp_t; int32_t bmp_p;
    float ax,ay,az; float pitch,roll;
    uint32_t tick;
} SensorData_t;
#define FLASH_CS_LOW()  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET)
#define FLASH_CS_HIGH() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET)
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
static osMessageQueueId_t dataQ;
static osMutexId_t i2cMtx;
static volatile uint8_t stream_on=0;
static char usb_cmd[64]; static volatile uint8_t cmd_ready=0;
static uint32_t flash_addr=0;
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
/* USB CDC send helper with busy retry */
static void usb_send(const char *s){
    uint32_t t=HAL_GetTick();
    while(CDC_Transmit_FS((uint8_t*)s,strlen(s))==USBD_BUSY&&HAL_GetTick()-t<50) osDelay(1);
}

/* ---- BMP280 (addr 0x76) ------------------------------------- */
static uint16_t bT1;static int16_t bT2,bT3;
static uint16_t bP1;static int16_t bP2,bP3,bP4,bP5,bP6,bP7,bP8,bP9;
static int32_t t_fine;
static void bmp_init(void){
    uint8_t cmd=0x88,c[24];
    HAL_I2C_Master_Transmit(&hi2c1,0x76<<1,&cmd,1,10);
    HAL_I2C_Master_Receive(&hi2c1,0x76<<1,c,24,30);
    bT1=c[1]<<8|c[0];bT2=(int16_t)(c[3]<<8|c[2]);bT3=(int16_t)(c[5]<<8|c[4]);
    bP1=c[7]<<8|c[6];bP2=(int16_t)(c[9]<<8|c[8]);bP3=(int16_t)(c[11]<<8|c[10]);
    bP4=(int16_t)(c[13]<<8|c[12]);bP5=(int16_t)(c[15]<<8|c[14]);bP6=(int16_t)(c[17]<<8|c[16]);
    bP7=(int16_t)(c[19]<<8|c[18]);bP8=(int16_t)(c[21]<<8|c[20]);bP9=(int16_t)(c[23]<<8|c[22]);
    uint8_t cfg[2]={0xF4,0x27};
    HAL_I2C_Master_Transmit(&hi2c1,0x76<<1,cfg,2,10);
}
static void bmp_read(float*t,int32_t*pr){
    uint8_t cmd=0xF7,r[6];
    HAL_I2C_Master_Transmit(&hi2c1,0x76<<1,&cmd,1,10);
    HAL_I2C_Master_Receive(&hi2c1,0x76<<1,r,6,20);
    int32_t aP=(r[0]<<12)|(r[1]<<4)|(r[2]>>4),aT=(r[3]<<12)|(r[4]<<4)|(r[5]>>4);
    int64_t v1=((int64_t)aT>>3)-((int64_t)bT1<<1);
    int64_t v2=(v1*(int64_t)bT2)>>11,v3=((v1>>1)*(v1>>1))>>12;
    v3=(v3*(int64_t)bT3)>>14;t_fine=(int32_t)(v2+v3);
    *t=(t_fine*5+128)/25600.0f;
    int64_t p1=((int64_t)t_fine)-128000;
    int64_t p2=p1*p1*(int64_t)bP6;p2+=((p1*(int64_t)bP5)<<17);p2+=((int64_t)bP4<<35);
    p1=((p1*p1*(int64_t)bP3)>>8)+((p1*(int64_t)bP2)<<12);
    p1=(((int64_t)1<<47)+p1)*(int64_t)bP1>>33;
    if(!p1){*pr=0;return;}
    int64_t p=1048576-aP;p=((p-(p2>>12))*3125);
    p=(p<0x80000000LL)?(p<<1)/p1:(p/p1)*2;
    p1=(((int64_t)bP9)*(p>>13)*(p>>13))>>25;p2=(((int64_t)bP8)*p)>>19;
    p=((p+p1+p2)>>8)+((int64_t)bP7<<4);*pr=(int32_t)(p/256);
}

/* ---- MPU6050 (addr 0x68) ------------------------------------ */
static void mpu_init(void){uint8_t d[2]={0x6B,0};HAL_I2C_Master_Transmit(&hi2c1,0x68<<1,d,2,10);}
static void mpu_read(float*ax,float*ay,float*az,float*pitch,float*roll){
    uint8_t cmd=0x3B,r[14];
    HAL_I2C_Master_Transmit(&hi2c1,0x68<<1,&cmd,1,10);
    HAL_I2C_Master_Receive(&hi2c1,0x68<<1,r,14,30);
    *ax=(int16_t)(r[0]<<8|r[1])/16384.0f;
    *ay=(int16_t)(r[2]<<8|r[3])/16384.0f;
    *az=(int16_t)(r[4]<<8|r[5])/16384.0f;
    *pitch=atan2f(*ay,sqrtf(*ax**ax+*az**az))*180.0f/3.14159f;
    *roll=atan2f(-*ax,*az)*180.0f/3.14159f;
}

/* ---- W25Q32 SPI1 -------------------------------------------- */
static uint8_t w25_spi(uint8_t b){uint8_t r;HAL_SPI_TransmitReceive(&hspi1,&b,&r,1,10);return r;}
static void w25_we(void){FLASH_CS_LOW();w25_spi(0x06);FLASH_CS_HIGH();}
static void w25_wait(void){uint8_t s;do{FLASH_CS_LOW();w25_spi(0x05);s=w25_spi(0);FLASH_CS_HIGH();}while(s&1);}
static void w25_write(uint32_t addr,const uint8_t*d,uint16_t len){
    w25_we();FLASH_CS_LOW();
    w25_spi(0x02);w25_spi(addr>>16);w25_spi(addr>>8);w25_spi(addr);
    for(uint16_t i=0;i<len;i++)w25_spi(d[i]);
    FLASH_CS_HIGH();w25_wait();
}

/* ---- SSD1306 minimal I2C ------------------------------------ */
static void oled_cmd(uint8_t c){uint8_t d[2]={0x00,c};HAL_I2C_Master_Transmit(&hi2c1,0x3C<<1,d,2,5);}
static void oled_init(void){
    const uint8_t s[]={0xAE,0xD5,0x80,0xA8,0x3F,0xD3,0x00,0x40,0x8D,0x14,
                       0x20,0x00,0xA1,0xC8,0xDA,0x12,0x81,0xCF,0xD9,0xF1,
                       0xDB,0x40,0xA4,0xA6,0xAF};
    for(uint8_t i=0;i<sizeof(s);i++) oled_cmd(s[i]);
}
static void oled_write_str(uint8_t row,const char*s){
    oled_cmd(0xB0+row);oled_cmd(0x00);oled_cmd(0x10);
    /* simplified: just send spaces for now — full font in BASIC/11 */
    (void)s;
}

/* ---- FreeRTOS Tasks ----------------------------------------- */
static void SensorTask(void *arg){(void)arg;
    osMutexAcquire(i2cMtx,osWaitForever);bmp_init();mpu_init();oled_init();osMutexRelease(i2cMtx);
    for(;;){
        SensorData_t d;
        osMutexAcquire(i2cMtx,osWaitForever);
        bmp_read(&d.bmp_t,&d.bmp_p);
        mpu_read(&d.ax,&d.ay,&d.az,&d.pitch,&d.roll);
        osMutexRelease(i2cMtx);
        d.tick=osKernelGetTickCount();
        osMessageQueuePut(dataQ,&d,0,0);
        osDelay(50);
    }
}
static SensorData_t last_data={0};
static void LogTask(void *arg){(void)arg;
    uint32_t last_stream=0;
    for(;;){
        SensorData_t d;
        if(osMessageQueueGet(dataQ,&d,NULL,100)==osOK){
            last_data=d;
            /* Write to W25Q32 every 10s */
            static uint32_t llog=0;
            if(d.tick-llog>=10000){
                llog=d.tick;
                uint8_t rec[16]={0};
                int16_t t16=(int16_t)(d.bmp_t*100);
                rec[0]=d.tick>>24;rec[1]=d.tick>>16;rec[2]=d.tick>>8;rec[3]=d.tick;
                rec[4]=t16>>8;rec[5]=t16;
                rec[6]=d.bmp_p>>24;rec[7]=d.bmp_p>>16;rec[8]=d.bmp_p>>8;rec[9]=d.bmp_p;
                w25_write(flash_addr,rec,16);
                flash_addr=(flash_addr+16)%0x400000UL;
            }
        }
        uint32_t now=osKernelGetTickCount();
        if(stream_on&&now-last_stream>=1000){
            last_stream=now;
            char json[200];
            int n=snprintf(json,sizeof(json),
                "{\"tick\":%lu,\"bmp_t\":%.2f,\"bmp_p\":%ld,"
                "\"ax\":%.2f,\"ay\":%.2f,\"az\":%.2f,\"pitch\":%.1f,\"roll\":%.1f}\r\n",
                last_data.tick,last_data.bmp_t,last_data.bmp_p,
                last_data.ax,last_data.ay,last_data.az,last_data.pitch,last_data.roll);
            usb_send(json);(void)n;
        }
    }
}
static void DisplayTask(void *arg){(void)arg;
    for(;;){
        char line[32];
        osMutexAcquire(i2cMtx,osWaitForever);
        snprintf(line,sizeof(line),"T:%.1fC P:%ld",last_data.bmp_t,last_data.bmp_p);
        oled_write_str(0,line);
        snprintf(line,sizeof(line),"P:%.1f R:%.1f",last_data.pitch,last_data.roll);
        oled_write_str(2,line);
        osMutexRelease(i2cMtx);
        osDelay(500);
    }
}
static void CmdTask(void *arg){(void)arg;
    for(;;){
        if(cmd_ready){
            cmd_ready=0; char *s=usb_cmd;
            if(strcmp(s,"stream on")==0){stream_on=1;usb_send("stream on\r\n");}
            else if(strcmp(s,"stream off")==0){stream_on=0;usb_send("stream off\r\n");}
            else if(strcmp(s,"read")==0){
                char json[200];
                snprintf(json,sizeof(json),
                    "{\"tick\":%lu,\"bmp_t\":%.2f,\"bmp_p\":%ld,\"pitch\":%.1f,\"roll\":%.1f}\r\n",
                    last_data.tick,last_data.bmp_t,last_data.bmp_p,last_data.pitch,last_data.roll);
                usb_send(json);
            } else if(strcmp(s,"reboot")==0){NVIC_SystemReset();}
            else if(strcmp(s,"status")==0){
                char buf[128];
                snprintf(buf,sizeof(buf),"heap_free=%lu stream=%d addr=0x%06lX\r\n",
                    (uint32_t)xPortGetFreeHeapSize(),(int)stream_on,flash_addr);
                usb_send(buf);
            } else { usb_send("Cmds: stream on|off read reboot status\r\n"); }
        }
        osDelay(10);
    }
}
void vApplicationStackOverflowHook(TaskHandle_t x,char*n){(void)x;(void)n;__disable_irq();while(1);}
/* Called from CDC_Receive_FS in usbd_cdc_if.c */
void App_CDC_Receive(const uint8_t *d, uint32_t len){
    uint32_t n=len<sizeof(usb_cmd)-1?len:sizeof(usb_cmd)-1;
    memcpy(usb_cmd,d,n);
    while(n>0&&(usb_cmd[n-1]=='\r'||usb_cmd[n-1]=='\n')) n--;
    usb_cmd[n]=0; cmd_ready=1;
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    i2cMtx = osMutexNew(NULL);
    dataQ  = osMessageQueueNew(32,sizeof(SensorData_t),NULL);
    const osThreadAttr_t sa={.name="Sens",.stack_size=512*4,.priority=osPriorityHigh};
    const osThreadAttr_t la={.name="Log", .stack_size=512*4,.priority=osPriorityNormal};
    const osThreadAttr_t da={.name="Disp",.stack_size=256*4,.priority=osPriorityLow};
    const osThreadAttr_t ca={.name="Cmd", .stack_size=256*4,.priority=osPriorityNormal};
    osThreadNew(SensorTask, NULL,&sa);
    osThreadNew(LogTask,    NULL,&la);
    osThreadNew(DisplayTask,NULL,&da);
    osThreadNew(CmdTask,    NULL,&ca);
    osKernelStart();
// ===== USER CODE END 2 =====
// ===== USER CODE BEGIN WHILE =====
    while(1){}
// ===== USER CODE END WHILE =====
