# Black Pill ADVANCED/05 — SD Card with FatFs

## Overview
Logs timestamped BMP280 readings (temperature + pressure) to a FAT32 SD card via SPI1. On Black Pill, SPI1 (PA5/PA6/PA7) is completely free — no LED conflict like on the Nucleo. Includes UART commands for reading back the log.

## Key Difference from Nucleo
Nucleo used SPI2 (PB13/PB14/PB15) to avoid the PA5 LED conflict. Black Pill has PC13 for the LED, so SPI1 on PA5/PA6/PA7 is available.

## SPI1 Pin Mapping
| SPI1 Signal | Black Pill Pin |
|-------------|---------------|
| SCK | PA5 |
| MISO | PA6 |
| MOSI | PA7 |
| CS (GPIO) | PA4 |

## FatFs File Layout
- File: `LOG.CSV`
- Format: `tick_ms,temp_c,pressure_pa\r\n`
- Append mode: data accumulates across power cycles
- Flush: `f_sync()` every 10 records instead of open/close per write

## CubeMX
- SPI1: Full-Duplex Master, 8-bit, CPOL=Low, CPHA=1Edge
- SPI Baud: ≤400 kHz for init, then bump to ≤25 MHz for data
- FatFs middleware enabled, user_diskio.c must be filled in

## UART Commands
| Command | Action |
|---------|--------|
| `dump` | Print last 20 lines from LOG.CSV |
| `erase` | Delete LOG.CSV and recreate |
| `info` | Print SD card size and used space |
| `status` | Print last written values |
