# Wiring Notes — 18 DC Motor H-Bridge (L298N)

## Voltage Warning
**All GPIO pins on the Blue Pill operate at 3.3 V logic.**
The L298N module's logic inputs (IN1, IN2, ENA) are 5 V tolerant, but the Blue Pill outputs 3.3 V. Most L298N modules recognize 3.3 V HIGH reliably; however, if the motor behaves erratically, add a 3.3 V → 5 V level shifter on IN1, IN2, and ENA.

## Pin Connections

| Signal | Blue Pill Pin | L298N Module Pin | Notes |
|--------|--------------|-----------------|-------|
| PWM (ENA) | PA8 (TIM1 CH1) | ENA | Remove the ENA jumper on the L298N module; connect PA8 directly |
| Direction IN1 | PB0 | IN1 | Motor direction bit 0 |
| Direction IN2 | PB1 | IN2 | Motor direction bit 1 |
| GND | GND | GND | Must be common ground |
| Motor Supply | — | VS (motor +) | 6–12 V external supply for motor power |
| Logic Supply | 5 V (USB) or 3.3 V | VSS (logic +) | L298N logic supply; check module jumper |

## Motor Connections

| Motor Wire | L298N Terminal |
|-----------|---------------|
| Motor + | OUT1 |
| Motor - | OUT2 |

## L298N Module Notes
- The onboard 5 V regulator on most L298N modules can provide a small 5 V output — do NOT use it as the Blue Pill's main supply; use it only for the module's own logic if needed.
- The ENA jumper (if present) must be **removed** and replaced with the PWM wire from PA8 to enable speed control. With the jumper in place ENA is permanently HIGH (full speed only).
- Keep motor power leads short and twisted to reduce EMI.
- Add a 100 nF ceramic capacitor across each motor terminal to suppress brush noise.

## Flyback / Protection
- The L298N module includes built-in flyback diodes for inductive motor loads. No external diodes are needed.
- If using a bare L298N IC (without module PCB), add 1N4007 diodes across each output.

## Heat
- The L298N IC dissipates significant heat driving motors above 1 A. Attach a heatsink if the chip becomes hot to the touch.

## PWM Frequency
- Code uses 1 kHz PWM (TIM1, PSC=71, ARR=999 at 72 MHz).
- DC brush motors generally work well between 1–20 kHz. Increase ARR or decrease PSC for higher frequencies if audible whine is a concern.

## Direction Truth Table
| PB0 (IN1) | PB1 (IN2) | Motor Action |
|-----------|-----------|-------------|
| HIGH | LOW | Forward |
| LOW | HIGH | Reverse |
| LOW | LOW | Coast (free-wheel) |
| HIGH | HIGH | Brake (short across motor) |
