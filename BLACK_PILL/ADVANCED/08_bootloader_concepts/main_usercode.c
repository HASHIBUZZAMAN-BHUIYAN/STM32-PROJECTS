// ============================================================
// Black Pill ADVANCED/08 — Bootloader Concepts
// STM32F411CEU6  |  HAL + CubeMX 6.16
// UART IAP: receive firmware over XMODEM, write Sector 4,
// validate CRC32, jump to application
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
#define APP_ADDR        0x08010000UL  /* Sector 4 */
#define APP_MAX_SIZE    (64*1024)     /* 64KB Sector 4 */
#define FLASH_SECTOR_4  FLASH_SECTOR_4
/* XMODEM control bytes */
#define SOH  0x01
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
static char rx_buf[64]; static uint8_t rx_byte; static uint8_t rx_idx=0;
static volatile uint8_t cmd_ready=0;
static uint8_t xm_block[128];
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static void uart_tx(const char *s){HAL_UART_Transmit(&huart1,(const uint8_t*)s,strlen(s),200);}
static void uart_byte(uint8_t b){HAL_UART_Transmit(&huart1,&b,1,50);}
static int uart_rx_timeout(uint8_t *b,uint32_t ms){
    uint32_t t=HAL_GetTick();
    while(HAL_GetTick()-t<ms){
        if(HAL_UART_Receive(&huart1,b,1,1)==HAL_OK) return 1;
    }
    return 0;
}

static uint16_t xmodem_crc(const uint8_t *d, uint16_t len){
    uint16_t crc=0;
    for(uint16_t i=0;i<len;i++){
        crc^=(uint16_t)d[i]<<8;
        for(int b=0;b<8;b++) crc=(crc&0x8000)?(crc<<1)^0x1021:(crc<<1);
    }
    return crc;
}

static void flash_erase_sector4(void){
    FLASH_EraseInitTypeDef e={.TypeErase=FLASH_TYPEERASE_SECTORS,
        .VoltageRange=FLASH_VOLTAGE_RANGE_3,.Sector=FLASH_SECTOR_4,.NbSectors=1};
    uint32_t err=0;
    HAL_FLASH_Unlock();
    HAL_FLASHEx_Erase(&e,&err);
    HAL_FLASH_Lock();
}

static int flash_write(uint32_t addr, const uint8_t *d, uint32_t len){
    HAL_FLASH_Unlock();
    for(uint32_t i=0;i<len;i+=4){
        uint32_t w=*(uint32_t*)(d+i);
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,addr+i,w)!=HAL_OK){
            HAL_FLASH_Lock();return 0;
        }
    }
    HAL_FLASH_Lock(); return 1;
}

static void do_xmodem_receive(void){
    uart_tx("Send firmware via XMODEM-CRC now...\r\n");
    uint8_t b; uint32_t write_addr=APP_ADDR; uint8_t seq=1;
    /* Request CRC mode */
    for(int t=0;t<3;t++){uart_byte('C');HAL_Delay(1000);
        if(uart_rx_timeout(&b,100)&&b==SOH) goto got_soh;}
    uart_tx("Timeout\r\n"); return;
got_soh:
    do {
        if(b==EOT){uart_byte(ACK);uart_tx("\r\nDone\r\n");return;}
        if(b!=SOH){uart_byte(CAN);return;}
        uint8_t blk,nblk;
        if(!uart_rx_timeout(&blk,2000)||!uart_rx_timeout(&nblk,2000)){uart_byte(CAN);return;}
        for(int i=0;i<128;i++) if(!uart_rx_timeout(&xm_block[i],1000)){uart_byte(CAN);return;}
        uint8_t ch,cl;
        if(!uart_rx_timeout(&ch,1000)||!uart_rx_timeout(&cl,1000)){uart_byte(CAN);return;}
        uint16_t recv_crc=(ch<<8)|cl, calc_crc=xmodem_crc(xm_block,128);
        if(recv_crc!=calc_crc||blk!=seq||(uint8_t)(~nblk)!=blk){uart_byte(NAK);}
        else {
            if(write_addr<APP_ADDR+APP_MAX_SIZE) flash_write(write_addr,xm_block,128);
            write_addr+=128; seq++; uart_byte(ACK);
        }
    } while(uart_rx_timeout(&b,5000));
    uart_tx("Transfer ended\r\n");
}

static void jump_to_app(void){
    uint32_t sp=*(volatile uint32_t*)APP_ADDR;
    uint32_t pc=*(volatile uint32_t*)(APP_ADDR+4);
    if(sp<0x20000000||sp>0x20020000){uart_tx("Invalid SP — no app\r\n");return;}
    uart_tx("Jumping...\r\n"); HAL_Delay(100);
    __disable_irq();
    /* Reset all peripherals and vector table */
    HAL_DeInit();
    SCB->VTOR=APP_ADDR;
    __set_MSP(sp);
    ((void(*)(void))pc)();
}

static uint32_t sw_crc32(const uint8_t *d,uint32_t len){
    uint32_t crc=0xFFFFFFFF;
    for(uint32_t i=0;i<len;i++){
        crc^=(uint32_t)d[i];
        for(int b=0;b<8;b++) crc=(crc&1)?(crc>>1)^0xEDB88320UL:(crc>>1);
    }
    return ~crc;
}

static void process_cmd(char *s){
    if(strcmp(s,"boot")==0){
        char buf[96];
        uint32_t rcc=RCC->CSR;
        const char *cause="Unknown";
        if(rcc&RCC_CSR_PINRSTF) cause="PIN";
        else if(rcc&RCC_CSR_PORRSTF) cause="POR";
        else if(rcc&RCC_CSR_IWDGRSTF) cause="IWDG";
        else if(rcc&RCC_CSR_SFTRSTF) cause="SW";
        __HAL_RCC_CLEAR_RESET_FLAGS();
        snprintf(buf,sizeof(buf),"Reset cause: %s | App@0x%08lX\r\n",cause,APP_ADDR);
        uart_tx(buf);
    } else if(strcmp(s,"flash start")==0){
        flash_erase_sector4(); do_xmodem_receive();
    } else if(strcmp(s,"jump")==0){
        jump_to_app();
    } else if(strcmp(s,"verify")==0){
        /* find app size by scanning for 0xFFFFFFFF boundary */
        const uint8_t *app=(const uint8_t*)APP_ADDR;
        uint32_t sz=0;
        while(sz<APP_MAX_SIZE && *(uint32_t*)(app+sz)!=0xFFFFFFFF) sz+=4;
        uint32_t crc=sw_crc32(app,sz);
        char buf[64]; snprintf(buf,sizeof(buf),"App %lubytes CRC32=0x%08lX\r\n",sz,crc);
        uart_tx(buf);
    } else if(strcmp(s,"erase")==0){
        flash_erase_sector4(); uart_tx("Sector 4 erased\r\n");
    } else {
        uart_tx("boot|flash start|jump|verify|erase\r\n");
    }
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    HAL_UART_Receive_IT(&huart1,&rx_byte,1);
    uart_tx("IAP bootloader ready\r\n");
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while(1){
        if(cmd_ready){cmd_ready=0;process_cmd(rx_buf);}
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
