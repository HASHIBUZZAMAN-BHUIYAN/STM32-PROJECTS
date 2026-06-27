# 22 — Hardware CRC Peripheral

## Overview
Demonstrates the STM32F103 hardware CRC unit. Computes a CRC-32 (ISO 3309 / Ethernet polynomial
0x04C11DB7) over a data buffer using `HAL_CRC_Calculate`. The result is compared to a known-good
expected value. A single-bit corruption is then injected into the buffer and the CRC is recomputed
to demonstrate mismatch detection. Results are printed over UART1.

## Hardware
- MCU: STM32F103C8T6 (Blue Pill), 72 MHz
- No external components needed beyond a USB-TTL adapter on UART1.

## CubeMX Configuration

### CRC
- Enable CRC peripheral in Peripherals panel (no parameters to set; fixed polynomial).

### USART1
| Parameter | Value |
|-----------|-------|
| Baud Rate | 115200 |
| Word Length | 8 bits |
| Parity | None |
| Stop Bits | 1 |

### GPIO
| Pin | Mode |
|-----|------|
| PA9 | USART1_TX |
| PA10 | USART1_RX |
| PC13 | GPIO_Output (onboard LED, active-low) |

## STM32 CRC Notes
- The hardware CRC uses the standard CRC-32 polynomial (0x04C11DB7).
- Input data must be 32-bit words; `HAL_CRC_Calculate` accepts a `uint32_t *` buffer and word count.
- The CRC unit is reset automatically by `HAL_CRC_Calculate` (calls `__HAL_CRC_DR_RESET`).
- Result is NOT bit-reversed relative to standard CRC-32 used in Ethernet/ZIP — the STM32 CRC
  peripheral result differs from zlib/Python `binascii.crc32`. Plan accordingly if interoperability
  is required.

## Expected Behavior (UART output)
```
=== STM32 Hardware CRC Demo ===
Buffer: 4 words
CRC Result : 0xABCDEF12   (value depends on data)
Expected   : 0xABCDEF12
PASS: CRC matches expected value

--- Corruption Test ---
Flipping bit 0 of word[0]
Corrupted CRC: 0x12345678
MISMATCH: Corruption detected!
```

## Notes
- This code has NOT been tested on hardware.
- The expected CRC constant in the source must be pre-computed for the chosen test buffer.
  The placeholder value `0x89A1897F` was derived offline; verify with your actual hardware.
