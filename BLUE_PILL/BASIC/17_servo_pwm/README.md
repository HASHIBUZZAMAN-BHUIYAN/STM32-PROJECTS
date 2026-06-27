# 17 — Servo PWM (TIM2 CH2)

## Overview
Generates a 50 Hz PWM signal on TIM2 Channel 2 (PA1) to drive a standard RC servo motor. The duty cycle sweeps from 1000 µs (0°) through 1500 µs (90°) to 2000 µs (180°) and back, creating a continuous 0°→180°→0° sweep with 20 ms per step.

## Hardware
- Board: Blue Pill (STM32F103C8T6)
- Servo signal: PA1 (TIM2 CH2 PWM output)
- Servo power: external 5 V supply recommended (servo draws too much current for the onboard regulator)

## Timer Math
- System clock: 72 MHz
- Prescaler (PSC): 71 → Timer clock = 72 MHz / (71+1) = **1 MHz**
- Auto-Reload (ARR): 19999 → Period = (19999+1) / 1 MHz = **20 ms = 50 Hz**
- Pulse (CCR2) range: 1000 (1 ms = 0°) to 2000 (2 ms = 180°)

## CubeMX Configuration
| Field | Value |
|-------|-------|
| Timer | TIM2 |
| Channel 2 | PWM Generation CH2 |
| Prescaler | 71 |
| Counter Period (ARR) | 19999 |
| Pulse (CCR2) | 1500 (centre start) |
| PWM Mode | PWM Mode 1 |
| PA1 | TIM2_CH2 (alternate function) |

## Behavior
1. Starts TIM2 PWM on CH2 (`HAL_TIM_PWM_Start`).
2. Sweeps CCR2 from 1000 → 2000 in steps of 10 µs, 20 ms per step.
3. Sweeps back 2000 → 1000.
4. Repeats indefinitely.

## Files
| File | Purpose |
|------|---------|
| `main_usercode.c` | User code sections to paste into CubeMX-generated `main.c` |
| `wiring_notes.md` | Pin connections and safety notes |

## Disclaimer
Code is provided as a reference starting point and has not been tested on hardware.
