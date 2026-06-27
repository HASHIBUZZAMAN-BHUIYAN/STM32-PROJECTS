# Wiring Notes — BASIC/07 UART Basics

## Pin Diagram
```
Nucleo-F401RE
+--------------------------------------+
| PA2 (USART2_TX) ──► ST-Link ──► USB ──► PC COM port |
| PA3 (USART2_RX) ◄── ST-Link ◄── USB ◄── PC COM port |
+--------------------------------------+
```
All connections are internal to the Nucleo board. No external wiring needed.

## Parts List
| Item | Qty | Notes |
|------|-----|-------|
| Nucleo-F401RE | 1 | ST-Link VCP built-in |
| USB-A to Mini-B cable | 1 | CN1 connector on Nucleo |
| PC with terminal software | 1 | PuTTY, Tera Term, etc. |

## Safety Notes
⚠ ALL GPIO pins operate at 3.3V logic.
- USART2 TX/RX are internally connected to the ST-Link IC — no user-accessible wiring needed.
- If connecting USART2 to an external device (not ST-Link), that device must use 3.3V UART levels.
- DO NOT connect 5V TTL UART directly to PA2/PA3.

## Nucleo VCP Advantage
Unlike bare STM32 boards (e.g. Blue Pill), the Nucleo's onboard ST-Link automatically exposes USART2 as a USB virtual COM port. No FTDI/CH340 adapter required. This is one of the key Nucleo development advantages.

## Terminal Settings
- Baud: 115200 | Data: 8 bits | Parity: None | Stop: 1 | Flow: None
