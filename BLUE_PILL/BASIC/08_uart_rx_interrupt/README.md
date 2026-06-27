# 08 — UART RX Interrupt (Line-Buffered Input)

## Overview

Receives bytes over USART1 using `HAL_UART_Receive_IT` (interrupt-driven, non-blocking).
Each received byte is appended to a line buffer. When a newline (`'\n'`) arrives,
the complete line is processed (echoed back with a prefix in this demo).

The key pattern:
- Arm once with `HAL_UART_Receive_IT(&huart1, &rx_byte, 1)`.
- In `HAL_UART_RxCpltCallback`: store byte, re-arm immediately.
- Main loop is free to do other work — it never blocks on UART.

## Hardware

| Signal | Pin | Direction |
|--------|-----|-----------|
| USART1 TX | PA9 | MCU → Host |
| USART1 RX | PA10 | Host → MCU |
| Onboard LED | PC13 (active-low) | Output |

## CubeMX Configuration

### USART1
- Mode: Asynchronous
- Baud Rate: 115200, 8N1, no flow control
- NVIC: **USART1 global interrupt — ENABLED** (this is the critical difference from project 07)

### NVIC
- USART1 global interrupt: Enabled, Priority 0 (or any non-zero priority below SysTick)

### Clock
- 72 MHz recommended

## Build & Flash

1. Generate CubeMX code with USART1 interrupt enabled.
2. Copy user code sections from `main_usercode.c` into the generated `main.c`.
3. Build and flash via ST-Link.
4. Connect a 3.3 V USB-to-TTL adapter (PA9→adapter RX, PA10→adapter TX).
5. Open terminal at 115200 8N1. Type a line and press Enter.

## Expected Behavior

- Main loop blinks LED at 1 Hz continuously — it never blocks.
- Type any text and press Enter in the terminal.
- MCU replies: `"ECHO: <your text>\r\n"`.
- Partially typed text (no newline yet) is silently buffered.

## Notes

- Code is **not tested on hardware**.
- `HAL_UART_RxCpltCallback` runs in ISR context — keep it short (no HAL_Delay, no printf).
- The line buffer is 64 bytes. Longer lines are silently truncated (index wraps).
- Re-arming with `HAL_UART_Receive_IT` inside the callback is the standard STM32 HAL pattern.
- 3.3 V logic — see `wiring_notes.md`.
