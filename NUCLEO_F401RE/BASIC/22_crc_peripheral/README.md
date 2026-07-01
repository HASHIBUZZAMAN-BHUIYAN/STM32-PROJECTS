# Nucleo-F401RE BASIC/22 — Hardware CRC32

## Overview
Uses the STM32F401's built-in CRC32 calculation unit (hardware peripheral, not software) to verify data integrity of a buffer. Demonstrates single-call and incremental CRC computation, then shows how a 1-bit flip changes the result — a real data integrity check pattern.

## CubeMX Configuration

| Peripheral | Setting |
|-----------|---------|
| CRC | Enable (Peripherals → CRC) |
| USART2 | 115200 8N1, PA2/PA3 |

No pin assignment needed — CRC is an internal peripheral.

## Expected Behavior
```
CRC32 Test:
  Buffer CRC: 0x29B1D71F
  Expected:   0x29B1D71F  -> PASS
  Flipped bit CRC: 0xA482E4D0  -> DIFFERENT (corruption detected)
```
