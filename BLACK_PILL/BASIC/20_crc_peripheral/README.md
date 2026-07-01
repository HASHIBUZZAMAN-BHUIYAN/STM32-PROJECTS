# Black Pill BASIC/20 — Hardware CRC32 Peripheral

## Overview
Uses the STM32F411's built-in CRC32 unit to compute checksums over data blocks. Demonstrates both single-block and incremental (accumulate) calculation. Compares hardware CRC against a software CRC32 to verify correctness.

## Why Hardware CRC?
- Single-cycle CRC32 per word (32 bits)
- Used for: firmware integrity, flash content verification, communication error detection
- The hardware CRC uses the standard IEEE 802.3 polynomial (0x04C11DB7)
- Free to use — no code required, just HAL calls

## STM32F411 CRC Notes
- Input: 32-bit words only (byte feeds require manual padding)
- Reset value: 0xFFFFFFFF (inverted at end → standard CRC32)
- The hardware resets the internal CRC register when you call `HAL_CRC_Calculate()`
- Use `HAL_CRC_Accumulate()` for multi-block without resetting

## Key API
```c
// Full block (resets internal state first)
uint32_t crc = HAL_CRC_Calculate(&hcrc, (uint32_t*)data, word_count);

// Accumulate over multiple calls (does NOT reset)
uint32_t crc = HAL_CRC_Accumulate(&hcrc, (uint32_t*)data, word_count);

// Reset manually before accumulate sequence
__HAL_CRC_DR_RESET(&hcrc);
```

## CubeMX Configuration
| Item | Setting |
|------|---------|
| CRC | Enable |
| USART1 | PA9/PA10, 115200 8N1 |

## Expected Output
```
Data: DEADBEEF CAFEBABE ...
HW CRC32: 0xA1B2C3D4
SW CRC32: 0xA1B2C3D4
Match: YES
```
