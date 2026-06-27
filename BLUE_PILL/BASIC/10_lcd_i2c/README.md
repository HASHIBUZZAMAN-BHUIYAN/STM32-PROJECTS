# 10 — 16x2 LCD via I2C (PCF8574 Backpack)

## Overview

Drives a standard 16x2 HD44780-compatible LCD through a **PCF8574 I2C I/O expander**
backpack module. No external LCD library is used — the entire driver is written
in user code using `HAL_I2C_Master_Transmit`.

Functions implemented:
- `lcd_init()` — initializes the LCD in 4-bit mode
- `lcd_send_cmd(cmd)` — sends a command byte (RS=0)
- `lcd_send_data(data)` — sends a character byte (RS=1)
- `lcd_print(str)` — prints a null-terminated string
- `lcd_set_cursor(row, col)` — positions the cursor

Display shows:
- Line 1: `STM32 Blue Pill`
- Line 2: `I2C LCD Test`

## Hardware

| Signal | Pin |
|--------|-----|
| I2C1 SCL | PB6 |
| I2C1 SDA | PB7 |
| LCD Backpack VCC | 5 V (see wiring notes) |
| LCD Backpack GND | GND |

## PCF8574 I2C Address

| Module marking | Typical address |
|----------------|-----------------|
| PCF8574 (generic) | 0x27 |
| PCF8574A (NXP) | 0x3F |

The address can be changed by soldering/bridging the A0/A1/A2 jumpers on the
backpack. Run project 09 first to detect the actual address on your module.

## PCF8574 Backpack Pin Mapping

The backpack connects PCF8574 output pins to the LCD as follows
(this is the standard for most cheap modules):

| PCF8574 Bit | LCD Pin | Function |
|-------------|---------|----------|
| P0 | RS | Register Select |
| P1 | RW | Read/Write (tied to GND = write) |
| P2 | EN | Enable |
| P3 | BL | Backlight (active-high via transistor) |
| P4 | D4 | Data bit 4 |
| P5 | D5 | Data bit 5 |
| P6 | D6 | Data bit 6 |
| P7 | D7 | Data bit 7 |

The driver in this project uses this standard mapping.

## CubeMX Configuration

### I2C1
- Mode: I2C, Standard Mode, 100 kHz
- PB6: I2C1_SCL (AF Open-Drain)
- PB7: I2C1_SDA (AF Open-Drain)

### Clock
- 72 MHz system clock

## Build & Flash

1. Configure CubeMX as above.
2. Set `LCD_I2C_ADDR` define to `0x27` or `0x3F` to match your module.
3. Copy user code sections from `main_usercode.c` into generated `main.c`.
4. Build and flash via ST-Link.

## Expected Behavior

- LCD backlight turns on.
- Line 1 displays: `STM32 Blue Pill`
- Line 2 displays: `I2C LCD Test`
- Onboard LED blinks at 1 Hz.

## Notes

- Code is **not tested on hardware**.
- The LCD module typically runs on **5 V VCC** but has 3.3 V-tolerant I2C lines
  on the PCF8574. See `wiring_notes.md` for details.
- Timing delays in the LCD init sequence (> 40 ms after power-on) are handled
  with `HAL_Delay` calls — do not reduce them.
- 3.3 V logic — see `wiring_notes.md`.
