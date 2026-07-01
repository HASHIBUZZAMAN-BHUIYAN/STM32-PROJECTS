# Black Pill BASIC/11 — SSD1306 OLED Display

## Overview
Drives a 128×64 SSD1306 OLED via I2C1 (PB6/PB7). Implements a minimal driver with command initialization, screen clear, and text rendering using a 5×8 font. Displays board info and a live tick counter.

## MCU Facts
| Item | Value |
|------|-------|
| MCU | STM32F411CEU6 |
| I2C | I2C1, PB6=SCL, PB7=SDA, 400 kHz |
| OLED | SSD1306, 128×64, I2C address 0x3C |

## CubeMX Configuration
| Item | Setting |
|------|---------|
| I2C1 | PB6/PB7, Fast Mode 400 kHz |
| USART1 | PA9/PA10, 115200 8N1 (optional debug) |

## SSD1306 Init Sequence
The SSD1306 needs a specific init command sequence after power-up (see main_usercode.c). Key commands:
- `0xAE` — display off (during init)
- `0x20, 0x00` — horizontal addressing mode
- `0x8D, 0x14` — enable charge pump
- `0xAF` — display on

## Display Coordinate System
- Pages: 0–7 (each page = 8 vertical pixels)
- Columns: 0–127
- Set cursor: `0xB0|page`, `0x00|(col&0x0F)`, `0x10|(col>>4)`
- Each character = 5 pixels wide + 1 gap = 6 columns

## Expected Display
```
Black Pill F411
SSD1306 OLED
Tick: 1234ms
```
