# 03 — PWM LED Fade (TIM1 CH1, PA8)

## Overview
Fades an external LED in and out using PWM on **PA8** (TIM1 Channel 1). The duty cycle sweeps from 0 % to 100 % and back, creating a breathing effect. PWM frequency is ~1 kHz.

## Hardware
- Board: STM32F103C8T6 Blue Pill
- External LED + current-limiting resistor on PA8

## CubeMX Configuration
| Peripheral | Setting |
|------------|---------|
| TIM1 | Channel 1: PWM Generation CH1 |
| TIM1 PSC | 71 (gives 1 MHz timer clock from 72 MHz APB2) |
| TIM1 ARR | 999 (PWM period = 1000 ticks → 1 kHz) |
| TIM1 Pulse | 0 (start at 0 % duty) |
| PA8 | TIM1_CH1 (alternate function, auto-configured by CubeMX) |
| SYS | Debug: Serial Wire |

### PWM Frequency Calculation
```
Timer clock  = 72 MHz / (PSC + 1) = 72 MHz / 72 = 1 MHz
PWM period   = (ARR + 1) / timer_clock = 1000 / 1 MHz = 1 ms
PWM freq     = 1 / 1 ms = 1 kHz
Duty cycle % = CCR1 / (ARR + 1) * 100
```

## How It Works
After starting the PWM channel with `HAL_TIM_PWM_Start`, the main loop increments `TIM1->CCR1` from 0 to 999 (0–100 %) then decrements it back to 0. A 2 ms delay between steps produces a ~4-second full sweep.

## Build & Flash
1. Create a new STM32CubeIDE project for STM32F103C8T6.
2. Apply CubeMX settings above and generate code.
3. Copy sections from `main_usercode.c` into `Core/Src/main.c`.
4. Build and flash via ST-Link.

## Notes
- Code is **not tested on hardware**. Review before flashing.
- PA8 is a 3.3 V output. Use an appropriate current-limiting resistor (see wiring_notes.md).
- TIM1 is on APB2 (72 MHz). TIM2–TIM4 are on APB1 (36 MHz max, or 72 MHz with multiplier) — adjust PSC accordingly if porting.
