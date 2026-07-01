# Nucleo-F401RE BASIC/13 — ST-Link Virtual COM Port Showcase

## Overview
Demonstrates the Nucleo's built-in ST-Link Virtual COM Port (VCP) — USART2 on PA2/PA3 routes through the onboard ST-Link chip to appear as a USB serial port on the PC with **zero extra hardware**. Sends structured JSON telemetry at 2 Hz. Contrasts with Blue Pill, which needs an external USB-serial adapter.

## CubeMX Configuration

| Peripheral | Setting | Pins |
|-----------|---------|------|
| USART2 | Async, 115200 8N1 | PA2 (TX), PA3 (RX) — pre-wired to ST-Link |
| TIM2 | Up counter, 84 MHz / (83999+1) / (999+1) = 1 Hz tick | Internal |
| ADC1 | IN0, single conversion | PA0 (Arduino A0) |
| GPIO PA5 | Output | LD2 LED |

**CubeMX tip**: Select board "NUCLEO-F401RE" to have PA2/PA3 pre-configured as USART2.

## Parts List
| Component | Qty | Notes |
|-----------|-----|-------|
| Nucleo-F401RE | 1 | All you need |
| USB cable | 1 | Plug into CN1 (ST-Link micro-USB/mini-USB) |
| Potentiometer 10 kΩ (optional) | 1 | Connect to PA0 for ADC reading |

## How It Works
The ST-Link chip on the Nucleo board enumerates as both a debugger/programmer AND a USB CDC virtual COM port simultaneously. The CDC port is directly wired to STM32 USART2. No driver needed on modern Windows/Linux/macOS.

Open any serial terminal at **115200 baud** on the ST-Link COM port.

## Expected Output (every 500ms)
```json
{"tick":1234,"led":1,"adc":2048,"volt_mv":1650}
{"tick":1735,"led":0,"adc":2091,"volt_mv":1684}
```
