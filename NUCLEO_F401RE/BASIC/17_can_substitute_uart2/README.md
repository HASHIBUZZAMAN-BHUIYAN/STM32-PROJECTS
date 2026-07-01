# Nucleo-F401RE BASIC/17 — Dual UART Bridge (CAN Substitute)

## CAN Bus Availability Note
**STM32F401 does NOT have CAN bus.** CAN is available on STM32F1 (Blue Pill), F3, F7, and H7 series, but NOT on F4 series (F401/F411). This project substitutes a dual-UART bridge demo that showcases two simultaneous UART peripherals — a genuinely useful pattern for protocol bridging/relay applications.

## Overview
USART1 (PA9/PA10) receives data from an external device (e.g. GPS module, another STM32, or a second USB-serial adapter). USART2 (PA2/PA3, ST-Link VCP) forwards it to the PC and vice versa. The MCU acts as a transparent bridge with optional local processing.

## CubeMX Configuration

| Peripheral | Setting | Pins |
|-----------|---------|------|
| USART1 | Async, 9600 8N1, RX interrupt | PA9 (TX), PA10 (RX) |
| USART2 | Async, 115200 8N1, RX interrupt | PA2 (TX) / PA3 (RX) — ST-Link VCP |
| GPIO PA5 | Output | LD2 — blinks on relayed data |

## Parts List
| Component | Qty | Notes |
|-----------|-----|-------|
| Nucleo-F401RE | 1 | |
| Second USB-serial adapter (3.3V) | 1 | Connect to USART1 as external device |
| Jumper wires | 3 | TX/RX/GND |

## Wiring (External Device to USART1)
| External Device | Nucleo |
|----------------|--------|
| TX | PA10 (USART1 RX) |
| RX | PA9 (USART1 TX) |
| GND | GND |

## Expected Behavior
- Data received on USART1 is forwarded to USART2 (PC terminal)
- Data typed in PC terminal (USART2) is forwarded to USART1 (external device)
- LD2 blinks on each forwarded byte
- Works as a USB↔UART bridge
