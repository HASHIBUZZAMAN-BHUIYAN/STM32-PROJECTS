# Nucleo-F401RE BASIC/16 — Multiple Timer Channels Simultaneously

## Overview
Configures TIM2 with all four channels (CH1-CH4) as simultaneous PWM outputs at different frequencies by exploiting different ARR/PSC combinations on separate timers. Demonstrates the F401's richer timer set vs F103.

## CubeMX Configuration

| Peripheral | Setting | Pins |
|-----------|---------|------|
| TIM2 CH1 | PWM Generation, PSC=83999, ARR=999 → 1 Hz | PA0 (Arduino A0) |
| TIM2 CH2 | PWM Generation, same PSC, ARR=499 → 2 Hz | PA1 (Arduino A1) |
| TIM3 CH1 | PWM Generation, PSC=83999, ARR=249 → 4 Hz | PA6 (Arduino D12) |
| TIM3 CH2 | PWM Generation, same PSC, ARR=124 → 8 Hz | PA7 (Arduino D11) |
| USART2 | 115200 8N1 | PA2/PA3 |

**Note**: All 4 channels at 50% duty cycle produce visible LED blink rates.

## Parts List
| Component | Qty | Notes |
|-----------|-----|-------|
| Nucleo-F401RE | 1 | |
| 5mm LEDs | 4 | One per channel |
| 330 Ω resistors | 4 | Current limiting |

## Wiring
- LED 1 (1 Hz): PA0 → 330Ω → LED → GND
- LED 2 (2 Hz): PA1 → 330Ω → LED → GND
- LED 3 (4 Hz): PA6 → 330Ω → LED → GND
- LED 4 (8 Hz): PA7 → 330Ω → LED → GND

## Expected Behavior
Four LEDs blink at 1, 2, 4, and 8 Hz simultaneously. No CPU involvement — timers run in hardware.
