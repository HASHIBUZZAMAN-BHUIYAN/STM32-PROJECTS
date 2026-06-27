# Wiring Notes — 03 PWM LED Fade

## Voltage Warning
**All GPIO pins on the STM32F103C8T6 Blue Pill operate at 3.3 V logic.**
Do NOT connect 5 V devices directly to GPIO pins. PA8 outputs a maximum of 3.3 V.

## Connections

| Signal | Blue Pill Pin | External Component | Notes |
|--------|--------------|-------------------|-------|
| PWM output | PA8 | LED anode via resistor | TIM1 CH1 |
| LED cathode | GND | — | Common ground |
| ST-Link SWDIO | PA13 | — | Programming/debug |
| ST-Link SWCLK | PA14 | — | Programming/debug |
| ST-Link GND | GND | — | Common ground |

## Current-Limiting Resistor
STM32 GPIO pins source a maximum of ~8 mA per pin (25 mA absolute max, but keep below 8 mA for reliability). A standard red LED (Vf ≈ 2.0 V) on a 3.3 V supply:

```
R = (Vcc - Vf) / I = (3.3 - 2.0) / 0.010 = 130 Ω
```

Use **150 Ω** (nearest standard value). For green/blue LEDs (Vf ≈ 3.0–3.2 V), use a smaller resistor (~10–47 Ω).

## Schematic

```
PA8 ──[150Ω]──[LED anode]──[LED cathode]── GND
```

## LED Polarity
The anode (longer leg) connects toward PA8 through the resistor. The cathode (shorter leg / flat side) connects to GND.

## Why PA8 / TIM1?
TIM1 is an advanced timer on APB2 (72 MHz). PA8 is the default alternate-function output for TIM1 CH1. No remap is needed.

## Do Not Connect
- Do NOT connect PA8 directly to a 5 V supply or 5 V logic.
- Do NOT omit the current-limiting resistor — exceeding pin current limits can damage the MCU.

## Boot Jumpers
BOOT0 = 0, BOOT1 = 0 to boot from Flash.
