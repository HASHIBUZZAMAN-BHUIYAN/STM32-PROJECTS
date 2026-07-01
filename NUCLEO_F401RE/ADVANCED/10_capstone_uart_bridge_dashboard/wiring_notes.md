# Wiring Notes — Nucleo-F401RE ADVANCED/10: Capstone Dashboard

## Full Pin Table
| Nucleo Pin | Signal | Component |
|-----------|--------|----------|
| PB8 | I2C1_SCL | BMP280 SCL |
| PB9 | I2C1_SDA | BMP280 SDA |
| PB13 | SPI2_SCK | SD card CLK |
| PB14 | SPI2_MISO | SD card MISO |
| PB15 | SPI2_MOSI | SD card MOSI |
| PB12 | GPIO Output | SD card CS |
| PA8 | GPIO (DHT11 data) | DHT11 data pin |
| PA2/PA3 | USART2 TX/RX | ST-Link VCP |
| PA5 | LD2 | Heartbeat |
| 3.3V | VCC | BMP280, DHT11 |
| GND | GND | All components |

## Wiring Diagram
```
Nucleo PB8  ──── BMP280 SCL
Nucleo PB9  ──── BMP280 SDA
Nucleo 3.3V ─┬── BMP280 VCC
              └── DHT11 VCC ──── 10kΩ ──┐
                                        │
Nucleo PA8  ────────────────── DHT11 DATA
Nucleo GND  ─┬── BMP280 GND
              └── DHT11 GND

Nucleo PB13 ──── SD_CLK
Nucleo PB14 ──── SD_MISO
Nucleo PB15 ──── SD_MOSI
Nucleo PB12 ──── SD_CS
Nucleo GND  ──── SD_GND
SD module VCC ── (3.3V or 5V per module type)
```

## SD Card Module Note
Use SPI2 (PB13-15), NOT SPI1 — SPI1 SCK is PA5, which conflicts with LD2 LED.

## FreeRTOS Heap Budget
```
Task         Stack (bytes)  Approx usage
Sensor       2048           ~800B (I2C + DHT)
Log          2048           ~1.5KB (FatFs internal buffers)
Display      1024           ~400B (snprintf)
Cmd          1024           ~300B
FreeRTOS     ~1KB           overhead
Total: ~8KB of 12KB heap
```

## Parts List
| Component | Qty | Notes |
|-----------|-----|-------|
| Nucleo-F401RE | 1 | |
| BMP280 I2C module | 1 | |
| DHT11 sensor | 1 | |
| 10kΩ resistor | 1 | DHT11 pull-up |
| SPI SD card module | 1 | FAT32 formatted |
| MicroSD card (≤32GB) | 1 | |
| Breadboard + jumpers | 1 set | |
| USB cable | 1 | |

## 3.3V Safety
- BMP280: 3.3V only
- DHT11: 3.3V (pull-up to 3.3V)
- SD card module: check type (3.3V or 5V with LDO)
- All Nucleo GPIO: 3.3V logic — never connect 5V signals
