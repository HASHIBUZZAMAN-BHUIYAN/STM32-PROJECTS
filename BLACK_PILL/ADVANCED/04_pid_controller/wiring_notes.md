# Wiring Notes — Black Pill ADVANCED/04: PID Controller

## No Extra Hardware Required
Simulated plant — only a USB-UART adapter is needed.

## USART1
| Black Pill | USB-UART |
|-----------|---------|
| PA9 (TX) | RX |
| PA10 (RX) | TX |
| GND | GND |

## CubeMX TIM3 Setup
| Parameter | Value |
|-----------|-------|
| Clock | APB1 = 100 MHz (with x2 APB1 multiplier) |
| PSC | 999 |
| ARR | 999 |
| Resulting frequency | 100 Hz |
| NVIC | TIM3 global interrupt enable |

## Porting to Real Motor
To drive a real DC motor + encoder, replace the `PID_Step()` plant simulation
with encoder read → RPM → error, and feed `u_out` to a PWM TIM channel.
ADVANCED/06 on the Nucleo shows the full motor version.

## Parts List
| Component | Qty |
|-----------|-----|
| Black Pill (STM32F411CEU6) | 1 |
| USB-UART adapter | 1 |
| Jumper wires | 3 |
