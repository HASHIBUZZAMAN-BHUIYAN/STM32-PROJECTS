# 07 — UART Basics (Transmit & Blocking Receive)

## Overview

Demonstrates the two fundamental UART HAL calls:

1. **HAL_UART_Transmit** — sends `"Hello STM32\r\n"` every second.
2. **HAL_UART_Receive** — waits up to 1 second for a single byte; if received, echoes it back.

Both calls are blocking (polled). See project 08 for interrupt-driven receive.

## Hardware

| Signal | Pin | Direction |
|--------|-----|-----------|
| USART1 TX | PA9 | MCU → Host |
| USART1 RX | PA10 | Host → MCU |
| Onboard LED | PC13 (active-low) | Output |

## CubeMX Configuration

### USART1
- Mode: Asynchronous
- Baud Rate: 115200
- Word Length: 8 bits
- Parity: None
- Stop Bits: 1
- Hardware Flow Control: None
- NVIC: USART1 global interrupt — **disabled** (polled mode)

### GPIO
- PA9: Alternate Function Push-Pull (USART1_TX) — CubeMX handles this
- PA10: Input (USART1_RX) — CubeMX handles this

### Clock
- 72 MHz system clock recommended for accurate 115200 baud

## USB-Serial Adapter Connection

Connect a 3.3 V USB-to-TTL adapter:

| Adapter | Blue Pill |
|---------|-----------|
| TX | PA10 (RX) |
| RX | PA9 (TX) |
| GND | GND |

Open a terminal at **115200 8N1** (e.g., PuTTY, TeraTerm, minicom).

## Build & Flash

1. Generate CubeMX code with USART1 configured as above.
2. Copy user code sections from `main_usercode.c` into generated `main.c`.
3. Build and flash via ST-Link.

## Expected Behavior

- Terminal prints `"Hello STM32\r\n"` once per second.
- Type any character in the terminal — the MCU echoes it back immediately.
- LED blinks at 1 Hz (toggled in the main loop).

## Notes

- Code is **not tested on hardware**.
- `HAL_UART_Receive` with a 1-second timeout will block the loop for up to 1 s
  if no byte arrives. This is intentional for a simple demo; use interrupts (project 08)
  for non-blocking receive.
- 3.3 V logic — see `wiring_notes.md`.
