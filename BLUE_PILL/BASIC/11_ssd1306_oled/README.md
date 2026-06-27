# 11 — SSD1306 OLED via I2C

## Overview
Drives a 128×64 SSD1306 OLED display over I2C1 at 400 kHz (Fast Mode).
A minimal driver is implemented entirely in USER CODE — no external library required.
On startup the display is cleared to black, a simple 5×7 font is used to render
"Hello STM32" on the first text line.

## Hardware
- Board : STM32F103C8T6 (Blue Pill)
- Display : SSD1306 128×64 OLED, I2C address 0x3C (SA0 = GND)
- Interface: I2C1 — PB6 (SCL), PB7 (SDA)

## CubeMX Settings
| Peripheral | Setting |
|------------|---------|
| I2C1 | Fast Mode, 400 kHz |
| I2C1 SCL | PB6, open-drain, pull-up |
| I2C1 SDA | PB7, open-drain, pull-up |
| SYS | SysTick as timebase |

## Files
| File | Purpose |
|------|---------|
| `main_usercode.c` | User code sections — driver, init, main loop |
| `wiring_notes.md` | Pin connections and hardware caveats |

## What the Code Does
1. Sends the SSD1306 initialisation command sequence over I2C.
2. Writes 0x00 to all 1024 data bytes (128 × 8 pages) — black screen.
3. Renders the string "Hello STM32" using a built-in 5×7 ASCII font.
4. Loops, toggling PC13 (onboard LED) every second as a heartbeat.

## Note
Code is provided as a reference template. It has not been tested on hardware.
