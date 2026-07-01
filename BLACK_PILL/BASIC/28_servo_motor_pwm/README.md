# Black Pill BASIC/28 — Servo Motor PWM Control

## Overview
Controls a standard RC servo motor using TIM2_CH1 (PA0). Standard servos use 50 Hz PWM with pulse width 1ms–2ms (1000–2000µs). The Black Pill's higher precision timer at 100 MHz gives 1µs resolution for smooth servo control. A UART command interface sets the angle (0°–180°).

## Servo PWM Specification
| Parameter | Value |
|-----------|-------|
| Frequency | 50 Hz (20ms period) |
| Pulse width 0° | 1000µs |
| Pulse width 90° | 1500µs |
| Pulse width 180° | 2000µs |

## TIM2 Configuration for 50 Hz
```
SYSCLK = 100 MHz, TIM2 APB1 timer clock = 100 MHz
PSC = 99 → 100 MHz / 100 = 1 MHz (1µs per tick)
ARR = 19999 → 1 MHz / 20000 = 50 Hz (20ms period)
CCR1: 1000 = 1ms pulse = 0°
CCR1: 1500 = 1.5ms pulse = 90°
CCR1: 2000 = 2ms pulse = 180°
```

## CubeMX Configuration
| Item | Setting |
|------|---------|
| TIM2_CH1 | PA0, PWM Mode 1 |
| TIM2 PSC | 99 |
| TIM2 ARR | 19999 |
| TIM2 CCR1 | 1500 (start at 90°) |
| USART1 | PA9/PA10, 115200 8N1, Global IT |

**PA0 Conflict**: TIM2_CH1 uses PA0, which is also the user button on WeAct Black Pill. For this project, the button cannot be used simultaneously with servo output. Use USART1 for angle control instead.

## UART Commands
- `angle 0` — set to 0°
- `angle 90` — set to 90°
- `angle 180` — set to 180°
- `sweep` — continuous 0°↔180° sweep

## Expected Behavior
Servo rotates to commanded angle within ~200ms.

## Safety
- Servo power: typically 4.8–6V (NOT 3.3V) — use external supply
- Servo control signal: 3.3V is accepted by most servos (signal is 5V tolerant on most servos)
- Ground must be shared between STM32 and servo supply
