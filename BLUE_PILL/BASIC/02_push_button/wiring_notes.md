# Wiring Notes — 02 Push Button

## Voltage Warning
**All GPIO pins on the STM32F103C8T6 Blue Pill operate at 3.3 V logic.**
Do NOT connect 5 V signals directly to any GPIO pin. Doing so may permanently damage the MCU.

## Connections

| Signal | Blue Pill Pin | External Component | Notes |
|--------|--------------|-------------------|-------|
| Button leg A | PA0 | Momentary push button | — |
| Button leg B | GND | — | Other side of button to GND |
| Onboard LED | PC13 | — | Active-low, no external parts needed |
| ST-Link SWDIO | PA13 | — | Programming/debug |
| ST-Link SWCLK | PA14 | — | Programming/debug |
| ST-Link GND | GND | — | Common ground |

## Pull-up Options

### Option A — Internal Pull-up (recommended for quick testing)
- Set PA0 to Pull-up in CubeMX.
- No external resistor needed.
- Pin reads HIGH (3.3 V) when button open, LOW (0 V) when button pressed.

### Option B — External Pull-up
- Connect a 10 kΩ resistor between PA0 and 3.3 V.
- Set PA0 to No Pull in CubeMX.
- Useful when the button wire is long (more noise-immune).

## Schematic (Option A)

```
3.3V ──[internal 40kΩ pull-up]──┬── PA0
                                 │
                              [BTN]
                                 │
                                GND
```

When the button is open: PA0 = 3.3 V (HIGH).
When the button is pressed: PA0 = 0 V (LOW) — button shorts pin to GND.

## Debounce Note
Mechanical buttons produce multiple rapid transitions (bounce) on press/release. The firmware enforces a 50 ms settling delay. An external 100 nF capacitor across the button terminals can also reduce bounce at the hardware level, but is optional here.

## Boot Jumpers
BOOT0 = 0, BOOT1 = 0 to boot from Flash.
