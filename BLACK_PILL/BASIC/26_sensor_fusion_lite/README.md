# Black Pill BASIC/26 — Sensor Fusion Lite (BMP280 + MPU6050)

## Overview
Combines BMP280 (temperature/pressure/altitude) and MPU6050 (accelerometer/gyroscope) on the same I2C1 bus. Computes pitch/roll from accelerometer data using atan2f() — leveraging the Black Pill's hardware FPU for fast floating-point. Streams fused data to USART1.

## Why Black Pill for Sensor Fusion?
The STM32F411 Cortex-M4F has a hardware FPU:
- `sinf`/`cosf`/`atan2f` run in ~14 cycles vs ~300+ on Blue Pill (M3 softfloat)
- Enables 100 Hz sensor fusion update rates comfortably

## MPU6050 on I2C
| MPU6050 Pin | Connect To | Notes |
|------------|-----------|-------|
| VCC | 3.3V | |
| GND | GND | |
| SCL | PB6 | I2C1_SCL |
| SDA | PB7 | I2C1_SDA |
| AD0 | GND | I2C address 0x68 |
| INT | (not used) | Optional for data-ready interrupt |

MPU6050 I2C address: AD0=GND → 0x68, AD0=VCC → 0x69

## Pitch/Roll from Accelerometer
```c
float ax, ay, az; // g units from MPU6050
float pitch = atan2f(ay, sqrtf(ax*ax + az*az)) * 180.0f / M_PI;
float roll  = atan2f(-ax, az) * 180.0f / M_PI;
```
This gives static tilt angle (no gyro drift correction). For dynamic tracking, add a complementary or Kalman filter.

## BMP280 Altitude
```
altitude_m = 44330 * (1 - pow(pressure/101325, 1/5.255))
```

## CubeMX Configuration
| Item | Setting |
|------|---------|
| I2C1 | PB6/PB7, 400 kHz |
| USART1 | PA9/PA10, 115200 8N1 |
| TIM2 | 1 MHz for timing |

## Expected Output
```
P=24.3C 1013hPa 50m  Pitch=-2.1 Roll=0.8
P=24.3C 1013hPa 50m  Pitch=-2.0 Roll=0.9
```
