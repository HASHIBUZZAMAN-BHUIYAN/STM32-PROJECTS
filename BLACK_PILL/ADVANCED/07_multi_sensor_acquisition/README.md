# Black Pill ADVANCED/07 — Multi-Sensor Acquisition

## Overview
Simultaneous acquisition from four sensors at different rates, all non-blocking. Uses ADC DMA (PA1 potentiometer + internal temp), I2C BMP280, I2C MPU6050 (IMU), and bit-bang DHT11 — coordinated by HAL_GetTick() timing without FreeRTOS.

## Sensor Schedule
| Sensor | Period | Bus |
|--------|--------|-----|
| ADC (pot + MCU temp) | 100ms | ADC1 DMA |
| BMP280 (temp + pressure) | 2000ms | I2C1 |
| MPU6050 (accel + gyro) | 50ms | I2C1 |
| DHT11 (temp + humidity) | 2000ms | PA8 bit-bang |

## Black Pill Extras Used
- 100 MHz: MPU6050 at 50 Hz is comfortable; tight on Blue Pill at 72 MHz
- FPU: atan2f for pitch/roll from accelerometer, no soft-float penalty
- 128KB RAM: all sensor buffers fit easily

## Output (JSON, every 500ms on USART1)
```json
{"adc_mv":1650,"mcu_t":31.2,"bmp_t":24.1,"bmp_p":101320,"mpu_ax":-0.05,"mpu_ay":0.98,"mpu_az":0.12,"pitch":1.2,"roll":-3.0,"dht_t":23.0,"dht_h":55}
```

## CubeMX
- ADC1: IN1 (PA1) + IN18 (temp), DMA2 Stream0 circular, 2-word buffer
- I2C1: PB6/PB7, 400 kHz (Fast Mode)
- TIM3: 1 MHz (PSC=99, ARR=0xFFFF) for DHT11 bit-bang timing
- USART1: 115200
