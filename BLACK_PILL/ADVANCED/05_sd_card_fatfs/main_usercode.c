// ============================================================
// Black Pill ADVANCED/05 — SD Card FatFs Logging
// STM32F411CEU6  |  HAL + CubeMX 6.16 + FatFs
// SPI1: PA5=SCK, PA6=MISO, PA7=MOSI, PA4=CS (free on Black Pill)
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include "fatfs.h"
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PV =====
static FATFS fs;
static FIL   fil;
static char  tx_buf[128], rx_buf[64];
static uint8_t rx_byte; static uint8_t rx_idx=0;
static volatile uint8_t cmd_ready=0;
static float last_temp=0.0f; static int32_t last_press=0;
static uint32_t log_count=0;
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
/* --- BMP280 minimal driver (I2C1 PB6/PB7, addr 0x76) ---------- */
static void bmp280_write(uint8_t reg, uint8_t val) {
    uint8_t d[2]={reg&0x7F,val};
    HAL_I2C_Master_Transmit(&hi2c1,0x76<<1,d,2,10);
}
static void bmp280_read_raw(int32_t *adc_T, int32_t *adc_P) {
    uint8_t cmd=0xF7, raw[6];
    HAL_I2C_Master_Transmit(&hi2c1,0x76<<1,&cmd,1,10);
    HAL_I2C_Master_Receive(&hi2c1,0x76<<1,raw,6,20);
    *adc_P=(raw[0]<<12)|(raw[1]<<4)|(raw[2]>>4);
    *adc_T=(raw[3]<<12)|(raw[4]<<4)|(raw[5]>>4);
}
static uint16_t dig_T1; static int16_t dig_T2,dig_T3;
static uint16_t dig_P1; static int16_t dig_P2,dig_P3,dig_P4,dig_P5,dig_P6,dig_P7,dig_P8,dig_P9;
static void bmp280_init(void) {
    uint8_t cmd=0x88, calib[24];
    HAL_I2C_Master_Transmit(&hi2c1,0x76<<1,&cmd,1,10);
    HAL_I2C_Master_Receive(&hi2c1,0x76<<1,calib,24,30);
    dig_T1=calib[1]<<8|calib[0]; dig_T2=(int16_t)(calib[3]<<8|calib[2]);
    dig_T3=(int16_t)(calib[5]<<8|calib[4]);
    dig_P1=calib[7]<<8|calib[6];  dig_P2=(int16_t)(calib[9]<<8|calib[8]);
    dig_P3=(int16_t)(calib[11]<<8|calib[10]); dig_P4=(int16_t)(calib[13]<<8|calib[12]);
    dig_P5=(int16_t)(calib[15]<<8|calib[14]); dig_P6=(int16_t)(calib[17]<<8|calib[16]);
    dig_P7=(int16_t)(calib[19]<<8|calib[18]); dig_P8=(int16_t)(calib[21]<<8|calib[20]);
    dig_P9=(int16_t)(calib[23]<<8|calib[22]);
    bmp280_write(0xF4,0x27); /* normal mode, osrs x1 */
    bmp280_write(0xF5,0xA0);
}
static int32_t t_fine;
static void bmp280_read(float *temp_c, int32_t *press_pa) {
    int32_t aT,aP; bmp280_read_raw(&aT,&aP);
    int64_t v1=((int64_t)aT>>3)-((int64_t)dig_T1<<1);
    int64_t v2=(v1*(int64_t)dig_T2)>>11;
    int64_t v3=((v1>>1)*(v1>>1))>>12; v3=(v3*(int64_t)dig_T3)>>14;
    t_fine=(int32_t)(v2+v3); *temp_c=(t_fine*5+128)/25600.0f;
    int64_t p1=((int64_t)t_fine)-128000;
    int64_t p2=p1*p1*(int64_t)dig_P6; p2+=((p1*(int64_t)dig_P5)<<17);
    p2+=((int64_t)dig_P4<<35);
    p1=((p1*p1*(int64_t)dig_P3)>>8)+((p1*(int64_t)dig_P2)<<12);
    p1=(((int64_t)1<<47)+p1)*(int64_t)dig_P1>>33;
    if(!p1){*press_pa=0;return;}
    int64_t p=1048576-aP; p=((p-(p2>>12))*3125);
    p=(p<0x80000000LL)?(p<<1)/p1:(p/p1)*2;
    p1=(((int64_t)dig_P9)*(p>>13)*(p>>13))>>25;
    p2=(((int64_t)dig_P8)*p)>>19;
    p=((p+p1+p2)>>8)+((int64_t)dig_P7<<4);
    *press_pa=(int32_t)(p/256);
}

/* --- SD helper -------------------------------------------------- */
static uint8_t sd_ok=0;
static FRESULT sd_open_append(void) {
    FRESULT fr=f_open(&fil,"LOG.CSV",FA_WRITE|FA_OPEN_APPEND);
    return fr;
}
static void sd_write_record(void) {
    if(!sd_ok) return;
    if(sd_open_append()!=FR_OK) return;
    char line[64];
    int n=snprintf(line,sizeof(line),"%lu,%.2f,%ld\r\n",
        HAL_GetTick(),last_temp,last_press);
    UINT bw; f_write(&fil,line,n,&bw);
    log_count++;
    if(log_count%10==0) f_sync(&fil);
    f_close(&fil);
}

static void process_cmd(char *s) {
    if(strcmp(s,"dump")==0) {
        if(!sd_ok){HAL_UART_Transmit(&huart1,(uint8_t*)"SD not mounted\r\n",16,100);return;}
        FIL f2; if(f_open(&f2,"LOG.CSV",FA_READ)!=FR_OK){HAL_UART_Transmit(&huart1,(uint8_t*)"open fail\r\n",11,100);return;}
        char line[80]; int lines=0,total=0;
        /* count lines first */
        while(f_gets(line,sizeof(line),&f2)) total++;
        /* rewind and skip to last 20 */
        f_rewind(&f2); int skip=total>20?total-20:0; int i=0;
        while(f_gets(line,sizeof(line),&f2)){
            if(i++<skip) continue;
            HAL_UART_Transmit(&huart1,(uint8_t*)line,strlen(line),50);
        }
        f_close(&f2);
    } else if(strcmp(s,"erase")==0) {
        f_unlink("LOG.CSV");
        HAL_UART_Transmit(&huart1,(uint8_t*)"LOG.CSV erased\r\n",16,100);
        log_count=0;
    } else if(strcmp(s,"info")==0) {
        FATFS *fsp; DWORD fre_clust;
        if(f_getfree("",&fre_clust,&fsp)==FR_OK){
            uint32_t tot=(fsp->n_fatent-2)*fsp->csize/2/1024;
            uint32_t fre=fre_clust*fsp->csize/2/1024;
            int n=snprintf(tx_buf,sizeof(tx_buf),"SD: %luMB total, %luMB free, %lu records\r\n",tot/1024,fre/1024,log_count);
            HAL_UART_Transmit(&huart1,(uint8_t*)tx_buf,n,100);
        }
    } else if(strcmp(s,"status")==0) {
        int n=snprintf(tx_buf,sizeof(tx_buf),"T=%.2fC P=%ldPa logs=%lu sd=%s\r\n",
            last_temp,last_press,log_count,sd_ok?"OK":"FAIL");
        HAL_UART_Transmit(&huart1,(uint8_t*)tx_buf,n,100);
    }
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    bmp280_init();
    if(f_mount(&fs,"",1)==FR_OK) sd_ok=1;
    HAL_UART_Transmit(&huart1,(uint8_t*)"SD logger ready\r\n",17,100);
    HAL_UART_Receive_IT(&huart1,&rx_byte,1);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    uint32_t last_log=0;
    while(1) {
        if(cmd_ready){cmd_ready=0;process_cmd(rx_buf);}
        uint32_t now=HAL_GetTick();
        if(now-last_log>=5000){
            last_log=now;
            bmp280_read(&last_temp,&last_press);
            sd_write_record();
            int n=snprintf(tx_buf,sizeof(tx_buf),"LOG: T=%.2fC P=%ldPa\r\n",last_temp,last_press);
            HAL_UART_Transmit(&huart1,(uint8_t*)tx_buf,n,50);
            HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
        }
    }
// ===== USER CODE END WHILE =====

// ===== USER CODE BEGIN 4 =====
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *h){
    if(h->Instance==USART1){
        if(rx_byte=='\r'||rx_byte=='\n'){
            if(rx_idx>0){rx_buf[rx_idx]=0;rx_idx=0;cmd_ready=1;}
        } else if(rx_idx<sizeof(rx_buf)-1){rx_buf[rx_idx++]=rx_byte;}
        HAL_UART_Receive_IT(&huart1,&rx_byte,1);
    }
}
// ===== USER CODE END 4 =====
