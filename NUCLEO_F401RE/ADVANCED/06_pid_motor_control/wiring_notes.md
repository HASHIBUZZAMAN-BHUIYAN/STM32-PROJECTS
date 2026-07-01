# Wiring Notes — Nucleo-F401RE ADVANCED/06: PID Motor Control

## Pin Assignment
| Nucleo Pin | Signal | Connected To |
|-----------|--------|-------------|
| PA8 | TIM1_CH1 PWM | H-bridge IN1 (speed) |
| PA9 | GPIO Output | H-bridge IN2 (direction) |
| PB6 | TIM4_CH1 (Encoder A) | Motor encoder channel A |
| PB7 | TIM4_CH2 (Encoder B) | Motor encoder channel B |
| 3.3V | Logic VCC | H-bridge logic VCC (if 3.3V compatible) |
| GND | GND | H-bridge GND + encoder GND |
| External 6–12V | Motor VCC | H-bridge VMOT |

## Wiring Diagram
```
Nucleo PA8 ──── DRV8833 AIN1 (or L298N IN1)
Nucleo PA9 ──── DRV8833 AIN2 (or L298N IN2)
Nucleo GND ──── DRV8833 GND
Nucleo 3.3V ─── DRV8833 VCC (logic)
External 6V ─── DRV8833 VM (motor supply)
DRV8833 AOUT1 ─── Motor +
DRV8833 AOUT2 ─── Motor -

Motor Encoder:
  VCC (3.3V) ─── Encoder VCC
  GND ──────── Encoder GND
  Channel A ─── PB6
  Channel B ─── PB7
```

## H-Bridge Selection
| Module | Logic voltage | Motor supply | Notes |
|--------|-------------|------------|-------|
| DRV8833 | 3.3V OK | up to 10.8V | Recommended — 3.3V compatible |
| L298N | 5V input | up to 46V | Needs level shifter for 3.3V Nucleo |
| TB6612FNG | 3.3V OK | up to 13.5V | Another good option |

## Encoder Notes
- TIM4 encoder mode counts both edges of both channels (4x resolution)
- 12 CPR motor × 4 (encoder mode) × 100:1 gearbox = 4800 counts per output shaft rev
- Adjust `ENCODER_CPR` in code to match your motor
- Encoder VCC: 3.3V (most hall effect encoders work from 3.3–5V)

## Tuning PID
Start with Kp only (Ki=0, Kd=0), then add Ki for steady-state error, finally Kd for damping:
1. Set `pid 1.0 0.0 0.0`, `set 100`
2. Increase Kp until oscillation, then halve it
3. Add Ki (e.g., 0.1) until steady-state error = 0
4. Add Kd (e.g., 0.05) to reduce overshoot

## 3.3V Safety
- Encoder signals are 3.3V-safe inputs on PB6/PB7
- Motor back-EMF should be suppressed by H-bridge's flyback diodes
- Keep motor wiring away from signal wires to reduce EMI

## Parts List
| Component | Qty | Notes |
|-----------|-----|-------|
| Nucleo-F401RE | 1 | |
| DC motor with encoder | 1 | GA12-N20, JGA25-370, or similar |
| DRV8833 H-bridge module | 1 | 3.3V compatible |
| Motor power supply (6–9V) | 1 | Separate from Nucleo |
| Breadboard + jumpers | 1 set | |
| USB cable | 1 | |
