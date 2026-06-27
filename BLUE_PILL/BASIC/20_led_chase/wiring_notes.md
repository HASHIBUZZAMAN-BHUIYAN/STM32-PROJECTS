# Wiring Notes — 20 LED Chase (Knight Rider)

## Voltage Warning
**All GPIO pins on the Blue Pill operate at 3.3 V logic.**
Do NOT connect 5 V signals to any GPIO pin. The LEDs are driven directly from 3.3 V GPIO outputs — ensure current limiting resistors are fitted on every LED.

## Current Budget
Each STM32F103C8T6 GPIO pin can source/sink up to 25 mA, with a recommended maximum of 8 mA for typical operation. The total GPIO port current limit is ~80 mA. With 8 LEDs, only one is on at a time (chase pattern) so the maximum instantaneous current is one LED's current.

| Parameter | Value |
|-----------|-------|
| GPIO output voltage | ~3.1 V (loaded) |
| LED forward voltage (red) | ~2.0 V |
| Voltage across resistor | ~1.1 V |
| Recommended resistor | 330 Ω → ~3.3 mA (safe, visible brightness) |
| Max resistor (dim) | 1 kΩ → ~1.1 mA |
| Min resistor (bright) | 150 Ω → ~7.3 mA (within limit) |

Use **330 Ω** resistors for a balanced, safe brightness.

## LED Wiring (each of 8 LEDs)

```
GPIOB Pin (PBx) ──── [330 Ω] ──── LED Anode (+) ──── LED Cathode (-) ──── GND
```

## Pin to LED Mapping

| LED Position | Blue Pill Pin | Notes |
|-------------|--------------|-------|
| LED 1 (leftmost) | PB8 | |
| LED 2 | PB9 | |
| LED 3 | PB10 | |
| LED 4 | PB11 | |
| LED 5 | PB12 | |
| LED 6 | PB13 | |
| LED 7 | PB14 | |
| LED 8 (rightmost) | PB15 | |

## Physical Layout Suggestion
Mount the 8 LEDs in a row on a breadboard, left to right, PB8 → PB15. Tie all cathodes to a GND rail. Insert a 330 Ω resistor in series with each anode leg.

## Shared GND
Ensure the LED GND rail connects to the Blue Pill GND pin. If powering the Blue Pill from USB, the USB GND is the same as the board GND.

## Onboard LED (PC13)
The Blue Pill's onboard LED on PC13 is NOT used in this project. It remains in its default state (off, since PC13 defaults to input). CubeMX will configure it only if you explicitly add it.

## Chase Speed
Adjust `CHASE_DELAY_MS` in `main_usercode.c` to change the scan speed:
- 30 ms → fast, energetic sweep
- 60 ms → classic Knight Rider pace
- 120 ms → slow, deliberate sweep
