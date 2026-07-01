# Wiring Notes — Black Pill BASIC/10: LCD I2C (PCF8574)

## Pin Assignment
| Black Pill Pin | Signal | LCD Backpack |
|---------------|--------|-------------|
| PB6 | I2C1_SCL | SCL |
| PB7 | I2C1_SDA | SDA |
| 5V (VBUS) | LCD VCC | VCC (better contrast/backlight) |
| GND | GND | GND |

## Note on Voltage
- PCF8574 logic: can run at 3.3V
- LCD backlight + contrast: works better at 5V
- I2C lines stay at 3.3V — safe because PCF8574 SDA/SCL are 5V-tolerant when VCC=3.3V

## I2C Address
- Most PCF8574 modules: **0x27** (address pins A0=A1=A2=1)
- PCF8574**A** variant: **0x3F**
- Run BASIC/09 (I2C Bus Scan) if uncertain about your module's address

## Pull-up Resistors
Most PCF8574 backpack modules include 4.7kΩ pull-up resistors onboard. If I2C is unreliable, check:
- External 2.2kΩ pull-ups for 400kHz Fast Mode
- Short wire length (<30cm recommended)

## Black Pill I2C1 Pins
Black Pill uses PB6/PB7 for I2C1 (different from Nucleo which uses PB8/PB9). Check your board pinout diagram.

## Parts List
| Component | Qty | Notes |
|-----------|-----|-------|
| Black Pill (STM32F411CEU6) | 1 | |
| 16x2 LCD with PCF8574 I2C backpack | 1 | Address 0x27 or 0x3F |
| USB-C cable | 1 | |
| Jumper wires | 4 | |

## Troubleshooting
- All blocks on LCD: wrong I2C address — change `LCD_ADDR` to `(0x3F << 1)`
- No backlight: check VCC connection (needs 5V for backlight)
- I2C error: check pull-up resistors and wiring

## 3.3V Safety
I2C lines (PB6/PB7) are 3.3V. The PCF8574 handles the level for the LCD. Never directly connect 5V to Black Pill GPIO pins.
