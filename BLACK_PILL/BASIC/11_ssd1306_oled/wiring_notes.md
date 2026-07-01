# Wiring Notes — Black Pill BASIC/11: SSD1306 OLED

## Pin Assignment
| Black Pill Pin | Signal | SSD1306 |
|---------------|--------|---------|
| PB6 | I2C1_SCL | SCL |
| PB7 | I2C1_SDA | SDA |
| 3.3V | VCC | VCC |
| GND | GND | GND |

## I2C Address
Most SSD1306 modules: **0x3C**. If your module has a solder jumper for address, check and set `OLED_ADDR` accordingly (`0x3C << 1` or `0x3D << 1`).

## I2C Pin Note (Black Pill vs Nucleo)
| Board | I2C1 Pins |
|-------|----------|
| Nucleo-F401RE | PB8=SCL, PB9=SDA |
| Black Pill F411 | PB6=SCL, PB7=SDA |

Always check your board's CubeMX I2C1 assignment.

## Font Note
The demo uses a minimal font (space + digits only). For full ASCII, include a complete 5×8 font table or use a font library like `ssd1306.h` from stm32-ssd1306.

## Parts List
| Component | Qty |
|-----------|-----|
| Black Pill (STM32F411CEU6) | 1 |
| SSD1306 OLED 128×64 I2C | 1 |
| USB-C cable | 1 |
| Jumper wires | 4 |

## 3.3V Safety
SSD1306 is 3.3V. Most modules have an onboard 3.3V regulator so they accept 3.3V or 5V on VCC. I2C signals are always 3.3V from the Black Pill.
