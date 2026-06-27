# 18 — DC Motor H-Bridge (L298N)

## Overview
Controls a DC motor via an L298N H-bridge module. TIM1 Channel 1 (PA8) provides a PWM signal for speed control. PB0 and PB1 are GPIO outputs for direction control (IN1/IN2 on the L298N). The motor cycles through: Forward → Stop → Reverse → Stop, repeating indefinitely.

## Hardware
- Board: Blue Pill (STM32F103C8T6)
- H-Bridge: L298N module
- Motor PWM: PA8 (TIM1 CH1)
- Direction pins: PB0 (IN1), PB1 (IN2)
- Motor power: external 6–12 V supply via L298N motor power input

## CubeMX Configuration
| Field | Value |
|-------|-------|
| Timer | TIM1 |
| Channel 1 | PWM Generation CH1 |
| Prescaler | 71 |
| Counter Period (ARR) | 999 |
| Pulse (CCR1) | 700 (70% duty = ~70% speed) |
| PWM Mode | PWM Mode 1 |
| PA8 | TIM1_CH1 (alternate function push-pull) |
| PB0 | GPIO Output Push-Pull, no pull |
| PB1 | GPIO Output Push-Pull, no pull |

**Timer math:**
- PSC=71 → timer clock = 72 MHz / 72 = 1 MHz
- ARR=999 → period = 1000 / 1 MHz = 1 ms → **1 kHz PWM** (suitable for DC motors)

## L298N Logic Table
| PB0 (IN1) | PB1 (IN2) | ENA (PWM) | Motor Action |
|-----------|-----------|-----------|-------------|
| HIGH | LOW | PWM | Forward |
| LOW | HIGH | PWM | Reverse |
| LOW | LOW | any | Coast (free spin) |
| HIGH | HIGH | any | Brake |

## Behavior
1. Forward at 70% speed for 2 seconds.
2. Brake (coast) for 1 second.
3. Reverse at 70% speed for 2 seconds.
4. Brake for 1 second.
5. Repeat.

## Files
| File | Purpose |
|------|---------|
| `main_usercode.c` | User code sections to paste into CubeMX-generated `main.c` |
| `wiring_notes.md` | Pin connections and safety notes |

## Disclaimer
Code is provided as a reference starting point and has not been tested on hardware.
