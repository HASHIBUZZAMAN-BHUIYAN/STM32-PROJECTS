# Wiring Notes — 01 Blink LED

## Voltage Warning
**All GPIO pins on the STM32F103C8T6 Blue Pill operate at 3.3 V logic.**
Do NOT connect 5 V signals directly to any GPIO pin. Doing so may permanently damage the MCU.

## Connections
No external wiring is required for this project. The onboard LED is already connected to PC13 on the Blue Pill PCB.

| Signal | Blue Pill Pin | Notes |
|--------|--------------|-------|
| Onboard LED | PC13 | Active-low, built-in series resistor on PCB |
| ST-Link SWDIO | PA13 | For programming/debugging |
| ST-Link SWCLK | PA14 | For programming/debugging |
| ST-Link GND | GND | Common ground |
| ST-Link 3.3V | 3.3V | Optional; can power board from ST-Link |

## Active-Low Explained
The Blue Pill routes PC13 through a resistor to the LED anode; the cathode connects to 3.3 V. This means:
- **Pin LOW (GPIO_PIN_RESET)** → current flows → **LED ON**
- **Pin HIGH (GPIO_PIN_SET)** → no current (same voltage on both sides) → **LED OFF**

This is the opposite of a typical active-high LED circuit. The code explicitly uses `HAL_GPIO_TogglePin` which handles both states transparently.

## Power Supply
Power the Blue Pill via USB (5 V → onboard LDO → 3.3 V) or directly through the 3.3 V pin from the ST-Link.

## Boot Jumpers
Ensure BOOT0 = 0 (jumper to GND) and BOOT1 = 0 to boot from Flash after programming.
