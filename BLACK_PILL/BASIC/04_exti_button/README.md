# Black Pill BASIC/04 — EXTI Button Interrupt

## Overview
Configures the onboard user button (PA0 on WeAct Black Pill) as an EXTI interrupt source. Pressing the button triggers an ISR which toggles the PC13 LED and increments a counter. Demonstrates interrupt-driven GPIO vs polling.

## MCU Facts
| Item | Value |
|------|-------|
| MCU | STM32F411CEU6 |
| Clock | 100 MHz |
| LED | PC13, active-LOW (SET = off, RESET = on) |
| Button | PA0, pull-down, triggers on rising edge |

## CubeMX Configuration
| Pin | Mode | Setting |
|-----|------|---------|
| PC13 | GPIO_Output | Initial state HIGH (LED off) |
| PA0 | GPIO_EXTI0 | Rising edge trigger, Pull-down |
| NVIC | EXTI0 | Enable, priority 1 |
| USART1 | PA9/PA10 | 115200 8N1, Async |

## Expected Behavior
- Each button press: PC13 LED toggles, counter increments
- USART1 prints: `Button pressed! count=3`
- LED stays in toggled state until next press

## Black Pill vs Nucleo Differences
| Feature | Nucleo F401RE | Black Pill F411 |
|---------|--------------|----------------|
| LED | PA5 active-HIGH | PC13 active-LOW |
| Button | PC13 active-LOW | PA0 active-HIGH |
| Debug UART | USART2 PA2/PA3 (ST-Link) | USART1 PA9/PA10 (USB-UART adapter) |
| SPI1 | Conflicts with LED | FREE (no conflict) |
| Max clock | 84 MHz | 100 MHz |

## PA10 Warning (WeAct Board)
PA10 is USART1_RX. On some WeAct Black Pill boards, PA10 can float to a wrong level. If UART RX is unreliable, add a 10kΩ pull-up to 3.3V on PA10.
