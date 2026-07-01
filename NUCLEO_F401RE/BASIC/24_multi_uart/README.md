# Nucleo-F401RE BASIC/24 — Multi-UART Simultaneous

## Overview
Runs USART1 (PA9/PA10) and USART2 (PA2/PA3, ST-Link VCP) simultaneously with independent RX interrupt handlers. USART2 receives PC commands; USART1 talks to an external device. Shows NVIC managing two UART interrupt sources.

## CubeMX Configuration

| Peripheral | Setting | Pins |
|-----------|---------|------|
| USART1 | Async, 9600 8N1, RX interrupt enabled | PA9 (TX), PA10 (RX) |
| USART2 | Async, 115200 8N1, RX interrupt enabled | PA2 (TX), PA3 (RX) |

**NVIC**: Both USART1 and USART2 global interrupts enabled. USART2 priority 1, USART1 priority 2.

## Parts List
| Component | Qty |
|-----------|-----|
| Nucleo-F401RE | 1 |
| USB-serial adapter (3.3V) | 1 |
| Jumper wires | 3 |

## Expected Behavior
- USART2 (VCP): type "send:hello" → forwarded to USART1 TX; type "status" → prints stats
- USART1: any received byte is prefixed "U1:" and printed to USART2
- Both handlers run independently via NVIC
