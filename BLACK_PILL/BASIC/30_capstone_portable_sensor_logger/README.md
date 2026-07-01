# Black Pill BASIC/30 — Capstone: Portable Sensor Logger

## Overview
A standalone USB-powered sensor logger that reads BMP280 + DHT11, displays on SSD1306 OLED, streams to USB CDC (native USB on Black Pill), and logs to SPI flash (W25Q32). No PC connection required for logging — data is stored internally and downloaded via USB CDC command.

## Why Black Pill for This Project?
- **Native USB CDC**: data streams via the built-in USB-C connector, no adapter needed
- **100 MHz + FPU**: fast sensor fusion calculations
- **SPI1 free**: PA5/PA6/PA7 for SPI flash (no LED conflict)
- **512KB flash**: more room for firmware even with FatFs + USB stack

## System Architecture
```
BMP280 (I2C1 PB6/PB7) ─┐
DHT11 (GPIO PA8) ────────┼→ main_loop() ─→ SSD1306 OLED (I2C1 shared)
                          │               ─→ SPI flash log (SPI1 PA5-7, PA4=CS)
                          └→ USB CDC ─────→ PC terminal (PA11/PA12)
```

## USB CDC Commands
| Command | Action |
|---------|--------|
| `read` | Print current sensor values |
| `log start` | Begin logging to flash every 10s |
| `log stop` | Stop logging |
| `dump` | Stream all logged data as CSV |
| `erase` | Bulk erase SPI flash |
| `info` | Board info + log count |

## CubeMX Configuration
| Peripheral | Setting |
|-----------|---------|
| USB_OTG_FS | Device Only, CDC class |
| I2C1 | PB6/PB7, 400 kHz (BMP280 + OLED) |
| SPI1 | PA5/PA6/PA7, Full-Duplex, 12.5 MHz |
| GPIO PA4 | SPI flash CS, Output |
| GPIO PA8 | DHT11 data |
| TIM3 | 1 MHz (DHT11 timing) |
| TIM11 | HAL timebase (SysTick taken by... actually USB doesn't use FreeRTOS here) |

## Log Format (SPI Flash)
Binary records at fixed 4KB sector boundaries:
```
[4B] magic = 0x4C4F4700
[4B] timestamp_ms
[4B] bmp_temp_x100 (e.g., 2435 = 24.35°C)
[4B] bmp_press_Pa
[1B] dht_temp
[1B] dht_hum
[2B] padding
= 20 bytes per record
```
W25Q32 = 4MB = 200,000 records maximum

## Expected OLED Display
```
Portable Logger
T:24.3C P:1013hPa
DHT: 23C 55%
Logs: 42
```
