# Wiring Notes — Nucleo-F401RE ADVANCED/04: FatFs SD Card

## Pin Assignment
| Nucleo Pin | Signal | SD Card Module |
|-----------|--------|---------------|
| PB13 | SPI2_SCK | CLK |
| PB14 | SPI2_MISO | MISO (DO) |
| PB15 | SPI2_MOSI | MOSI (DI) |
| PB12 | CS (GPIO Output) | CS |
| 3.3V | VCC | VCC (if 3.3V module) |
| GND | GND | GND |
| PB8 | I2C1_SCL | BMP280 SCL |
| PB9 | I2C1_SDA | BMP280 SDA |

## Why SPI2 (Not SPI1)
SPI1 uses PA5 (SCK) which conflicts with Nucleo's LD2 LED. SPI2 (PB13/14/15) is conflict-free.

## SD Card Module Types
| Module | Logic | Level shifter | Use 3.3V |
|--------|-------|--------------|---------|
| 3.3V bare module | 3.3V | No | Yes (directly from Nucleo) |
| 5V module (blue) | 5V input | Yes (built-in) | From Nucleo 5V pin (CN7 pin 16) |

Most common "blue" SD modules on Amazon/AliExpress have a built-in 5V→3.3V LDO + level shifters. Power from the 5V pin.

## Wiring Diagram
```
Nucleo PB13 ──── SD CLK
Nucleo PB14 ──── SD MISO
Nucleo PB15 ──── SD MOSI
Nucleo PB12 ──── SD CS
Nucleo 3.3V ──── SD VCC  (3.3V module)
  OR
Nucleo 5V   ──── SD VCC  (5V module with built-in LDO)
Nucleo GND ───── SD GND
```

## user_diskio.c Note
CubeMX generates this file as a skeleton. The actual SPI SD card driver must be filled in. A minimal working implementation:
- `USER_initialize`: send 74+ dummy clocks with CS high, then CMD0 (GO_IDLE), CMD8, CMD55+ACMD41 (SDHC init)
- `USER_read`: CMD17 for single block, CMD18 for multi-block
- `USER_write`: CMD24 for single block
- `USER_ioctl`: respond to GET_SECTOR_COUNT, GET_BLOCK_SIZE
See ChaN's FatFs sample diskio.c for a reference implementation.

## SD Card Preparation
1. Insert card in PC
2. Format as **FAT32** (not exFAT, not NTFS)
3. Eject safely, insert in module

## 3.3V Safety
SD card signals are 3.3V. Do NOT connect directly to 5V. If using a module with built-in level shifters, power VCC from 5V rail and connect signals to 3.3V Nucleo pins through the module.

## Parts List
| Component | Qty | Notes |
|-----------|-----|-------|
| Nucleo-F401RE | 1 | |
| SPI SD card module | 1 | Check voltage — 3.3V or 5V module |
| MicroSD card (≤32GB FAT32) | 1 | |
| BMP280 I2C module | 1 | |
| Breadboard + jumpers | 1 set | |
| USB cable | 1 | |
