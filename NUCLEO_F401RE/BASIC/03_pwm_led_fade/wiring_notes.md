# Wiring Notes — BASIC/03 PWM LED Fade

## Pin Diagram
```
Nucleo-F401RE
   PA8 (D7)
     │
    [330Ω]
     │
    [LED+]
    [LED-]
     │
    GND
```

## Parts List
| Item | Qty | Notes |
|------|-----|-------|
| Nucleo-F401RE | 1 | |
| LED (any color) | 1 | ~2V forward voltage typical |
| 330Ω resistor | 1 | Limits current to ~4 mA at 3.3V |
| Breadboard | 1 | |
| Jumper wires | 2 | |

## Safety Notes
⚠ ALL GPIO pins operate at 3.3V logic. Do NOT apply 5V to any GPIO pin.
- PA8 maximum source current: 25 mA. Use a current-limiting resistor (≥100Ω, 330Ω recommended).
- 3.3V - 2.0V (LED Vf) / 330Ω ≈ 4 mA — well within safe limits.
- Do NOT connect the LED directly to PA8 without a resistor.

## Notes
- PA8 is the Arduino D7 header on the Nucleo-F401RE.
- TIM1 is an advanced timer; ensure TIM1 and its clock are properly enabled in CubeMX.
- The 1 kHz PWM frequency is imperceptible to the human eye (flicker-free).
