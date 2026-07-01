# Nucleo-F401RE ADVANCED/07 — Internal Flash EEPROM Emulation

## Overview
Demonstrates reading and writing the STM32F401's internal flash to persist configuration data across power cycles. Uses Sector 7 (the last 128 KB sector) so the bootloader and firmware remain intact. Implements a minimal key-value store with sector erase + write.

## Why Flash, Not Real EEPROM?
The STM32F401 has no dedicated EEPROM. Options:
1. **Internal flash** (this project) — free, no external hardware, but limited erase cycles (~10,000) and must erase an entire sector
2. **External I2C EEPROM** (AT24C256) — easier wear management, slower
3. **Flash with EEPROM emulation** — ST's AN2594 dual-page scheme for wear leveling

This project demonstrates the raw HAL Flash API. For production, use ST's EEPROM emulation library.

## STM32F401 Flash Sectors
| Sector | Address | Size |
|--------|---------|------|
| 0 | 0x08000000 | 16 KB (bootloader area) |
| 1 | 0x08004000 | 16 KB |
| 2 | 0x08008000 | 16 KB |
| 3 | 0x0800C000 | 16 KB |
| 4 | 0x08010000 | 64 KB |
| 5 | 0x08020000 | 128 KB |
| 6 | 0x08040000 | 128 KB |
| 7 | 0x08060000 | 128 KB ← this project uses this |

## CubeMX Configuration
USART2 only. No special flash peripheral to enable — HAL_FLASH_Unlock/Lock API is always available.

## Key HAL API
```c
HAL_FLASH_Unlock();
// Erase sector (entire 128KB erased to 0xFF)
FLASH_EraseInitTypeDef e = { .TypeErase=FLASH_TYPEERASE_SECTORS,
    .Sector=FLASH_SECTOR_7, .NbSectors=1, .VoltageRange=FLASH_VOLTAGE_RANGE_3 };
uint32_t err;
HAL_FLASHEx_Erase(&e, &err);
// Write 4 bytes (must be aligned, written once per location)
HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, data);
HAL_FLASH_Lock();
```

## Data Lifetime
- Flash write endurance: ~10,000 program/erase cycles
- At one write per boot: 10,000 boots before wear-out
- Use erase only when full; append-write within sector to extend life

## Expected UART Output
```
Boot #1: booting fresh
Boot #2: stored val=42 boot_count=2
Boot #3: stored val=42 boot_count=3
```
