# Nucleo-F401RE ADVANCED/04 — FatFs SD Card Logging

## Overview
Mounts a FAT32 SD card over SPI2 using the FatFs middleware from CubeMX. On each boot the firmware appends a timestamped line to `LOG.CSV` on the SD card, reads BMP280 temperature, logs it with HAL_GetTick(), then closes the file.

## CubeMX Configuration
| Item | Setting |
|------|---------|
| SPI2 | PB13=SCK, PB14=MISO, PB15=MOSI, Full-Duplex Master |
| SPI2 baud | ≤400 kHz during init, switch to ≤25 MHz after mount |
| GPIO PB12 | CS (chip select), Output Push-Pull |
| FatFs | User-defined (SPI) |
| USART2 | PA2/PA3, 115200 8N1 |
| I2C1 | PB8/PB9, 400 kHz (for BMP280) |

**Note**: SPI1 (PA5/PA6/PA7) conflicts with Nucleo LD2 LED on PA5. Always use **SPI2** on this board.

## FatFs Workflow
```c
FATFS fs;
FIL   file;
f_mount(&fs, "", 1);               // mount
f_open(&file, "LOG.CSV", FA_WRITE|FA_OPEN_APPEND);
f_printf(&file, "%lu,%.2f\r\n", HAL_GetTick(), temp);
f_close(&file);                    // MUST close to flush
f_mount(NULL, "", 0);              // unmount (optional but safe)
```

## user_diskio.c
CubeMX generates a skeleton `user_diskio.c`. You must fill in:
- `USER_initialize()` — init SPI, assert CS high
- `USER_status()` — return RES_OK if card detected
- `USER_read()` — SPI read sectors
- `USER_write()` — SPI write sectors
- `USER_ioctl()` — GET_SECTOR_COUNT, GET_BLOCK_SIZE

A complete reference implementation is in the STM32_SETUP.md appendix.

## Expected SD Card File (LOG.CSV)
```
tick_ms,temp_c
100,24.50
5100,24.51
10100,24.52
```

## Common Pitfalls
1. **CS must idle HIGH** — SD card SPI uses active-low CS
2. **Init at low speed** (≤400 kHz) — many cards reject fast clock during CMD0/CMD8
3. **f_close is mandatory** — FatFs buffers writes; if you power off before f_close, last sector is lost
4. **FAT32 format** — format card on PC as FAT32 (not exFAT, not NTFS)
5. **3.3V logic only** — SD cards are 3.3V; use a level shifter if the module has one for 5V hosts
