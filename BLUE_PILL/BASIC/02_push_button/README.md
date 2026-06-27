# 02 — Push Button Input (PA0) with Software Debounce

## Overview
Reads a momentary push button on **PA0** and toggles the onboard LED (PC13, active-low) on each confirmed press. Software debounce uses a 50 ms delay after detecting a falling edge to ignore contact bounce.

## Hardware
- Board: STM32F103C8T6 Blue Pill
- Button: Momentary push button on PA0
- LED: Onboard PC13 (active-low)

## CubeMX Configuration
| Peripheral | Setting |
|------------|---------|
| PA0 | GPIO_Input, Pull-up |
| PC13 | GPIO_Output, Push-Pull, No Pull, Speed: Low |
| SYS | Debug: Serial Wire |

With the internal pull-up enabled, the pin reads HIGH when the button is open and LOW when the button is pressed (button connects PA0 to GND).

## How It Works
The main loop polls PA0. When it detects a LOW level (button pressed), it waits 50 ms for contact bounce to settle, then re-reads the pin. If still LOW the press is confirmed and the LED is toggled. The code then waits for the pin to return HIGH (button released) before accepting the next press — preventing repeated triggers while held.

## Build & Flash
1. Create a new STM32CubeIDE project for STM32F103C8T6.
2. Apply CubeMX settings above and generate code.
3. Copy sections from `main_usercode.c` into `Core/Src/main.c`.
4. Build and flash via ST-Link.

## Notes
- Code is **not tested on hardware**. Review before flashing.
- If you use an external pull-up resistor (e.g., 10 kΩ to 3.3 V), set PA0 to No Pull in CubeMX instead.
