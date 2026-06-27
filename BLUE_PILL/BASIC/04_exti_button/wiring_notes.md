# Wiring Notes — 04 EXTI Button

## Voltage Warning
**All GPIO pins on the STM32F103C8T6 Blue Pill operate at 3.3 V logic.**
Do NOT connect 5 V signals to any GPIO pin. Doing so may permanently damage the MCU.

## Connections

| Signal | Blue Pill Pin | External Component | Notes |
|--------|--------------|-------------------|-------|
| Button leg A | PA0 | Momentary push button | EXTI0 source |
| Button leg B | GND | — | Completes circuit on press |
| Onboard LED | PC13 | — | Active-low, built-in |
| ST-Link SWDIO | PA13 | — | Programming/debug |
| ST-Link SWCLK | PA14 | — | Programming/debug |
| ST-Link GND | GND | — | Common ground |

## Pull-up
The internal pull-up on PA0 is enabled in CubeMX. When the button is open, PA0 = 3.3 V (HIGH). When the button is pressed (shorts PA0 to GND), PA0 = 0 V (LOW) → falling edge → EXTI fires.

An optional external 10 kΩ resistor from PA0 to 3.3 V can be added for noise immunity on long wires, but is not required for bench testing.

## Schematic

```
3.3V ──[internal ~40kΩ pull-up]──┬── PA0 (EXTI0, falling edge)
                                  │
                               [BTN]
                                  │
                                 GND
```

## Optional Hardware Debounce
A 100 nF capacitor across the button terminals (between PA0 and GND) attenuates bounce at the hardware level and complements the software debounce window. It is not required for the firmware to work correctly.

```
PA0 ──┬──[100nF]── GND
      └── [BTN] ── GND
```

## EXTI Priority
The NVIC priority for EXTI0 is set to 0/0 (highest) by default in CubeMX. If you add other peripherals with higher-priority ISRs (e.g., UART, DMA), adjust NVIC priorities accordingly in CubeMX.

## Boot Jumpers
BOOT0 = 0, BOOT1 = 0 to boot from Flash.
