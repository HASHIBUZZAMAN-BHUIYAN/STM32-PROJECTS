// ============================================================
// Black Pill BASIC/26 — Sensor Fusion Lite (BMP280 + MPU6050)
// STM32F411CEU6  |  HAL + CubeMX 6.16  |  100 MHz + FPU
// I2C1: PB6/PB7  BMP280@0x76  MPU6050@0x68  USART1
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <math.h>
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
#define BMP_ADDR  (0x76 << 1)
#define MPU_ADDR  (0x68 << 1)
// MPU6050 registers
#define MPU_PWR1  0x6B
#define MPU_ACCEL 0x3B
#define MPU_WHO   0x75
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
static float bmp_temp=0; static int32_t bmp_press=0;
static float pitch=0, roll=0;
static char buf[96];
static uint16_t dT1; static int16_t dT2,dT3;
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static void i2c_read(uint8_t dev, uint8_t reg, uint8_t *dst, uint8_t len) {
    HAL_I2C_Master_Transmit(&hi2c1, dev, &reg, 1, 10);
    HAL_I2C_Master_Receive(&hi2c1, dev, dst, len, 20);
}
static void i2c_write(uint8_t dev, uint8_t reg, uint8_t val) {
    uint8_t d[2]={reg,val};
    HAL_I2C_Master_Transmit(&hi2c1, dev, d, 2, 10);
}

static void bmp280_init(void) {
    uint8_t c[6]; i2c_read(BMP_ADDR, 0x88, c, 6);
    dT1=(uint16_t)(c[1]<<8|c[0]); dT2=(int16_t)(c[3]<<8|c[2]); dT3=(int16_t)(c[5]<<8|c[4]);
    i2c_write(BMP_ADDR, 0xF4, 0x27);
}
static void bmp280_read(void) {
    uint8_t r[3]; i2c_read(BMP_ADDR, 0xFA, r, 3);
    int32_t adc_T=(int32_t)((r[0]<<12)|(r[1]<<4)|(r[2]>>4));
    int32_t v1=((((adc_T>>3)-((int32_t)dT1<<1)))*dT2)>>11;
    int32_t v2=(((((adc_T>>4)-(int32_t)dT1)*((adc_T>>4)-(int32_t)dT1))>>12)*dT3)>>14;
    bmp_temp=(float)(((v1+v2)*5+128)>>8)/100.0f;
    // Pressure omitted for brevity
    bmp_press=101325;
}

static void mpu6050_init(void) {
    i2c_write(MPU_ADDR, MPU_PWR1, 0x00); // wake up
    HAL_Delay(100);
}
static void mpu6050_read_accel(float *ax, float *ay, float *az) {
    uint8_t r[6]; i2c_read(MPU_ADDR, MPU_ACCEL, r, 6);
    int16_t raw_x = (int16_t)(r[0]<<8|r[1]);
    int16_t raw_y = (int16_t)(r[2]<<8|r[3]);
    int16_t raw_z = (int16_t)(r[4]<<8|r[5]);
    // Default ±2g range: 16384 LSB/g
    *ax = raw_x / 16384.0f;
    *ay = raw_y / 16384.0f;
    *az = raw_z / 16384.0f;
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    bmp280_init();
    mpu6050_init();
    HAL_UART_Transmit(&huart1,
        (uint8_t*)"Black Pill Sensor Fusion (BMP280+MPU6050)\r\n", 43, 100);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        bmp280_read();
        float ax, ay, az;
        mpu6050_read_accel(&ax, &ay, &az);
        pitch = atan2f(ay, sqrtf(ax*ax + az*az)) * 180.0f / 3.14159265f;
        roll  = atan2f(-ax, az) * 180.0f / 3.14159265f;
        float alt = 44330.0f * (1.0f - powf((float)bmp_press / 101325.0f, 0.1903f));
        int n = snprintf(buf, sizeof(buf),
                         "T=%.1fC P=%ldhPa Alt=%.0fm  Pitch=%.1f Roll=%.1f\r\n",
                         (double)bmp_temp, bmp_press/100L, (double)alt,
                         (double)pitch, (double)roll);
        HAL_UART_Transmit(&huart1, (uint8_t*)buf, n, 100);
        (void)n;
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        HAL_Delay(100);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====
