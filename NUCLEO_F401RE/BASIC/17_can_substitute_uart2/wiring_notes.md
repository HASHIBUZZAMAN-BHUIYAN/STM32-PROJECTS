# Wiring Notes — Nucleo-F401RE BASIC/17: Dual UART Bridge

## ⚠️ 3.3V Logic Warning
USART1 pins (PA9/PA10) are 3.3V. Connect only 3.3V-level devices or use level shifters.

## Why No CAN?
STM32F401 and F411 do not include CAN controller hardware. CAN is available on:
- **Blue Pill** (STM32F103) — bxCAN
- STM32F3xx, F7xx, H7xx series

## USART2 (ST-Link VCP) — No Wiring
Plug USB into CN1. Appears as COM port at 115200 baud.

## USART1 (External Device)
| Signal | Nucleo Pin | External Device |
|--------|-----------|----------------|
| TX out | PA9 (Arduino D8) | RX of external |
| RX in | PA10 (Arduino D2) | TX of external |
| GND | GND | GND of external |

## Test Setup (without a real external device)
Use a second USB-serial adapter:
1. Connect adapter TX → PA10 (USART1 RX)
2. Connect adapter RX → PA9 (USART1 TX)
3. Open two terminal windows: one at 115200 (USART2/VCP), one at 9600 (adapter/USART1)
4. Text typed in one window appears in the other

## Parts List
| Component | Qty |
|-----------|-----|
| Nucleo-F401RE | 1 |
| 3.3V USB-serial adapter | 1 |
| USB cables | 2 |
| Jumper wires | 3 |
