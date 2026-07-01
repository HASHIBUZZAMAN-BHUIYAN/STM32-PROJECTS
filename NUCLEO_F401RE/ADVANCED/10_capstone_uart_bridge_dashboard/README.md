# Nucleo-F401RE ADVANCED/10 — Capstone: UART Bridge + FreeRTOS Dashboard

## Overview
A capstone project combining FreeRTOS, multiple sensors, USB CDC, and a live UART dashboard. Three FreeRTOS tasks sample BMP280 + DHT11 sensors, log to an SD card via FatFs, and stream a live text dashboard to the ST-Link VCP. A UART command interface controls logging and display.

## Architecture
```
SensorTask (High)
  │── BMP280 I2C read (every 2s)
  │── DHT11 GPIO read (every 2s)
  └── Put SensorData_t to xSensorQueue

LogTask (Normal)
  │── Get from xSensorQueue
  │── Append to SD card (FatFs SPI2)
  └── Signal xLogDoneEvent

DisplayTask (Low)
  │── Get latest data (shared, mutex protected)
  │── Print live dashboard to USART2 every 1s
  └── Print to USB CDC if connected (ADVANCED/05)

CmdTask (Normal)
  │── USART2 RX interrupt → parse commands
  └── start/stop/log/status/clear
```

## CubeMX Configuration
| Peripheral | Use |
|-----------|-----|
| FreeRTOS CMSIS-V2 | Task scheduler, heap4 12KB |
| I2C1 PB8/PB9 | BMP280 sensor |
| SPI2 PB13/14/15 + PB12=CS | SD card (FatFs) |
| GPIO PA8 | DHT11 data |
| TIM3 | 1 MHz for DHT11 timing |
| USART2 PA2/PA3 | ST-Link VCP dashboard |
| TIM11 | HAL timebase (FreeRTOS owns SysTick) |

## Dashboard Format (UART, refreshes every 1s)
```
=== Sensor Dashboard ===
BMP280: T=24.3C  P=1013hPa
DHT11:  T=23C    H=55%
Log:    enabled  #42 entries
SD:     mounted  LOG.CSV
Uptime: 00:02:15
========================
```

## UART Commands
| Command | Action |
|---------|--------|
| `start` | Enable SD card logging |
| `stop` | Disable logging |
| `status` | Print current readings once |
| `clear` | Erase LOG.CSV |
| `period <s>` | Set sample period (1–60s) |

## FreeRTOS Memory
Total heap: 12288 bytes. Task stack usage:
- SensorTask: 512 words (2KB)
- LogTask: 512 words (2KB) — FatFs is stack-hungry
- DisplayTask: 256 words (1KB)
- CmdTask: 256 words (1KB)
- FreeRTOS internal: ~1KB

## What Makes This a Capstone
This project integrates every major skill from the BASIC and ADVANCED series:
- I2C peripheral (BMP280) — BASIC/09
- DHT11 bit-bang timing — BASIC/30
- FatFs SD card — ADVANCED/04
- FreeRTOS tasks + queues + mutexes — ADVANCED/01, 02
- UART command parser — seen throughout
- Power-aware design (IWDG optional) — BASIC/23
