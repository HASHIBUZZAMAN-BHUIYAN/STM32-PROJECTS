# Nucleo-F401RE BASIC/25 — Servo + DC Motor Combined PWM

## Overview
Controls a servo motor (50 Hz PWM, TIM1 CH1) and a DC motor via L298N H-bridge (TIM3 CH1 for speed, PB0/PB1 for direction) simultaneously. UART2 command interface sets servo angle and motor speed/direction.

## CubeMX Configuration

| Peripheral | Setting | Pins |
|-----------|---------|------|
| TIM1 CH1 | PWM, PSC=83, ARR=19999 → 50 Hz at 84 MHz | PA8 (Arduino D7) |
| TIM3 CH1 | PWM, PSC=83, ARR=999 → 1 kHz | PA6 (Arduino D12) |
| GPIO PB0 | Output Push-Pull | Direction 1 |
| GPIO PB1 | Output Push-Pull | Direction 2 |
| USART2 | 115200 8N1 | PA2/PA3 |

**Servo PWM**: 50 Hz, pulse 1000–2000 µs → 0°–180°. With ARR=19999: CCR=1000→1ms, CCR=2000→2ms.

## Parts List
| Component | Qty |
|-----------|-----|
| Nucleo-F401RE | 1 |
| SG90 or MG90S servo | 1 |
| L298N H-bridge module | 1 |
| DC motor (5-12V) | 1 |
| External 5V supply for servo | 1 |
| USB cable | 1 |

## UART Commands (115200)
- `S0` to `S180` — set servo angle
- `M0` to `M100` — set motor speed (%)
- `MF` / `MR` / `MS` — Forward / Reverse / Stop
