# Nucleo-F401RE BASIC/09 — I2C Bus Scan

## Overview
Scans the entire I2C address space (0x01–0x7F) using `HAL_I2C_IsDeviceReady`, prints found device addresses via UART2 (ST-Link VCP). Essential diagnostic tool for any I2C project.

## CubeMX Configuration

### Part / Board: NUCLEO-F401RE

| Peripheral | Setting | Pins |
|-----------|---------|------|
| I2C1 | Standard Mode, 100 kHz | PB8 (SCL / Arduino D15), PB9 (SDA / Arduino D14) |
| USART2 | Async, 115200 8N1 | PA2 (TX), PA3 (RX) — ST-Link VCP |

**I2C pull-ups**: Add 4.7 kΩ resistors from PB8 and PB9 to 3.3 V. Many breakout boards include them.

**Clock**: 84 MHz SYSCLK via PLL.

## Parts List
| Component | Qty | Notes |
|-----------|-----|-------|
| Nucleo-F401RE | 1 | |
| I2C device(s) to scan | 1+ | BMP280, SSD1306, MPU6050, etc. |
| 4.7 kΩ resistors | 2 | I2C pull-ups (if not on module) |
| USB cable | 1 | ST-Link VCP for serial output |

## How to Build & Flash
1. CubeMX: configure I2C1 + USART2 as above, generate STM32CubeIDE project
2. Paste `main_usercode.c` blocks into `main.c`
3. Build, flash via ST-Link
4. Open serial terminal at 115200 baud on ST-Link VCP COM port

## Expected Behavior
```
I2C Bus Scan (0x01-0x7F):
  Found device at 0x3C  (SSD1306 OLED)
  Found device at 0x76  (BMP280)
Scan complete. 2 device(s) found.
```
Repeated every 3 seconds.
