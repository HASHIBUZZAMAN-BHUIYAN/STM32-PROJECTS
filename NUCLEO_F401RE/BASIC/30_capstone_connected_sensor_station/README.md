# Nucleo-F401RE BASIC/30 — Capstone: Connected Sensor Station

## Overview
A complete sensor station that reads BMP280 (temperature/pressure) and DHT11 (humidity) over I2C and GPIO respectively, displays live data on an SSD1306 OLED, and streams JSON-formatted readings over UART every 5 seconds. Serves as a capstone integrating I2C, OLED, UART, ADC, and GPIO from previous projects.

## Features
- BMP280 temperature + pressure via I2C1 (PB8/PB9)
- DHT11 temperature + humidity via GPIO bit-banging (PA8)
- SSD1306 128×64 OLED display via I2C1 (shared bus)
- JSON telemetry every 5 seconds via USART2 (ST-Link VCP)
- LD2 heartbeat blink every second
- UART command interface: `read` forces immediate reading, `info` prints board info

## CubeMX Configuration
| Peripheral | Settings |
|-----------|---------|
| I2C1 | PB8=SCL, PB9=SDA, 400 kHz |
| USART2 | PA2/PA3, 115200 8N1, Async, Global IT enabled |
| GPIO PA8 | Output initially, switches to Input during DHT11 bit-banging |
| TIM3 | 1 MHz tick (PSC=83, ARR=0xFFFF) for DHT11 timing |
| ADC1 IN0 | PA0, single conversion, for internal Vref |

## Expected Behavior
```
{"t1":24.3,"p":101325,"t2":23.0,"h":55,"tick":5000}
{"t1":24.4,"p":101320,"t2":23.0,"h":54,"tick":10000}
```
- t1 = BMP280 temperature °C
- p = pressure Pa
- t2 = DHT11 temperature °C
- h = DHT11 humidity %
- tick = milliseconds since boot

## OLED Display Layout
```
[Line 0] Sensor Station
[Line 1] T:24.3C  P:1013hPa
[Line 2] DHT: 23C  55%
[Line 3] Uptime: 5s
```

## Wiring Diagram
```
Nucleo       BMP280 (I2C)    SSD1306 (I2C)   DHT11
PB8 (SCL) ─── SCL ──────────── SCL
PB9 (SDA) ─── SDA ──────────── SDA
3.3V ──────── VCC ──────────── VCC ──────────── VCC
GND ────────── GND ──────────── GND ──────────── GND
                                                PA8 ── DATA ── 10kΩ ── 3.3V
```
BMP280 SDO → GND (I2C address 0x76). SDO → 3.3V gives 0x77.
SSD1306 default address 0x3C.

## I2C Bus Addresses
| Device | Address |
|--------|---------|
| BMP280 | 0x76 |
| SSD1306 | 0x3C |

## Safety
- All 3.3V logic — do NOT use 5V
- DHT11 data line needs 10kΩ pull-up to 3.3V (NOT 5V)
- BMP280 SPI pads must not be confused with I2C pads
