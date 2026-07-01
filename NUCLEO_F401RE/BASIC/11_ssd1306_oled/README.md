# Nucleo-F401RE BASIC/11 — SSD1306 OLED via I2C

## Overview
Drives a 128×64 SSD1306 OLED display over I2C using a minimal HAL-based driver written directly in USER CODE — no external library. Displays "Hello Nucleo" and a tick counter.

## CubeMX Configuration

| Peripheral | Setting | Pins |
|-----------|---------|------|
| I2C1 | Fast Mode, 400 kHz | PB8 (SCL), PB9 (SDA) |

## Parts List
| Component | Qty |
|-----------|-----|
| Nucleo-F401RE | 1 |
| SSD1306 128×64 OLED (I2C) | 1 |
| 4.7 kΩ resistors | 2 (if module lacks pull-ups) |

## Wiring
| OLED | Nucleo |
|------|--------|
| VCC | 3.3V |
| GND | GND |
| SCL | PB8 (D15) |
| SDA | PB9 (D14) |

## Expected Behavior
OLED initializes, clears to black, then shows "Hello Nucleo" on row 0 and a tick counter incrementing every second on row 2.
