# Black Pill BASIC/07 — UART Basics

## Overview
Demonstrates basic UART communication using USART1 (PA9/PA10). Sends a startup message, then echoes any received characters with a prefix. Introduces the HAL UART transmit and receive polling APIs.

## MCU Facts
| Item | Value |
|------|-------|
| MCU | STM32F411CEU6 |
| UART | USART1, PA9=TX, PA10=RX |
| Baud | 115200, 8N1 |

## Why USART1 on Black Pill?
The Black Pill has no built-in ST-Link. USART1 (PA9/PA10) is the conventional debug UART — connect a USB-UART adapter to these pins to see serial output on a PC.

## CubeMX Configuration
| Item | Setting |
|------|---------|
| USART1 | Asynchronous, 115200 8N1 |
| PA9 | USART1_TX |
| PA10 | USART1_RX |

## Expected Behavior
1. On power-up: prints `Black Pill UART ready. Type something!`
2. Type any character in terminal → STM32 echoes `Echo: <char>`

## PA10 Pull-up Note
On some WeAct Black Pill boards, PA10 can be left floating before UART init. If the board spuriously "receives" data on startup, add a 10kΩ pull-up from PA10 to 3.3V.
