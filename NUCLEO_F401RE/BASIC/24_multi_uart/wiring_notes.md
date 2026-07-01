# Wiring Notes — Nucleo-F401RE BASIC/24: Multi-UART

## ⚠️ 3.3V Logic Warning
USART1 pins (PA9/PA10) are 3.3V. Use 3.3V-level USB-serial adapter.

## USART2 (ST-Link VCP) — No Wiring
Open at 115200 baud via CN1 USB.

## USART1 (External Device)
| Signal | Nucleo | External |
|--------|--------|---------|
| TX | PA9 (D8) | RX of device |
| RX | PA10 (D2) | TX of device |
| GND | GND | GND |

## Parts List
| Component | Qty |
|-----------|-----|
| Nucleo-F401RE | 1 |
| 3.3V USB-serial adapter (CP2102/CH340) | 1 |
| USB cables | 2 |
| Jumper wires | 3 |

## Test Commands (type in USART2 terminal at 115200)
- `send:hello` → sends "hello" to USART1
- `status` → prints received byte counts
