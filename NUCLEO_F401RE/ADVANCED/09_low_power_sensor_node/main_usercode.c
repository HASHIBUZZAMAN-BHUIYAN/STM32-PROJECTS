// ============================================================
// Nucleo-F401RE ADVANCED/09 — Low Power Sensor Node
// STM32F401RETx  |  HAL + CubeMX 6.16
// BMP280 on I2C1, USART2, RTC Alarm A → Standby
// Wake every 60s, read sensor, TX JSON, sleep
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
#define BMP280_ADDR  (0x76 << 1)
#define BMP280_CTRL  0xF4
#define BMP280_CALIB 0x88
#define BMP280_PMSB  0xF7

#define WAKE_INTERVAL_SEC  60
#define BKP_WAKE_COUNT     RTC_BKP_DR0
#define BKP_MAGIC          RTC_BKP_DR1
#define MAGIC_VAL          0xABCD1234UL
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
static char json_buf[80];
static float bmp_temp = 0;
static int32_t bmp_press = 0;
static uint16_t dT1; static int16_t dT2, dT3;
static uint16_t dP1; static int16_t dP2,dP3,dP4,dP5,dP6,dP7,dP8,dP9;
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static void bmp_read_raw(uint8_t reg, uint8_t *buf, uint8_t len) {
    HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, &reg, 1, 10);
    HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, buf, len, 10);
}
static void bmp_write(uint8_t reg, uint8_t val) {
    uint8_t d[2] = {reg, val};
    HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, d, 2, 10);
}

static void bmp280_init(void) {
    uint8_t c[24]; bmp_read_raw(BMP280_CALIB, c, 24);
    dT1=(uint16_t)(c[1]<<8|c[0]); dT2=(int16_t)(c[3]<<8|c[2]); dT3=(int16_t)(c[5]<<8|c[4]);
    dP1=(uint16_t)(c[7]<<8|c[6]); dP2=(int16_t)(c[9]<<8|c[8]); dP3=(int16_t)(c[11]<<8|c[10]);
    dP4=(int16_t)(c[13]<<8|c[12]); dP5=(int16_t)(c[15]<<8|c[14]); dP6=(int16_t)(c[17]<<8|c[16]);
    dP7=(int16_t)(c[19]<<8|c[18]); dP8=(int16_t)(c[21]<<8|c[20]); dP9=(int16_t)(c[23]<<8|c[22]);
    bmp_write(BMP280_CTRL, 0x25); // forced mode, osrs_t=1x, osrs_p=1x
}

static void bmp280_read_forced(void) {
    // Trigger one forced measurement
    bmp_write(BMP280_CTRL, 0x25);
    HAL_Delay(10); // ~9ms conversion time
    uint8_t raw[6]; bmp_read_raw(BMP280_PMSB, raw, 6);
    int32_t adc_P = (int32_t)((raw[0]<<12)|(raw[1]<<4)|(raw[2]>>4));
    int32_t adc_T = (int32_t)((raw[3]<<12)|(raw[4]<<4)|(raw[5]>>4));
    int32_t v1 = ((((adc_T>>3)-((int32_t)dT1<<1)))*dT2)>>11;
    int32_t v2 = (((((adc_T>>4)-(int32_t)dT1)*((adc_T>>4)-(int32_t)dT1))>>12)*dT3)>>14;
    int32_t tf = v1+v2;
    bmp_temp = (float)((tf*5+128)>>8)/100.0f;
    int64_t p1=(int64_t)tf-128000, p2=p1*p1*(int64_t)dP6;
    p2=p2+((p1*(int64_t)dP5)<<17); p2=p2+(((int64_t)dP4)<<35);
    p1=((p1*p1*(int64_t)dP3)>>8)+((p1*(int64_t)dP2)<<12);
    p1=(((((int64_t)1)<<47)+p1)*(int64_t)dP1)>>33;
    if(p1==0){bmp_press=0;return;}
    int64_t pp=1048576-adc_P; pp=(((pp<<31)-p2)*3125)/p1;
    p1=(((int64_t)dP9)*(pp>>13)*(pp>>13))>>25; p2=(((int64_t)dP8)*pp)>>19;
    pp=((pp+p1+p2)>>8)+(((int64_t)dP7)<<4);
    bmp_press=(int32_t)(pp>>8);
}

static void rtc_set_alarm_seconds(uint8_t seconds_from_now) {
    RTC_TimeTypeDef t = {0};
    RTC_DateTypeDef d = {0};
    HAL_RTC_GetTime(&hrtc, &t, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &d, RTC_FORMAT_BIN); // mandatory shadow unlock

    RTC_AlarmTypeDef a = {0};
    a.AlarmTime.Seconds = (t.Seconds + seconds_from_now) % 60;
    a.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY | RTC_ALARMMASK_HOURS | RTC_ALARMMASK_MINUTES;
    a.Alarm = RTC_ALARM_A;
    HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A);
    HAL_RTC_SetAlarm_IT(&hrtc, &a, RTC_FORMAT_BIN);
}

static void enter_standby(void) {
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU | PWR_FLAG_SB);
    HAL_PWR_EnterSTANDBYMode();
    // Never returns
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    // Check if waking from Standby or cold boot
    uint8_t cold_boot = (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) == RESET);
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);

    uint32_t wake_count = 0;
    if (!cold_boot && HAL_RTCEx_BKUPRead(&hrtc, BKP_MAGIC) == MAGIC_VAL) {
        wake_count = HAL_RTCEx_BKUPRead(&hrtc, BKP_WAKE_COUNT) + 1;
    } else {
        // First boot — initialize backup domain
        HAL_RTCEx_BKUPWrite(&hrtc, BKP_MAGIC, MAGIC_VAL);
    }
    HAL_RTCEx_BKUPWrite(&hrtc, BKP_WAKE_COUNT, wake_count);

    // Read sensor
    bmp280_init();
    bmp280_read_forced();

    // Transmit JSON
    int n = snprintf(json_buf, sizeof(json_buf),
                     "{\"wake\":%lu,\"t\":%.2f,\"p\":%ld}\r\n",
                     wake_count, (double)bmp_temp, bmp_press);
    HAL_UART_Transmit(&huart2, (uint8_t*)json_buf, n, 200);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
    HAL_Delay(10); // let UART finish

    // Set alarm and go to standby
    rtc_set_alarm_seconds(WAKE_INTERVAL_SEC > 59 ? 59 : (uint8_t)WAKE_INTERVAL_SEC);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
    enter_standby();
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1) { } // Never reached — standby entered in init
// ===== USER CODE END WHILE =====
