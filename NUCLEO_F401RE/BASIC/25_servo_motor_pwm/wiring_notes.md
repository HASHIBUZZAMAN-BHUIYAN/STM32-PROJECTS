# Wiring Notes — Nucleo-F401RE BASIC/25: Servo + DC Motor

## ⚠️ 3.3V Logic Warning
All Nucleo GPIO pins are 3.3V. L298N and servos have 5V-compatible signal inputs.

## Servo (SG90/MG90S)
| Servo Wire | Connection |
|-----------|-----------|
| Brown/Black (GND) | GND |
| Red (VCC) | **5V** external supply (not from Nucleo 5V pin — servo draws too much current) |
| Orange/Yellow (Signal) | PA8 (TIM1 CH1) — 3.3V signal is acceptable |

## DC Motor via L298N
| L298N Pin | Connection |
|-----------|-----------|
| IN1 | PB0 |
| IN2 | PB1 |
| ENA | PA6 (TIM3 CH1 PWM) |
| 12V / Motor power | External 6-12V supply |
| GND | GND (shared with Nucleo) |
| 5V out | Can power Arduino-style devices (not servo — too much current) |

## Parts List
| Component | Qty |
|-----------|-----|
| Nucleo-F401RE | 1 |
| SG90 servo | 1 |
| L298N H-bridge | 1 |
| Small DC motor (3-12V) | 1 |
| 5V 1A supply (servo power) | 1 |
| Motor supply (6-12V) | 1 |
| Jumper wires | 10 |
