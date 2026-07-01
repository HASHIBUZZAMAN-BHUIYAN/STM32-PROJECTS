# Wiring Notes — Black Pill BASIC/24: Multi-Peripheral Integration

## Pin Assignment
| Black Pill Pin | Signal | Component |
|---------------|--------|----------|
| PB6 | I2C1_SCL | BMP280 SCL + SSD1306 SCL |
| PB7 | I2C1_SDA | BMP280 SDA + SSD1306 SDA |
| PA1 | ADC1_IN1 | Potentiometer wiper |
| PA9 | USART1_TX | USB-UART adapter RX |
| PC13 | LED (active-LOW) | Heartbeat |
| 3.3V | VCC | BMP280 + SSD1306 |
| GND | GND | All components |

## Wiring
```
Black Pill PB6 ──┬── BMP280 SCL
                 └── SSD1306 SCL

Black Pill PB7 ──┬── BMP280 SDA
                 └── SSD1306 SDA

Black Pill PA1 ── Pot wiper (pot between 3.3V and GND)
Black Pill 3.3V ─┬── BMP280 VCC
                 └── SSD1306 VCC
Black Pill GND  ─┬── BMP280 GND
                 └── SSD1306 GND
                 └── Pot GND
```

## I2C Addresses
- BMP280: 0x76 (SDO → GND)
- SSD1306: 0x3C

Both on same I2C1 bus — only one transmits at a time (sequential access from main loop).

## Parts List
| Component | Qty |
|-----------|-----|
| Black Pill (STM32F411CEU6) | 1 |
| BMP280 I2C module | 1 |
| SSD1306 OLED 128×64 | 1 |
| 10kΩ potentiometer | 1 |
| USB-UART adapter | 1 |
| USB-C cable | 1 |
| Breadboard + jumpers | 1 set |

## 3.3V Safety
All components are 3.3V logic. Never connect 5V to Black Pill GPIO or I2C lines.
