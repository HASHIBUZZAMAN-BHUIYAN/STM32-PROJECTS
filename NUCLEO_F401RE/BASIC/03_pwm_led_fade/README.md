# BASIC/03 — PWM LED Fade (TIM1 CH1, PA8)

Generates a PWM signal on PA8 (TIM1 Channel 1) to fade an external LED from off to full brightness and back, creating a breathing effect.

## CubeMX Configuration
- Board: NUCLEO-F401RE
- Timers → TIM1:
  - Channel 1: PWM Generation CH1
  - Prescaler (PSC): 83  (84 MHz / (83+1) = 1 MHz timer clock)
  - Counter Period (ARR): 999  (1 MHz / (999+1) = 1 kHz PWM frequency)
  - Pulse (CCR1): 0 (start at 0% duty)
  - PWM Mode: PWM Mode 1
  - Fast mode: Disabled
- PA8: TIM1_CH1 (auto-assigned)
- System Core → SYS → Debug: Serial Wire
- Clock: 84 MHz

## Parts List
| Item | Qty | Notes |
|------|-----|-------|
| Nucleo-F401RE | 1 | |
| LED (any color) | 1 | External, on PA8 |
| Resistor 330Ω | 1 | Current limiting |
| Breadboard + jumper wires | 1 set | |

## Wiring
- PA8 (Arduino D7 header pin) → 330Ω resistor → LED anode
- LED cathode → GND

## Build and Flash
1. Create CubeMX project, configure TIM1 CH1 as above.
2. Generate code, paste main_usercode.c sections.
3. Build and flash.

## Expected Behavior
External LED on PA8 smoothly fades from off to full brightness to off (breathing effect) on a ~2-second cycle.
