# Wiring Notes — Black Pill BASIC/20: Hardware CRC32

## No Extra Hardware Required
Uses only USART1 via USB-UART adapter for output.

## USART1 Connection
| Black Pill | USB-UART Adapter |
|-----------|----------------|
| PA9 (TX) | RX |
| GND | GND |

## CRC32 Polynomial Difference
The STM32 hardware CRC unit uses the standard CRC32 polynomial 0x04C11DB7 but with:
- MSB-first bit processing (no bit reversal)
- Initial value 0xFFFFFFFF
- No final XOR

Standard "CRC32/MPEG-2" matches this.
Standard "CRC32/ISO-HDLC" (used in zlib, PNG, Ethernet) uses LSB-first bit reversal.
The two produce different results for the same input — this is expected and documented.

## Applications
- Firmware integrity: compute CRC32 of flash contents, store in last 4 bytes, verify on boot
- Communication: append CRC32 to UART/SPI messages, verify on receiving end
- FatFs: FatFs uses CRC7 for SD commands (not CRC32)

## Parts List
| Component | Qty |
|-----------|-----|
| Black Pill (STM32F411CEU6) | 1 |
| USB-UART adapter | 1 |
| USB-C cable | 1 |
| Jumper wires | 2 |
