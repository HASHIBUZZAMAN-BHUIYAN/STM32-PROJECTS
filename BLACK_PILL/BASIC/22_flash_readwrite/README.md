# Black Pill BASIC/22 — Internal Flash Read/Write

## Overview
Reads and writes the STM32F411's internal flash using the HAL Flash API. Uses Sector 7 (last 128KB) for data storage, leaving Sectors 0-6 (384KB) for firmware. Demonstrates erase-before-write, word-aligned writes, and readback verification.

## STM32F411 Flash Layout
| Sector | Address | Size |
|--------|---------|------|
| 0 | 0x08000000 | 16 KB |
| 1 | 0x08004000 | 16 KB |
| 2 | 0x08008000 | 16 KB |
| 3 | 0x0800C000 | 16 KB |
| 4 | 0x08010000 | 64 KB |
| 5 | 0x08020000 | 128 KB |
| 6 | 0x08040000 | 128 KB |
| **7** | **0x08060000** | **128 KB ← user data** |

Total: 512 KB (F411 has more flash than F401: both 512KB, but sector layout differs slightly from F103)

## HAL Flash API
```c
HAL_FLASH_Unlock();
// Erase sector:
FLASH_EraseInitTypeDef e = {
    .TypeErase = FLASH_TYPEERASE_SECTORS,
    .Sector    = FLASH_SECTOR_7,
    .NbSectors = 1,
    .VoltageRange = FLASH_VOLTAGE_RANGE_3
};
uint32_t err;
HAL_FLASHEx_Erase(&e, &err);
// Write word:
HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, 0x08060000, 0x12345678UL);
// Read: direct pointer access
uint32_t v = *(__IO uint32_t*)0x08060000;
HAL_FLASH_Lock();
```

## Linker Script
Edit `STM32F411CETx_FLASH.ld`: reduce FLASH LENGTH from 512K to 384K to prevent linker from placing code in Sector 7.

## Expected Output
```
Erasing Sector 7...
Writing 16 words...
Readback verify: OK
Data[0]: 0x00000001 OK
Data[15]: 0x00000010 OK
```
