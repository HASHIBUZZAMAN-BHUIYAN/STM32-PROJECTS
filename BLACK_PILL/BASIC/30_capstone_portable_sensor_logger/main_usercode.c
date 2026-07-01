// ============================================================
// Black Pill BASIC/30 — Capstone: Portable Sensor Logger
// STM32F411CEU6  |  HAL + CubeMX 6.16  |  100 MHz + FPU
// BMP280(I2C1) + DHT11(PA8) + SSD1306(I2C1) + W25Q32(SPI1) + USB CDC
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include "usbd_cdc_if.h"
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
#define BMP_ADDR  (0x76 << 1)
#define OLED_ADDR (0x3C << 1)
#define SPI_CS_PORT GPIOA
#define SPI_CS_PIN  GPIO_PIN_4
#define DHT_PORT    GPIOA
#define DHT_PIN     GPIO_PIN_8

#define LOG_MAGIC   0x4C4F4700UL
#define LOG_SECTOR_SIZE 4096
#define LOG_RECORD_SIZE 20

typedef struct __attribute__((packed)) {
    uint32_t magic;
    uint32_t ts_ms;
    int32_t  temp_x100;
    int32_t  press_pa;
    int8_t   dht_temp;
    uint8_t  dht_hum;
    uint16_t pad;
} LogRecord_t;
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
static float bmp_temp = 0; static int32_t bmp_press = 0;
static int8_t dht_temp = 0; static uint8_t dht_hum = 0;
static uint8_t logging = 0;
static uint32_t log_count = 0;
static uint32_t flash_write_addr = 0;
static char cdc_rx[64]; static uint8_t cdc_rx_len = 0;
static volatile uint8_t cdc_rx_flag = 0;
static uint16_t dT1; static int16_t dT2,dT3;
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
/* ---- I2C helpers ---- */
static void i2c_rd(uint8_t dev,uint8_t reg,uint8_t*dst,uint8_t len){
    HAL_I2C_Master_Transmit(&hi2c1,dev,&reg,1,10);
    HAL_I2C_Master_Receive(&hi2c1,dev,dst,len,20);
}
static void i2c_wr(uint8_t dev,uint8_t reg,uint8_t val){
    uint8_t d[2]={reg,val}; HAL_I2C_Master_Transmit(&hi2c1,dev,d,2,10);
}

/* ---- BMP280 ---- */
static void bmp280_init(void){
    uint8_t c[6]; i2c_rd(BMP_ADDR,0x88,c,6);
    dT1=(uint16_t)(c[1]<<8|c[0]);dT2=(int16_t)(c[3]<<8|c[2]);dT3=(int16_t)(c[5]<<8|c[4]);
    i2c_wr(BMP_ADDR,0xF4,0x27);
}
static void bmp280_read(void){
    uint8_t r[3]; i2c_rd(BMP_ADDR,0xFA,r,3);
    int32_t adc=(int32_t)((r[0]<<12)|(r[1]<<4)|(r[2]>>4));
    int32_t v1=((((adc>>3)-((int32_t)dT1<<1)))*dT2)>>11;
    int32_t v2=(((((adc>>4)-(int32_t)dT1)*((adc>>4)-(int32_t)dT1))>>12)*dT3)>>14;
    bmp_temp=(float)(((v1+v2)*5+128)>>8)/100.0f;
    bmp_press=101325; // simplified
}

/* ---- SSD1306 ---- */
static void oled_cmd(uint8_t c){uint8_t d[2]={0x00,c};HAL_I2C_Master_Transmit(&hi2c1,OLED_ADDR,d,2,10);}
static void oled_init(void){
    HAL_Delay(100);
    const uint8_t cmds[]={0xAE,0x20,0x00,0xB0,0xC8,0x00,0x10,0x40,
        0x81,0xCF,0xA1,0xA6,0xA8,0x3F,0xA4,0xD3,0x00,0xD5,0xF0,
        0xD9,0x22,0xDA,0x12,0xDB,0x20,0x8D,0x14,0xAF};
    for(uint8_t i=0;i<sizeof(cmds);i++) oled_cmd(cmds[i]);
}
static void oled_set_pos(uint8_t page,uint8_t col){
    oled_cmd(0xB0|page);oled_cmd(0x00|(col&0x0F));oled_cmd(0x10|(col>>4));
}
static void oled_show(uint8_t page,const char*s){
    oled_set_pos(page,0);
    uint8_t line[129];line[0]=0x40;
    uint8_t len=strlen(s);if(len>21)len=21;
    for(uint8_t i=0;i<len;i++) line[1+i*6]=0x7F;
    memset(line+1+len*6,0,128-len*6);
    HAL_I2C_Master_Transmit(&hi2c1,OLED_ADDR,line,129,50);
}

/* ---- DHT11 ---- */
static void delay_us(uint16_t us){__HAL_TIM_SET_COUNTER(&htim3,0);while(__HAL_TIM_GET_COUNTER(&htim3)<us);}
static void dht_out(void){GPIO_InitTypeDef g={0};g.Pin=DHT_PIN;g.Mode=GPIO_MODE_OUTPUT_PP;g.Speed=GPIO_SPEED_FREQ_LOW;HAL_GPIO_Init(DHT_PORT,&g);}
static void dht_in(void){GPIO_InitTypeDef g={0};g.Pin=DHT_PIN;g.Mode=GPIO_MODE_INPUT;g.Pull=GPIO_NOPULL;HAL_GPIO_Init(DHT_PORT,&g);}
static uint8_t dht11_read_sensor(void){
    uint8_t b[5]={0};
    dht_out(); HAL_GPIO_WritePin(DHT_PORT,DHT_PIN,GPIO_PIN_RESET); HAL_Delay(18);
    HAL_GPIO_WritePin(DHT_PORT,DHT_PIN,GPIO_PIN_SET); delay_us(40); dht_in();
    uint32_t t=HAL_GetTick();
    while(HAL_GPIO_ReadPin(DHT_PORT,DHT_PIN)==GPIO_PIN_SET) if(HAL_GetTick()-t>2) return 0;
    while(HAL_GPIO_ReadPin(DHT_PORT,DHT_PIN)==GPIO_PIN_RESET) if(HAL_GetTick()-t>2) return 0;
    while(HAL_GPIO_ReadPin(DHT_PORT,DHT_PIN)==GPIO_PIN_SET) if(HAL_GetTick()-t>2) return 0;
    for(uint8_t i=0;i<40;i++){
        while(HAL_GPIO_ReadPin(DHT_PORT,DHT_PIN)==GPIO_PIN_RESET);
        delay_us(35); b[i/8]<<=1;
        if(HAL_GPIO_ReadPin(DHT_PORT,DHT_PIN)) b[i/8]|=1;
        while(HAL_GPIO_ReadPin(DHT_PORT,DHT_PIN)==GPIO_PIN_SET);
    }
    if(b[4]==(uint8_t)(b[0]+b[1]+b[2]+b[3])){dht_hum=b[0];dht_temp=(int8_t)b[2];return 1;}
    return 0;
}

/* ---- SPI Flash (W25Q32) ---- */
static void flash_cs(uint8_t low){HAL_GPIO_WritePin(SPI_CS_PORT,SPI_CS_PIN,low?GPIO_PIN_RESET:GPIO_PIN_SET);}
static void flash_cmd1(uint8_t cmd){flash_cs(1);HAL_SPI_Transmit(&hspi1,&cmd,1,10);flash_cs(0);}
static void flash_wait(void){uint8_t c[2]={0x05,0xFF},r[2];do{flash_cs(1);HAL_SPI_TransmitReceive(&hspi1,c,r,2,10);flash_cs(0);}while(r[1]&1);}
static void flash_write_record(const LogRecord_t*rec){
    // Erase sector if at start of sector
    if((flash_write_addr%LOG_SECTOR_SIZE)==0){
        flash_cmd1(0x06); // write enable
        uint8_t e[4]={0x20,(uint8_t)(flash_write_addr>>16),(uint8_t)(flash_write_addr>>8),(uint8_t)flash_write_addr};
        flash_cs(1);HAL_SPI_Transmit(&hspi1,e,4,10);flash_cs(0);
        flash_wait();
    }
    flash_cmd1(0x06); // write enable
    uint8_t hdr[4]={0x02,(uint8_t)(flash_write_addr>>16),(uint8_t)(flash_write_addr>>8),(uint8_t)flash_write_addr};
    flash_cs(1);
    HAL_SPI_Transmit(&hspi1,hdr,4,10);
    HAL_SPI_Transmit(&hspi1,(uint8_t*)rec,sizeof(LogRecord_t),50);
    flash_cs(0);
    flash_wait();
    flash_write_addr+=LOG_RECORD_SIZE;
    log_count++;
}

/* ---- USB CDC ---- */
static void cdc_send(const char*s){
    CDC_Transmit_FS((uint8_t*)s,strlen(s));
}

static void process_cdc_cmd(void){
    char reply[128];
    if(strncmp(cdc_rx,"read",4)==0){
        snprintf(reply,sizeof(reply),"{\"t\":%.1f,\"p\":%ld,\"dht_t\":%d,\"dht_h\":%u}\r\n",
            (double)bmp_temp,bmp_press,(int)dht_temp,dht_hum);
        cdc_send(reply);
    } else if(strcmp(cdc_rx,"log start")==0){logging=1;cdc_send("Logging started\r\n");}
    else if(strcmp(cdc_rx,"log stop")==0){logging=0;cdc_send("Logging stopped\r\n");}
    else if(strcmp(cdc_rx,"info")==0){
        snprintf(reply,sizeof(reply),"Black Pill F411 100MHz | Logs: %lu\r\n",log_count);
        cdc_send(reply);
    } else if(strcmp(cdc_rx,"erase")==0){
        cdc_send("Erasing flash (bulk)...\r\n");
        flash_cmd1(0x06);flash_cmd1(0x60);flash_wait();
        flash_write_addr=0;log_count=0;
        cdc_send("Erased\r\n");
    } else {
        cdc_send("Cmds: read, log start, log stop, info, erase\r\n");
    }
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    flash_cs(0); // CS idle high
    HAL_TIM_Base_Start(&htim3);
    bmp280_init();
    oled_init();
    HAL_Delay(500); // USB enumeration
    cdc_send("Black Pill Portable Sensor Logger\r\nType 'info' for help\r\n");
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    uint32_t t_sensor=0, t_oled=0, t_log=0;
    while (1)
    {
        uint32_t now=HAL_GetTick();

        if(now-t_sensor>=2000){t_sensor=now;bmp280_read();dht11_read_sensor();}
        if(now-t_oled>=500){
            t_oled=now;
            char line[24];
            oled_show(0,"Portable Logger");
            snprintf(line,sizeof(line),"T:%.1fC P:%ldhPa",(double)bmp_temp,bmp_press/100L);
            oled_show(2,line);
            snprintf(line,sizeof(line),"DHT:%dC %u%%",dht_temp,dht_hum);
            oled_show(4,line);
            snprintf(line,sizeof(line),"Logs:%lu",log_count);
            oled_show(6,line);
        }
        if(logging && (now-t_log>=10000)){
            t_log=now;
            LogRecord_t r={.magic=LOG_MAGIC,.ts_ms=now,
                .temp_x100=(int32_t)(bmp_temp*100),.press_pa=bmp_press,
                .dht_temp=dht_temp,.dht_hum=dht_hum,.pad=0};
            flash_write_record(&r);
        }
        if(cdc_rx_flag){cdc_rx_flag=0;process_cdc_cmd();}

        static uint32_t t_led=0;
        if(now-t_led>=1000){t_led=now;HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);}
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====

// ===== USER CODE BEGIN 4 =====
// Add to usbd_cdc_if.c CDC_Receive_FS:
// extern volatile uint8_t cdc_rx_flag; extern char cdc_rx[];
// memcpy(cdc_rx,Buf,*Len); cdc_rx[*Len]='\0'; cdc_rx_flag=1;
// ===== USER CODE END 4 =====
