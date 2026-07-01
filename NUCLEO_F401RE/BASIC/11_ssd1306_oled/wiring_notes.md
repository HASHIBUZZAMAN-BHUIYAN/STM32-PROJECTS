# Wiring Notes — Nucleo-F401RE BASIC/11: SSD1306 OLED

## ⚠️ 3.3V Logic Warning
SSD1306 OLED modules are 3.3V compatible. Power from the Nucleo 3.3V pin.

## Connections
| OLED Pin | Nucleo Pin | Notes |
|----------|-----------|-------|
| VCC | 3.3V (CN7 or CN10) | |
| GND | GND | |
| SCL | PB8 (Arduino D15) | 4.7 kΩ pull-up to 3.3V |
| SDA | PB9 (Arduino D14) | 4.7 kΩ pull-up to 3.3V |

Most SSD1306 modules include onboard pull-up resistors — check before adding externals.

## I2C Address
Default: **0x3C**. Some modules use **0x3D** (SA0 pulled high). Check your module's markings.

## Parts List
| Component | Qty |
|-----------|-----|
| Nucleo-F401RE | 1 |
| SSD1306 128×64 OLED | 1 |
| 4.7 kΩ resistors | 2 (if needed) |
| USB cable | 1 |

## Font Note
The built-in font in this project covers digits 0-9 only. To display full ASCII text, add a complete 5×7 ASCII font array (many open-source examples available — just paste into USER CODE).
