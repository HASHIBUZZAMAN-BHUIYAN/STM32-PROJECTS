# Nucleo-F401RE BASIC/10 — 16x2 LCD via I2C Backpack

## Overview
Drives a standard 16×2 character LCD connected via a PCF8574 I2C expander backpack. All LCD communication uses `HAL_I2C_Master_Transmit` directly — no external library. Demonstrates I2C master transmit and LCD nibble-mode protocol.

## CubeMX Configuration

### Part / Board: NUCLEO-F401RE

| Peripheral | Setting | Pins |
|-----------|---------|------|
| I2C1 | Fast Mode, 400 kHz | PB8 (SCL), PB9 (SDA) |
| USART2 | Async, 115200 8N1 | PA2/PA3 — ST-Link VCP |

## Parts List
| Component | Qty | Notes |
|-----------|-----|-------|
| Nucleo-F401RE | 1 | |
| 16×2 LCD module | 1 | HD44780 compatible |
| PCF8574 I2C backpack | 1 | Pre-soldered to LCD; typical addr 0x27 or 0x3F |
| USB cable | 1 | ST-Link VCP |

## Wiring
| I2C Backpack | Nucleo |
|-------------|--------|
| VCC | 5V (CN7 pin 18) — backpack has its own 3.3V→5V for LCD logic |
| GND | GND |
| SCL | PB8 (D15) |
| SDA | PB9 (D14) |

Note: Most PCF8574 backpacks accept 3.3V I2C signals even when the LCD is powered at 5V. Verify your module's datasheet.

## How to Build & Flash
1. Configure I2C1 in CubeMX, generate project
2. Paste `main_usercode.c` into `main.c` USER CODE sections
3. Build and flash via ST-Link

## Expected Behavior
Line 1: `Nucleo F401RE`  
Line 2: `I2C LCD Ready!` then scrolls a counter every second.
