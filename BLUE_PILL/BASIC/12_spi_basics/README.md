# 12 — SPI1 Basics & Loopback Test

## Overview
Demonstrates SPI1 Full-Duplex Master communication on the Blue Pill.
A loopback test is performed by jumpering MISO (PA6) to MOSI (PA7): every byte
transmitted is immediately received back and compared.  A second code section
shows the pattern used to communicate with a generic SPI peripheral.

## Hardware
- Board : STM32F103C8T6 (Blue Pill)
- SPI1 pins : PA5 (SCK), PA6 (MISO), PA7 (MOSI)
- NSS (CS) : PA4 — software-controlled GPIO output

## CubeMX Settings
| Peripheral | Setting |
|------------|---------|
| SPI1 | Full-Duplex Master |
| SPI1 Data Size | 8-bit |
| SPI1 Frame Format | Motorola |
| SPI1 Prescaler | /8 → 9 MHz (adjust as needed) |
| SPI1 CPOL / CPHA | Low / 1 Edge (Mode 0) |
| NSS | Software (PA4 manual GPIO output, default HIGH) |
| SYS | SysTick as timebase |

## Files
| File | Purpose |
|------|---------|
| `main_usercode.c` | Loopback test + generic SPI device pattern |
| `wiring_notes.md` | Pin connections and hardware caveats |

## What the Code Does
1. Pulls CS (PA4) HIGH on init.
2. Builds a 16-byte test buffer {0x00 … 0x0F}.
3. Asserts CS LOW, calls `HAL_SPI_TransmitReceive`, de-asserts CS.
4. Compares TX and RX buffers byte-by-byte; blinks LED fast on mismatch.
5. Indicates pass/fail via UART1 (optional — PA9 TX).
6. Shows a generic "write register" helper as a commented example.

## Loopback Jumper
For the loopback test, connect a wire from **PA6 (MISO) → PA7 (MOSI)**.
Remove the jumper before connecting a real SPI device.

## Note
Code is provided as a reference template. It has not been tested on hardware.
