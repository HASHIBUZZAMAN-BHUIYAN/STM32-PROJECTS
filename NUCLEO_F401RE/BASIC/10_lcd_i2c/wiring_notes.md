# Wiring Notes — Nucleo-F401RE BASIC/10: 16x2 LCD via I2C

## ⚠️ 3.3V Logic Warning
Nucleo GPIO pins are 3.3V. PCF8574 I2C backpacks typically accept 3.3V logic signals even when the LCD itself runs at 5V. Verify your specific backpack module.

## Connections
| PCF8574 Backpack | Nucleo Pin | Notes |
|-----------------|-----------|-------|
| VCC | 5V (CN7 pin 18 or CN10 pin 8) | LCD needs 5V for contrast |
| GND | GND | |
| SCL | PB8 (D15) | |
| SDA | PB9 (D14) | |

## I2C Address
- Default: **0x27** (A0=A1=A2=LOW on PCF8574)
- Alternative: **0x3F** (PCF8574A variant)
- Run project BASIC/09 (I2C bus scan) to confirm your module's address, then update `LCD_ADDR` in the code

## Parts List
| Component | Qty |
|-----------|-----|
| Nucleo-F401RE | 1 |
| 16×2 HD44780 LCD | 1 |
| PCF8574 I2C backpack | 1 |
| USB cable | 1 |

## Contrast Adjustment
The PCF8574 backpack usually has a small trimmer potentiometer for LCD contrast. Adjust it until characters are clearly visible. If you see all blocks or nothing, the contrast is set wrong.
