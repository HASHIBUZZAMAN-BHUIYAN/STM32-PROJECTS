# Wiring Notes — BASIC/08 UART RX Interrupt

## Pin Diagram
```
Nucleo-F401RE
+--------------------------------------+
| PA2 (USART2_TX) ──► ST-Link ──► USB ──► PC |
| PA3 (USART2_RX) ◄── ST-Link ◄── USB ◄── PC |
| PA5 (LD2)       ──► onboard LED (blink)    |
+--------------------------------------+
```
No external wiring required.

## Parts List
| Item | Qty | Notes |
|------|-----|-------|
| Nucleo-F401RE | 1 | ST-Link VCP built-in |
| USB cable | 1 | CN1 connector |
| PC with terminal | 1 | 115200 8N1 |

## Safety Notes
⚠ ALL GPIO pins operate at 3.3V logic.
- External UART devices must use 3.3V logic levels on PA2/PA3.
- The interrupt callback re-arms immediately after each byte — ensure no race condition with HAL in your final application by using a proper event queue for production code.

## Implementation Notes
- HAL_UART_Receive_IT handles 1 byte at a time and fires HAL_UART_RxCpltCallback.
- Always re-arm HAL_UART_Receive_IT inside the callback, otherwise further reception stops.
- lineReady flag avoids string copying in ISR — main loop handles the echo.
