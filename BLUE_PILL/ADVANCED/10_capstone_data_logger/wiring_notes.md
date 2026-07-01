# Wiring Notes — Blue Pill ADVANCED/10: FreeRTOS Data Logger

## ⚠️ 3.3V Logic Warning
All Blue Pill GPIO pins are 3.3V. BMP280 and SSD1306 accept 3.3V power and signals. SD card module must also be 3.3V compatible (level-shifted module). Never apply 5V to any MCU pin.

## I2C Bus (shared by BMP280 + SSD1306)
| Signal | Blue Pill | Notes |
|--------|-----------|-------|
| SCL | PB6 | 4.7kΩ pull-up to 3.3V |
| SDA | PB7 | 4.7kΩ pull-up to 3.3V |

### BMP280
| BMP280 | Blue Pill |
|--------|-----------|
| VCC | 3.3V |
| GND | GND |
| SCL | PB6 |
| SDA | PB7 |
| SDO | GND (addr 0x76) |
| CSB | 3.3V |

### SSD1306 OLED
| OLED | Blue Pill |
|------|-----------|
| VCC | 3.3V |
| GND | GND |
| SCL | PB6 |
| SDA | PB7 |

## SPI2 SD Card
| SD Module | Blue Pill |
|-----------|-----------|
| VCC | 3.3V (use 3.3V-compatible module!) |
| GND | GND |
| SCK | PB13 |
| MISO | PB14 |
| MOSI | PB15 |
| CS | PB12 |

## UART
| Signal | Blue Pill | USB-Serial Adapter |
|--------|-----------|-------------------|
| TX | PA9 | RX |
| RX | PA10 | TX |
| GND | GND | GND |

3.3V-level adapter (CP2102 / CH340G at 3.3V). Terminal: 115200 8N1.

## ST-Link (programming)
| Signal | Blue Pill | ST-Link V2 |
|--------|-----------|-----------|
| SWDIO | PA13 | SWDIO |
| SWCLK | PA14 | SWCLK |
| GND | GND | GND |
| 3.3V | 3.3V | 3.3V (power from ST-Link) |

## Parts List
| Component | Qty |
|-----------|-----|
| Blue Pill STM32F103C8T6 | 1 |
| BMP280 breakout | 1 |
| SSD1306 128x64 OLED | 1 |
| SPI SD card module (3.3V) | 1 |
| microSD card (FAT32 formatted) | 1 |
| 4.7kΩ resistors | 2 |
| USB-serial adapter (3.3V) | 1 |
| ST-Link V2 | 1 |
| Breadboard + jumper wires | 1 set |

## Notes
- Both BMP280 and SSD1306 share I2C1 — protected by FreeRTOS mutex in firmware
- SD card CS (PB12) must be held HIGH when idle; firmware handles this
- FAT32 format the SD card before first use
- IWDG timeout is ~3s; if any task hangs, the board resets automatically
