# Wiring Notes — 10_lcd_i2c

## CRITICAL: 3.3 V Logic

**All STM32F103C8T6 GPIO pins operate at 3.3 V.**
PB6 (SCL) and PB7 (SDA) are 3.3 V open-drain signals.

## LCD Module Voltage — Special Case

Most 16x2 LCD + PCF8574 backpack modules are designed to run on **5 V VCC**.
The LCD backlight and contrast circuit require 5 V to operate correctly.

However, the PCF8574 I2C pins on typical cheap modules are **3.3 V tolerant**
because the PCF8574 I/O operates at its VCC (5 V) but the MCU's 3.3 V signal
is usually enough to be recognized as a HIGH by the PCF8574's input threshold.

**Verify this with your specific module before powering on.**
If the module is not 3.3 V tolerant on I2C lines, use a **bidirectional
I2C level shifter** (e.g., a module based on BSS138 MOSFETs) between the
Blue Pill (3.3 V side) and the LCD backpack (5 V side).

## Connections

| Blue Pill Pin | LCD Backpack Pin | Notes |
|---------------|------------------|-------|
| PB6 | SCL | + 4.7 kΩ pull-up to 3.3 V (or 5 V side — see below) |
| PB7 | SDA | + 4.7 kΩ pull-up to 3.3 V (or 5 V side — see below) |
| GND | GND | Common ground — connect first |
| 5V (USB) | VCC | LCD module powered from 5 V |

The Blue Pill exposes a 5V pin that passes through the USB connector's 5 V rail
(when powered via USB). This can be used to power the LCD module VCC.

## Pull-Up Resistor Placement

If you use a level shifter:
- Pull-up resistors on the **low-voltage side** (3.3 V) go to 3.3 V.
- Pull-up resistors on the **high-voltage side** (5 V) go to 5 V.
- Many I2C level-shifter modules include pull-ups.

If connecting directly (verified 3.3 V tolerant module only):
- 4.7 kΩ from PB6 to 3.3 V
- 4.7 kΩ from PB7 to 3.3 V

## Wiring Diagram (Direct, No Level Shifter)

```
Blue Pill                     LCD I2C Backpack (PCF8574)
─────────                     ──────────────────────────
PB6 ──[4.7kΩ to 3V3]──────── SCL
PB7 ──[4.7kΩ to 3V3]──────── SDA
GND ─────────────────────── GND
5V ──────────────────────── VCC  (5V from USB)
```

## Contrast Adjustment

Most PCF8574 backpack modules have a small trimmer potentiometer on the board.
Adjust it after powering on to make the character blocks visible.
If the display is blank but backlight is on, contrast is likely set wrong.

## Identifying Your Module's Address

Run project **09_i2c_bus_scan** first to find the PCF8574's I2C address.
Update `LCD_I2C_ADDR` in `main_usercode.c` accordingly:

| Module type | Default address | Define value |
|-------------|-----------------|--------------|
| PCF8574 | 0x27 | `(0x27U << 1U)` |
| PCF8574A | 0x3F | `(0x3FU << 1U)` |

## Common Problems

| Symptom | Likely cause |
|---------|-------------|
| Backlight on, no characters | Contrast too low — turn trimmer |
| No backlight, no characters | VCC not 5 V, or wrong I2C address |
| Garbled characters | Wrong I2C address or bit mapping |
| I2C timeout / HAL error | Missing pull-ups, bad wiring, wrong address |
| Only top row visible | Contrast too high — turn trimmer |
