# Nucleo-F401RE BASIC/26 — Sensor Fusion Lite (BMP280 + MPU6050)

## Overview
Reads two I2C sensors simultaneously on I2C1: BMP280 (temperature/pressure) at 0x76 and MPU6050 (accelerometer/gyroscope) at 0x68. Computes pitch and roll from accelerometer data using atan2. Prints fused telemetry via UART2.

## CubeMX Configuration

| Peripheral | Setting | Pins |
|-----------|---------|------|
| I2C1 | Fast Mode, 400 kHz | PB8 (SCL), PB9 (SDA) |
| USART2 | 115200 8N1 | PA2/PA3 |

Both sensors share the same I2C1 bus (different addresses).

## Parts List
| Component | Qty |
|-----------|-----|
| Nucleo-F401RE | 1 |
| BMP280 breakout | 1 |
| MPU6050 breakout | 1 |
| 4.7 kΩ resistors | 2 |

## Wiring
Both sensors connect to the same I2C bus:
- BMP280: PB8/PB9, VCC=3.3V, SDO→GND (addr 0x76)
- MPU6050: PB8/PB9, VCC=3.3V, AD0→GND (addr 0x68)

## Expected Output (every 500ms)
```
T=24.3C P=1013hPa | Acc=(-0.01g, 0.02g, 0.98g) Pitch=1.2 Roll=-2.1 deg
```
