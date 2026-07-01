# Wiring Notes — Black Pill ADVANCED/05: SD Card FatFs

## SPI1 Advantage on Black Pill
Unlike Nucleo (PA5 = LED → forced to use SPI2), Black Pill has PC13 for the LED.
SPI1 on PA5/PA6/PA7 is fully free.

## SD Card Module Wiring (SPI1)
| SD Module | Black Pill | Note |
|-----------|-----------|------|
| VCC | 3.3V | Use 3.3V SD module — NOT 5V |
| GND | GND | |
| SCK | PA5 | SPI1_SCK |
| MOSI | PA7 | SPI1_MOSI |
| MISO | PA6 | SPI1_MISO |
| CS | PA4 | GPIO Output (SW CS) |

## BMP280 Wiring (I2C1)
| BMP280 | Black Pill | Note |
|--------|-----------|------|
| VCC | 3.3V | |
| GND | GND | |
| SCL | PB6 | I2C1_SCL (Black Pill = PB6, not PB8) |
| SDA | PB7 | I2C1_SDA (Black Pill = PB7, not PB9) |
| SDO | GND | Sets I2C addr 0x76 |

## USART1 Connection
| Black Pill | USB-UART |
|-----------|---------|
| PA9 (TX) | RX |
| PA10 (RX) | TX |
| GND | GND |

## SD Card Preparation
1. Format with FAT32 (not exFAT) using Windows/Linux
2. ≤32GB recommended for reliable FAT32

## CubeMX FatFs Configuration
- Enable FatFs middleware
- Interface: User-defined (fill `user_diskio.c`)
- MAX_SS = 512
- _USE_LFN = 0 (short filenames only)

## Parts List
| Component | Qty |
|-----------|-----|
| Black Pill (STM32F411CEU6) | 1 |
| Micro-SD SPI module (3.3V) | 1 |
| BMP280 module | 1 |
| Micro-SD card (FAT32, ≤32GB) | 1 |
| USB-UART adapter | 1 |
| Jumper wires | 12 |
