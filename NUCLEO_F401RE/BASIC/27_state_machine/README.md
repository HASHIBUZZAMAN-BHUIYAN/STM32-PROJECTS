# Nucleo-F401RE BASIC/27 — State Machine (Traffic Light)

## Overview
Implements a traffic light controller as a clean C state machine (enum + switch). States: RED → RED_YELLOW → GREEN → YELLOW → RED. Three GPIO outputs control LEDs. B1 button triggers an EMERGENCY state (all red, fast blink). Demonstrates a good embedded software pattern.

## CubeMX Configuration

| Peripheral | Setting | Pins |
|-----------|---------|------|
| GPIO PB5 | Output Push-Pull | Red LED |
| GPIO PB4 | Output Push-Pull | Yellow LED |
| GPIO PB3 | Output Push-Pull | Green LED |
| GPIO PC13 | EXTI13, falling edge, Pull-up | B1 emergency button |
| USART2 | 115200 8N1 | PA2/PA3 |

## Parts List
| Component | Qty |
|-----------|-----|
| Nucleo-F401RE | 1 |
| 5mm LEDs: red, yellow, green | 1 each |
| 330 Ω resistors | 3 |

## Expected Behavior
Normal cycle: RED(3s) → RED_YELLOW(1s) → GREEN(3s) → YELLOW(1s) → repeat.
B1 press: EMERGENCY — all LEDs blink red at 200ms for 5s, then resume normal.
