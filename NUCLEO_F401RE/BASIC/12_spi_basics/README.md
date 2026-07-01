# Nucleo-F401RE BASIC/12 — SPI Basics

## Overview
Demonstrates SPI communication using SPI2 (avoids conflict with PA5 LD2 LED on SPI1). Performs a loopback test (MISO jumpered to MOSI) and shows the pattern for communicating with a MAX7219 LED driver.

## CubeMX Configuration

| Peripheral | Setting | Pins |
|-----------|---------|------|
| SPI2 | Full-Duplex Master, 8-bit, CPOL=0 CPHA=0, Prescaler /64 (~656 kHz) | PB13 (SCK), PB14 (MISO), PB15 (MOSI) |
| GPIO PB12 | Output Push-Pull, no pull | PB12 (manual CS / NSS) |
| USART2 | Async, 115200 8N1 | PA2/PA3 — ST-Link VCP |

**Note**: SPI1 (PA5/PA6/PA7) is avoided because PA5 is the onboard LD2 LED. Use SPI2 for SPI projects on the Nucleo.

## Parts List
| Component | Qty | Notes |
|-----------|-----|-------|
| Nucleo-F401RE | 1 | |
| Jumper wire | 1 | Bridge PB14 (MISO) to PB15 (MOSI) for loopback |
| MAX7219 LED matrix (optional) | 1 | For real SPI device demo |

## Expected Behavior
**Loopback test** (MISO↔MOSI jumpered):
```
SPI Loopback test:
  TX: AA BB CC DD EE FF 00 11
  RX: AA BB CC DD EE FF 00 11
  PASS (8/8 bytes match)
```
