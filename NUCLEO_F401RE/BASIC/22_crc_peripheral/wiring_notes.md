# Wiring Notes — Nucleo-F401RE BASIC/22: Hardware CRC32

## No External Wiring Required
CRC is an internal hardware peripheral — no pins, no external components.

## How the STM32 CRC Unit Works
- Uses CRC-32 (ISO 3309 polynomial: 0x04C11DB7)
- HAL_CRC_Calculate: resets and computes in one call
- HAL_CRC_Accumulate: continues computation without reset (incremental)
- Input word order matters: data is fed 32 bits at a time
- Output: 32-bit CRC value

## Use Cases in Embedded Systems
- Verifying flash memory integrity on boot
- Checking received data packets against transmitted CRC
- Config block validation before use

## Parts List
| Component | Qty |
|-----------|-----|
| Nucleo-F401RE | 1 |
| USB cable | 1 |
