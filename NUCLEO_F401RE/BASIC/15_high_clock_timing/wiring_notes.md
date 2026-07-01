# Wiring Notes — Nucleo-F401RE BASIC/15: 84 MHz Timing Demo

## ⚠️ 3.3V Logic Warning
PB0 output is 3.3V. Logic analyzer input must be 3.3V compatible.

## Connections
| Signal | Nucleo Pin | Notes |
|--------|-----------|-------|
| 1 MHz output | PB0 (Arduino A3) | Connect to scope/logic analyzer |
| GND | GND | Reference ground for measurement |

## Measurement Equipment
- **Oscilloscope**: Set to 2 µs/div, should show square wave with ~1 µs period
- **Logic analyzer**: Set sample rate ≥ 10 MHz to capture 1 MHz

## Tuning
The `HALF_CYCLE_NOPS` constant may need adjustment because:
- HAL_GPIO_WritePin takes a variable number of cycles
- Compiler optimization affects NOP loop elimination (use `-O1` or `-O2`)

Measure actual frequency on scope and adjust the constant:
- Too slow → decrease HALF_CYCLE_NOPS
- Too fast → increase HALF_CYCLE_NOPS (can't go faster than 42 MHz GPIO toggle limit)

## Comparison Table
| Board | SYSCLK | Expected freq with same loop |
|-------|--------|------------------------------|
| Blue Pill (F103) | 72 MHz | ~857 kHz |
| Nucleo-F401RE | 84 MHz | ~1.0 MHz |
| Black Pill (F411) | 100 MHz | ~1.19 MHz |
