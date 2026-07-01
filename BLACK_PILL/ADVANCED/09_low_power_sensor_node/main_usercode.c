// ============================================================
// Black Pill ADVANCED/09 — Low-Power Sensor Node
// STM32F411CEU6  |  HAL + CubeMX 6.16
// Standby mode, RTC alarm wakeup, BMP280, W25Q32, USART1
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
#define MAGIC         0xDEADBEEFUL
#define WAKE_INTERVAL 60   /* seconds */
/* W25Q32 sector/page addresses */
#define FLASH_CS_LOW()  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET)
#define FLASH_CS_HIGH() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET)
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
static float bmp_t=0.0f; static int32_t bmp_p=0;
static uint32_t wake_count=0;
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
/* ---------- BMP280 Forced Mode ---------------------------------- */
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
}
static void bmp_forced_read(void){
    /* Trigger single measurement (forced mode) */
    uint8_t cmd[2]={0xF4,0x25}; /* osrs_t x1, osrs_p x1, forced */
    HAL_I2C_Master_Transmit(&hi2c1,0x76<<1,cmd,2,10);
    HAL_Delay(10); /* ~8ms measurement time */
    uint8_t r_cmd=0xF7, r[6];
    HAL_I2C_Master_Transmit(&hi2c1,0x76<<1,&r_cmd,1,10);
    HAL_I2C_Master_Receive(&hi2c1,0x76<<1,r,6,20);
    int32_t aP=(r[0]<<12)|(r[1]<<4)|(r[2]>>4),aT=(r[3]<<12)|(r[4]<<4)|(r[5]>>4);
    int64_t v1=((int64_t)aT>>3)-((int64_t)bT1<<1);
    int64_t v2=(v1*(int64_t)bT2)>>11,v3=((v1>>1)*(v1>>1))>>12;
    v3=(v3*(int64_t)bT3)>>14;t_fine=(int32_t)(v2+v3);
    bmp_t=(t_fine*5+128)/25600.0f;
    int64_t p1=((int64_t)t_fine)-128000;
    int64_t p2=p1*p1*(int64_t)bP6;p2+=((p1*(int64_t)bP5)<<17);p2+=((int64_t)bP4<<35);
    p1=((p1*p1*(int64_t)bP3)>>8)+((p1*(int64_t)bP2)<<12);
    p1=(((int64_t)1<<47)+p1)*(int64_t)bP1>>33;
    if(!p1){bmp_p=0;return;}
    int64_t p=1048576-aP;p=((p-(p2>>12))*3125);
    p=(p<0x80000000LL)?(p<<1)/p1:(p/p1)*2;
    p1=(((int64_t)bP9)*(p>>13)*(p>>13))>>25;p2=(((int64_t)bP8)*p)>>19;
    p=((p+p1+p2)>>8)+((int64_t)bP7<<4);bmp_p=(int32_t)(p/256);
}

/* ---------- W25Q32 SPI1 ---------------------------------------- */
static uint8_t w25_spi(uint8_t b){uint8_t r;HAL_SPI_TransmitReceive(&hspi1,&b,&r,1,10);return r;}
static void w25_cmd(uint8_t cmd){FLASH_CS_LOW();w25_spi(cmd);FLASH_CS_HIGH();}
static uint8_t w25_status(void){uint8_t s;FLASH_CS_LOW();w25_spi(0x05);s=w25_spi(0xFF);FLASH_CS_HIGH();return s;}
static void w25_wait(void){while(w25_status()&0x01)HAL_Delay(1);}
static void w25_write_enable(void){w25_cmd(0x06);}
static void w25_page_write(uint32_t addr,const uint8_t*d,uint16_t len){
    w25_write_enable();
    FLASH_CS_LOW();
    w25_spi(0x02);w25_spi(addr>>16);w25_spi(addr>>8);w25_spi(addr);
    for(uint16_t i=0;i<len;i++)w25_spi(d[i]);
    FLASH_CS_HIGH(); w25_wait();
}
static void w25_log_record(uint32_t wc,float t,int32_t p){
    /* Each record 16 bytes: [count:4][temp_x100:2][press:4][tick:4][pad:2] */
    /* Use address = (wc % 2048) * 16 — 2048 records before wrap */
    uint32_t addr=((wc%2048)*16);
    /* Erase sector (4KB) on sector boundary */
    if(addr%4096==0){
        w25_write_enable();
        FLASH_CS_LOW();w25_spi(0x20);w25_spi(addr>>16);w25_spi(addr>>8);w25_spi(addr);FLASH_CS_HIGH();
        w25_wait();
    }
    uint8_t rec[16]={0};
    rec[0]=wc>>24;rec[1]=wc>>16;rec[2]=wc>>8;rec[3]=wc;
    int16_t t_x100=(int16_t)(t*100);
    rec[4]=t_x100>>8;rec[5]=t_x100;
    rec[6]=p>>24;rec[7]=p>>16;rec[8]=p>>8;rec[9]=p;
    uint32_t tk=HAL_GetTick();
    rec[10]=tk>>24;rec[11]=tk>>16;rec[12]=tk>>8;rec[13]=tk;
    w25_page_write(addr,rec,16);
}

/* ---------- RTC Alarm ------------------------------------------ */
static void set_rtc_alarm_seconds(RTC_HandleTypeDef *hrtc, uint8_t sec_offset){
    RTC_AlarmTypeDef al={0};
    RTC_TimeTypeDef t; RTC_DateTypeDef d;
    HAL_RTC_GetTime(hrtc,&t,RTC_FORMAT_BIN);
    HAL_RTC_GetDate(hrtc,&d,RTC_FORMAT_BIN); /* mandatory on F4 */
    al.Alarm=RTC_ALARM_A;
    al.AlarmTime.Seconds=(t.Seconds+sec_offset)%60;
    al.AlarmMask=RTC_ALARMMASK_DATEWEEKDAY|RTC_ALARMMASK_HOURS|RTC_ALARMMASK_MINUTES;
    HAL_RTC_SetAlarm_IT(hrtc,&al,RTC_FORMAT_BIN);
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    /* Check reset cause */
    uint32_t magic=HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR0);
    uint8_t cold_boot=(magic!=MAGIC);
    if(cold_boot){
        /* First boot: init RTC, write magic */
        HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR0,MAGIC);
        HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR1,0);
    }
    wake_count=HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR1)+(!cold_boot?1:0);
    HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR1,wake_count);

    /* Acquire sensor data */
    bmp_init(); bmp_forced_read();
    w25_log_record(wake_count,bmp_t,bmp_p);

    /* Transmit JSON */
    char json[128];
    int n=snprintf(json,sizeof(json),
        "{\"wake\":%lu,\"temp\":%.2f,\"press\":%ld,\"cold\":%d}\r\n",
        wake_count,bmp_t,bmp_p,(int)cold_boot);
    HAL_UART_Transmit(&huart1,(uint8_t*)json,n,500);
    HAL_Delay(100); /* let UART drain */

    /* Set RTC alarm and enter Standby */
    set_rtc_alarm_seconds(&hrtc,WAKE_INTERVAL);
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWR_EnterSTANDBYMode();
    /* Never reached */
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while(1){ /* Should never reach here */ }
// ===== USER CODE END WHILE =====
