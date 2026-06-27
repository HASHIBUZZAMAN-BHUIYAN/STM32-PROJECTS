# BASIC/08 — UART RX Interrupt (Line Accumulator)

Receives characters on UART2 via interrupt (1-byte at a time). Accumulates characters into a line buffer until '\n' is received, then processes the complete line and echoes it back with "Echo: [line]\r\n". Re-arms the receive interrupt in the callback.

## CubeMX Configuration
- Board: NUCLEO-F401RE
- Connectivity → USART2:
  - Mode: Asynchronous
  - Baud Rate: 115200, 8N1
  - PA2: USART2_TX, PA3: USART2_RX
- System Core → NVIC:
  - USART2 global interrupt: Enabled, Priority 1 / Sub-priority 0
- Clock: 84 MHz

## Parts List
- Nucleo-F401RE, USB cable, terminal emulator

## Wiring
No external wiring — uses ST-Link VCP.

## Build and Flash
1. Configure USART2 with interrupt in CubeMX.
2. Generate code, paste main_usercode.c sections.
3. Build, flash, open terminal at 115200 baud.

## Expected Behavior
Type text in terminal and press Enter. MCU echoes: "Echo: [your text]\r\n". Non-blocking — LED blinks in main loop while waiting for input.
