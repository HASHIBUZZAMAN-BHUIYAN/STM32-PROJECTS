# Nucleo-F401RE BASIC/14 — Onboard User Button Multi-Mode

## Overview
Uses the Nucleo's B1 user button (PC13, active-LOW) to implement three interaction modes detected via EXTI interrupt: single press = toggle LD2, double press within 300ms = fast blink mode, long press >1s = LED off. No external hardware needed.

## CubeMX Configuration

| Peripheral | Setting | Pins |
|-----------|---------|------|
| GPIO PC13 | EXTI13, falling edge, Pull-up | B1 user button (pre-wired on Nucleo) |
| GPIO PA5 | Output Push-Pull | LD2 LED |
| USART2 | Async, 115200 8N1 | PA2/PA3 — ST-Link VCP |

**NVIC**: EXTI15_10 IRQ — enable, priority 2.

## Parts List
| Component | Qty |
|-----------|-----|
| Nucleo-F401RE | 1 |
| USB cable | 1 |

No extra components — everything is onboard.

## Expected Behavior
- **Single press**: LD2 toggles ON/OFF; UART prints "Single press → LED toggled"
- **Double press** (two presses < 300ms apart): Fast blink mode (100ms); UART prints "Double press → Fast blink"
- **Long press** (hold > 1s): LED off; UART prints "Long press → LED off"
