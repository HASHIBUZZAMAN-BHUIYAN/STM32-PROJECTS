# Wiring Notes — Nucleo-F401RE BASIC/30: Capstone Connected Sensor Station

## Pin Assignment
| Nucleo Pin | Signal | Connected To |
|-----------|--------|-------------|
| PB8 | I2C1_SCL | BMP280 SCL + SSD1306 SCL |
| PB9 | I2C1_SDA | BMP280 SDA + SSD1306 SDA |
| PA8 | DHT11_DATA | DHT11 data pin (via 10kΩ pull-up to 3.3V) |
| PA5 | LD2 LED | Heartbeat (onboard) |
| PA2/PA3 | USART2 TX/RX | ST-Link VCP |
| 3.3V | VCC | BMP280 VCC + SSD1306 VCC + DHT11 VCC |
| GND | GND | All modules GND |

## Wiring Diagram
```
Nucleo 3.3V ──┬──── BMP280 VCC
              ├──── SSD1306 VCC
              └──── DHT11 VCC
                    DHT11 DATA ── 10kΩ ── 3.3V
                    DHT11 DATA ── PA8

Nucleo GND ───┬──── BMP280 GND
              ├──── SSD1306 GND
              └──── DHT11 GND

Nucleo PB8 ───┬──── BMP280 SCL (470Ω series resistor optional)
              └──── SSD1306 SCL

Nucleo PB9 ───┬──── BMP280 SDA (470Ω series resistor optional)
              └──── SSD1306 SDA
```

## I2C Address Configuration
- BMP280: SDO → GND = 0x76 (SDO → 3.3V = 0x77)
- SSD1306: fixed at 0x3C (most modules)

## Parts List
| Component | Qty | Notes |
|-----------|-----|-------|
| Nucleo-F401RE | 1 | |
| BMP280 module | 1 | I2C breakout |
| SSD1306 OLED 128×64 | 1 | I2C, 4-pin |
| DHT11 sensor | 1 | Or DHT22 for better accuracy |
| 10kΩ resistor | 1 | DHT11 pull-up |
| Breadboard + jumpers | 1 set | |
| USB cable | 1 | |

## DHT11 Timing Notes
DHT11 uses a proprietary single-wire timing protocol:
- Start: pull low 18ms, release → 40µs high
- Response: DHT pulls low 80µs then high 80µs
- Each bit: low 50µs → high; 26–28µs = 0, 70µs = 1
- Sample at ~35µs into high period to distinguish 0 from 1
- **Minimum interval between DHT11 reads: 2 seconds**
- DHT22 is a drop-in hardware replacement with better accuracy (0.1°C vs 1°C)

## 3.3V Safety Reminders
- BMP280: MUST be 3.3V module — bare chip is 3.3V only
- SSD1306: most modules have onboard 3.3V/5V LDO — check your module
- DHT11: runs on 3.3V–5V, but **data pull-up must match VCC** — use 3.3V here
- NEVER connect 5V signals to Nucleo GPIO pins — permanent damage

## Terminal Settings
115200 baud, 8N1. Commands typed then press Enter:
- `read` — force immediate sensor read and JSON output
- `info` — print board information
