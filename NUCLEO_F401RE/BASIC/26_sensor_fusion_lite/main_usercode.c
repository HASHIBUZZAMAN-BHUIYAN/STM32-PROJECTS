// ============================================================
// Nucleo-F401RE BASIC/26 — Sensor Fusion Lite
// BMP280 (0x76) + MPU6050 (0x68) on I2C1
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <math.h>
#include <stdio.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
#define BMP280_ADDR  (0x76 << 1)
#define MPU6050_ADDR (0x68 << 1)
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
static char buf[128];
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static void i2c_write_reg(uint16_t dev, uint8_t reg, uint8_t val) {
    uint8_t b[2] = {reg, val};
    HAL_I2C_Master_Transmit(&hi2c1, dev, b, 2, 10);
}
static void i2c_read_regs(uint16_t dev, uint8_t reg, uint8_t *data, uint16_t len) {
    HAL_I2C_Master_Transmit(&hi2c1, dev, &reg, 1, 10);
    HAL_I2C_Master_Receive(&hi2c1, dev, data, len, 20);
}

static void bmp280_init(void) {
    i2c_write_reg(BMP280_ADDR, 0xF4, 0x57); // forced mode osrs_t/p x2
    i2c_write_reg(BMP280_ADDR, 0xF5, 0x00);
}
static int32_t bmp280_temp_cdeg(void) {
    i2c_write_reg(BMP280_ADDR, 0xF4, 0x57); HAL_Delay(10);
    uint8_t d[3]; i2c_read_regs(BMP280_ADDR, 0xFA, d, 3);
    int32_t raw = ((int32_t)d[0]<<12)|((int32_t)d[1]<<4)|(d[2]>>4);
    return (raw * 100) / 5243;
}

static void mpu6050_init(void) {
    i2c_write_reg(MPU6050_ADDR, 0x6B, 0x00); // wake up
    i2c_write_reg(MPU6050_ADDR, 0x1C, 0x00); // accel ±2g
}
static void mpu6050_read_accel(float *ax, float *ay, float *az) {
    uint8_t d[6]; i2c_read_regs(MPU6050_ADDR, 0x3B, d, 6);
    int16_t raw_x = (int16_t)((d[0]<<8)|d[1]);
    int16_t raw_y = (int16_t)((d[2]<<8)|d[3]);
    int16_t raw_z = (int16_t)((d[4]<<8)|d[5]);
    *ax = raw_x / 16384.0f; // ±2g → LSB = 16384
    *ay = raw_y / 16384.0f;
    *az = raw_z / 16384.0f;
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    bmp280_init();
    mpu6050_init();
    HAL_UART_Transmit(&huart2, (uint8_t*)"Sensor fusion ready\r\n", 21, 100);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        int32_t temp = bmp280_temp_cdeg();
        float ax, ay, az;
        mpu6050_read_accel(&ax, &ay, &az);
        float pitch = atan2f(ax, sqrtf(ay*ay + az*az)) * (180.0f / 3.14159265f);
        float roll  = atan2f(ay, sqrtf(ax*ax + az*az)) * (180.0f / 3.14159265f);

        int n = snprintf(buf, sizeof(buf),
            "T=%ld.%02ldC | Ax=%.2f Ay=%.2f Az=%.2f | P=%.1f R=%.1f deg\r\n",
            temp/100, (temp<0?-temp:temp)%100, ax, ay, az, pitch, roll);
        HAL_UART_Transmit(&huart2, (uint8_t*)buf, n, 200);
        HAL_Delay(500);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====
