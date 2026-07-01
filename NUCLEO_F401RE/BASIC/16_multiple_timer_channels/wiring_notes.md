# Wiring Notes — Nucleo-F401RE BASIC/16: Multiple Timer Channels

## ⚠️ 3.3V Logic Warning
All timer outputs are 3.3V. LEDs need current-limiting resistors.

## LED Wiring (4 channels)
| Channel | Nucleo Pin | Arduino | Freq | Connection |
|---------|-----------|---------|------|-----------|
| TIM2 CH1 | PA0 | A0 | 1 Hz | PA0 → 330Ω → LED → GND |
| TIM2 CH2 | PA1 | A1 | 2 Hz | PA1 → 330Ω → LED → GND |
| TIM3 CH1 | PA6 | D12 | 4 Hz | PA6 → 330Ω → LED → GND |
| TIM3 CH2 | PA7 | D11 | 8 Hz | PA7 → 330Ω → LED → GND |

## Parts List
| Component | Qty |
|-----------|-----|
| Nucleo-F401RE | 1 |
| 5mm LED (any color) | 4 |
| 330 Ω resistors | 4 |
| Breadboard + jumpers | 1 set |

## CubeMX Timer Settings
TIM2: PSC = 83999, Clock = 84 MHz / 84000 = 1 kHz
- CH1: ARR = 999 → 1 kHz / 1000 = **1 Hz**
- CH2: ARR = 499 → 1 kHz / 500 = **2 Hz**

TIM3: PSC = 83999, Clock = 1 kHz  
- CH1: ARR = 249 → **4 Hz**
- CH2: ARR = 124 → **8 Hz**

All 4 timers run completely in hardware. Zero CPU cycles consumed after Start.
