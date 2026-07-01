// ============================================================
// Black Pill ADVANCED/06 — UART Framed Binary Protocol
// STM32F411CEU6  |  HAL + CubeMX 6.16
// USART1 DMA RX circular, BMP280 I2C1 PB6/PB7
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
#include <string.h>
#include <stdint.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
#define SOF0 0x55u
#define SOF1 0xAAu
#define DMA_RX_SIZE 64
#define CMD_PING     0x01u
#define CMD_READ_TEMP 0x02u
#define CMD_READ_ALL 0x03u
#define CMD_SET_LED  0x04u
#define CMD_NACK     0xFFu
typedef struct __attribute__((packed)) {
    float temp_c; int32_t press_pa; uint32_t tick;
} SensorAll_t;
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
static uint8_t dma_rx[DMA_RX_SIZE];
static uint16_t rx_head=0;
static uint8_t tx_frame[32];
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
/* CRC16/CCITT (poly=0x1021, init=0xFFFF) */
static uint16_t crc16(const uint8_t *d, uint16_t len) {
    uint16_t crc=0xFFFF;
    for(uint16_t i=0;i<len;i++){
        crc^=(uint16_t)d[i]<<8;
        for(uint8_t b=0;b<8;b++) crc=(crc&0x8000)?(crc<<1)^0x1021:(crc<<1);
    }
    return crc;
}
static void send_frame(uint8_t type, const uint8_t *payload, uint16_t plen) {
    uint16_t total_len = 1 + plen; /* type + payload */
    tx_frame[0]=SOF0; tx_frame[1]=SOF1;
    tx_frame[2]=(uint8_t)(total_len>>8); tx_frame[3]=(uint8_t)(total_len);
    tx_frame[4]=type;
    if(plen && payload) memcpy(tx_frame+5,payload,plen);
    uint16_t crc=crc16(tx_frame+2, 2+total_len);
    tx_frame[5+plen]=(uint8_t)(crc>>8); tx_frame[6+plen]=(uint8_t)crc;
    HAL_UART_Transmit(&huart1,tx_frame,7+plen,100);
}
static void send_nack(uint8_t code) { uint8_t p=code; send_frame(CMD_NACK,&p,1); }

/* BMP280 minimal (same as ADVANCED/05) */
static float last_temp=0; static int32_t last_press=0;
extern void bmp280_init(void); extern void bmp280_read(float*,int32_t*);
/* (Copy bmp280 functions from ADVANCED/05 main_usercode.c) */

static void dispatch(const uint8_t *payload, uint16_t plen, uint8_t type) {
    (void)plen;
    switch(type) {
    case CMD_PING: {
        uint32_t t=HAL_GetTick();
        send_frame(CMD_PING,(uint8_t*)&t,4);
        break;
    }
    case CMD_READ_TEMP: {
        bmp280_read(&last_temp,&last_press);
        send_frame(CMD_READ_TEMP,(uint8_t*)&last_temp,4);
        break;
    }
    case CMD_READ_ALL: {
        bmp280_read(&last_temp,&last_press);
        SensorAll_t s={last_temp,last_press,HAL_GetTick()};
        send_frame(CMD_READ_ALL,(uint8_t*)&s,sizeof(s));
        break;
    }
    case CMD_SET_LED: {
        uint8_t on = payload ? payload[0] : 0;
        HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,on?GPIO_PIN_RESET:GPIO_PIN_SET);
        send_frame(CMD_SET_LED,NULL,0);
        break;
    }
    default: send_nack(0x01); break;
    }
}

/* Ring buffer parser */
static uint8_t parse_buf[128]; static uint16_t parse_len=0;
static void process_byte(uint8_t b) {
    parse_buf[parse_len++]=(uint8_t)b;
    if(parse_len>sizeof(parse_buf)) parse_len=0;
    /* Resync on SOF */
    while(parse_len>=2 && !(parse_buf[0]==SOF0&&parse_buf[1]==SOF1)){
        memmove(parse_buf,parse_buf+1,--parse_len);
    }
    if(parse_len<6) return; /* need at least SOF+LEN+TYPE+CRC */
    uint16_t tlen=((uint16_t)parse_buf[2]<<8)|parse_buf[3];
    if(tlen>252){parse_len=0;return;}
    uint16_t frame_end=4+tlen+2; /* SOF(2)+LEN(2)+payload+CRC(2) */
    if(parse_len<frame_end) return;
    uint16_t crc_recv=((uint16_t)parse_buf[frame_end-2]<<8)|parse_buf[frame_end-1];
    uint16_t crc_calc=crc16(parse_buf+2,2+tlen);
    if(crc_recv!=crc_calc){parse_len=0;send_nack(0x02);return;}
    dispatch(tlen>1?parse_buf+5:NULL,tlen-1,parse_buf[4]);
    memmove(parse_buf,parse_buf+frame_end,parse_len-frame_end);
    parse_len-=frame_end;
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    bmp280_init();
    HAL_UART_Receive_DMA(&huart1,dma_rx,DMA_RX_SIZE);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while(1) {
        uint16_t ndtr=(uint16_t)__HAL_DMA_GET_COUNTER(huart1.hdmarx);
        uint16_t tail=(uint16_t)(DMA_RX_SIZE-ndtr);
        while(rx_head!=tail){
            process_byte(dma_rx[rx_head]);
            rx_head=(rx_head+1)%DMA_RX_SIZE;
        }
        HAL_Delay(1);
    }
// ===== USER CODE END WHILE =====
