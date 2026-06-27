# BASIC/07 — UART Basics (USART2 ST-Link VCP)

Demonstrates UART2 (PA2 TX, PA3 RX) connected to the onboard ST-Link virtual COM port. Transmits "Hello Nucleo\r\n" every 1 second and echoes back any received characters. No external USB-serial adapter needed — the Nucleo's onboard ST-Link provides the VCP automatically via CN1 USB.

## CubeMX Configuration
- Board: NUCLEO-F401RE
- Connectivity → USART2:
  - Mode: Asynchronous
  - Baud Rate: 115200
  - Word Length: 8 bits
  - Parity: None
  - Stop Bits: 1
  - PA2: USART2_TX
  - PA3: USART2_RX
  - Hardware Flow Control: Disabled
- System Core → SYS → Debug: Serial Wire
- Clock: 84 MHz

## Parts List
- Nucleo-F401RE board
- USB cable (to PC)
- Terminal emulator (PuTTY, Tera Term, etc.)

## Wiring
None — ST-Link VCP is internal. Connect Nucleo USB (CN1) to PC.

## Build and Flash
1. CubeMX: configure USART2 as above.
2. Generate code, paste main_usercode.c, build, flash.
3. Open terminal at 115200 baud on ST-Link COM port.

## Expected Behavior
- Every 1 second: "Hello Nucleo\r\n" appears in terminal.
- Type any character in terminal → echoed back with prefix "Echo: X\r\n".
