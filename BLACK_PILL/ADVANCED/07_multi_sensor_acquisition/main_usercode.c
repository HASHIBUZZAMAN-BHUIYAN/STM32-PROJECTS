// ============================================================
// Black Pill ADVANCED/07 — Multi-Sensor Acquisition
// STM32F411CEU6  |  HAL + CubeMX 6.16
// ADC DMA + I2C BMP280 + I2C MPU6050 + DHT11 bit-bang
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
#include <string.h>
#include <math.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PV =====
static uint16_t adc_buf[2]; /* [0]=PA1, [1]=IN18 temp sensor */
static float mcu_temp=0,bmp_t=0,dht_t=0,dht_h=0;
static int32_t bmp_p=0;
static float mpu_ax=0,mpu_ay=0,mpu_az=0;
static float mpu_gx=0,mpu_gy=0,mpu_gz=0;
static float pitch=0,roll=0;
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
/* ---------- ADC -------------------------------------------------- */
static void adc_process(void) {
    uint16_t raw_adc = adc_buf[0];
    uint16_t raw_t   = adc_buf[1];
    /* MCU internal temp: Vsense=(raw*3300/4095)mV, T=(V-760)/2.5+25 */
    float v_sense = raw_t * 3300.0f / 4095.0f;
    mcu_temp = (v_sense - 760.0f) / 2.5f + 25.0f;
    (void)raw_adc; /* potentiometer reading — used in JSON */
}

/* ---------- BMP280 ----------------------------------------------- */
static uint16_t bT1; static int16_t bT2,bT3;
static uint16_t bP1; static int16_t bP2,bP3,bP4,bP5,bP6,bP7,bP8,bP9;
static int32_t t_fine;
static void bmp_init(void) {
    uint8_t cmd=0x88,c[24];
    HAL_I2C_Master_Transmit(&hi2c1,0x76<<1,&cmd,1,10);
    HAL_I2C_Master_Receive(&hi2c1,0x76<<1,c,24,30);
    bT1=c[1]<<8|c[0];bT2=(int16_t)(c[3]<<8|c[2]);bT3=(int16_t)(c[5]<<8|c[4]);
    bP1=c[7]<<8|c[6];bP2=(int16_t)(c[9]<<8|c[8]);bP3=(int16_t)(c[11]<<8|c[10]);
    bP4=(int16_t)(c[13]<<8|c[12]);bP5=(int16_t)(c[15]<<8|c[14]);bP6=(int16_t)(c[17]<<8|c[16]);
    bP7=(int16_t)(c[19]<<8|c[18]);bP8=(int16_t)(c[21]<<8|c[20]);bP9=(int16_t)(c[23]<<8|c[22]);
    uint8_t cfg[]={0xF4,0x27,0xF5,0xA0};
    HAL_I2C_Master_Transmit(&hi2c1,0x76<<1,cfg,2,10);
    HAL_I2C_Master_Transmit(&hi2c1,0x76<<1,cfg+2,2,10);
}
static void bmp_read(void) {
    uint8_t cmd=0xF7,r[6];
    HAL_I2C_Master_Transmit(&hi2c1,0x76<<1,&cmd,1,10);
    HAL_I2C_Master_Receive(&hi2c1,0x76<<1,r,6,20);
    int32_t aP=(r[0]<<12)|(r[1]<<4)|(r[2]>>4),aT=(r[3]<<12)|(r[4]<<4)|(r[5]>>4);
    int64_t v1=((int64_t)aT>>3)-((int64_t)bT1<<1);
    int64_t v2=(v1*(int64_t)bT2)>>11,v3=((v1>>1)*(v1>>1))>>12;
    v3=(v3*(int64_t)bT3)>>14; t_fine=(int32_t)(v2+v3);
    bmp_t=(t_fine*5+128)/25600.0f;
    int64_t p1=((int64_t)t_fine)-128000;
    int64_t p2=p1*p1*(int64_t)bP6;p2+=((p1*(int64_t)bP5)<<17);p2+=((int64_t)bP4<<35);
    p1=((p1*p1*(int64_t)bP3)>>8)+((p1*(int64_t)bP2)<<12);
    p1=(((int64_t)1<<47)+p1)*(int64_t)bP1>>33;
    if(!p1){bmp_p=0;return;}
    int64_t p=1048576-aP;p=((p-(p2>>12))*3125);
    p=(p<0x80000000LL)?(p<<1)/p1:(p/p1)*2;
    p1=(((int64_t)bP9)*(p>>13)*(p>>13))>>25;p2=(((int64_t)bP8)*p)>>19;
    p=((p+p1+p2)>>8)+((int64_t)bP7<<4); bmp_p=(int32_t)(p/256);
}

/* ---------- MPU6050 I2C addr 0x68 ------------------------------ */
static void mpu_write(uint8_t reg,uint8_t val){
    uint8_t d[2]={reg,val};HAL_I2C_Master_Transmit(&hi2c1,0x68<<1,d,2,10);
}
static void mpu_init(void){mpu_write(0x6B,0);mpu_write(0x1C,0);mpu_write(0x1B,0);}
static void mpu_read(void){
    uint8_t cmd=0x3B,r[14];
    HAL_I2C_Master_Transmit(&hi2c1,0x68<<1,&cmd,1,10);
    HAL_I2C_Master_Receive(&hi2c1,0x68<<1,r,14,30);
    int16_t ax=(int16_t)(r[0]<<8|r[1]),ay=(int16_t)(r[2]<<8|r[3]),az=(int16_t)(r[4]<<8|r[5]);
    int16_t gx=(int16_t)(r[8]<<8|r[9]),gy=(int16_t)(r[10]<<8|r[11]),gz=(int16_t)(r[12]<<8|r[13]);
    mpu_ax=ax/16384.0f;mpu_ay=ay/16384.0f;mpu_az=az/16384.0f;
    mpu_gx=gx/131.0f;mpu_gy=gy/131.0f;mpu_gz=gz/131.0f;
    pitch=atan2f(mpu_ay,sqrtf(mpu_ax*mpu_ax+mpu_az*mpu_az))*180.0f/3.14159f;
    roll=atan2f(-mpu_ax,mpu_az)*180.0f/3.14159f;
}

/* ---------- DHT11 bit-bang (TIM3 1 MHz) ------------------------- */
static void delay_us(uint32_t us){
    __HAL_TIM_SET_COUNTER(&htim3,0);
    while(__HAL_TIM_GET_COUNTER(&htim3)<us);
}
static void dht_read(void){
    uint8_t data[5]={0};
    /* Send start pulse */
    GPIO_InitTypeDef g={.Pin=GPIO_PIN_8,.Mode=GPIO_MODE_OUTPUT_PP,.Pull=GPIO_NOPULL,.Speed=GPIO_SPEED_FREQ_VERY_HIGH};
    HAL_GPIO_Init(GPIOA,&g);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET); delay_us(18000);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET);   delay_us(30);
    g.Mode=GPIO_MODE_INPUT; g.Pull=GPIO_PULLUP; HAL_GPIO_Init(GPIOA,&g);
    /* Response */
    uint32_t t0=__HAL_TIM_GET_COUNTER(&htim3);
    while(!HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_8)&&(__HAL_TIM_GET_COUNTER(&htim3)-t0<100));
    while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_8)&&(__HAL_TIM_GET_COUNTER(&htim3)-t0<200));
    /* 40 bits */
    for(int i=0;i<40;i++){
        while(!HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_8));
        delay_us(40);
        if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_8)) data[i/8]|=(1<<(7-i%8));
        while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_8));
    }
    if(data[4]==((data[0]+data[1]+data[2]+data[3])&0xFF)){
        dht_h=data[0]; dht_t=data[2];
    }
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    ADC->CCR |= ADC_CCR_TSVREFE;  /* enable internal temp sensor */
    HAL_ADC_Start_DMA(&hadc1,(uint32_t*)adc_buf,2);
    HAL_TIM_Base_Start(&htim3);
    bmp_init(); mpu_init();
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    uint32_t t_adc=0,t_bmp=0,t_mpu=0,t_dht=0,t_print=0;
    while(1){
        uint32_t now=HAL_GetTick();
        if(now-t_adc>=100){t_adc=now;adc_process();}
        if(now-t_mpu>=50) {t_mpu=now;mpu_read();}
        if(now-t_bmp>=2000){t_bmp=now;bmp_read();}
        if(now-t_dht>=2000){t_dht=now;dht_read();}
        if(now-t_print>=500){
            t_print=now;
            char buf[256];
            int n=snprintf(buf,sizeof(buf),
                "{\"adc_mv\":%u,\"mcu_t\":%.1f,\"bmp_t\":%.1f,\"bmp_p\":%ld,"
                "\"mpu_ax\":%.2f,\"mpu_ay\":%.2f,\"mpu_az\":%.2f,"
                "\"pitch\":%.1f,\"roll\":%.1f,\"dht_t\":%.0f,\"dht_h\":%.0f}\r\n",
                (unsigned)(adc_buf[0]*3300/4095),mcu_temp,bmp_t,bmp_p,
                mpu_ax,mpu_ay,mpu_az,pitch,roll,dht_t,dht_h);
            HAL_UART_Transmit(&huart1,(uint8_t*)buf,n,100);
            HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
        }
    }
// ===== USER CODE END WHILE =====
