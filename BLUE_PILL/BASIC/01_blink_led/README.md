# 01 — Blink LED (PC13, Active-Low)

## Overview
Blinks the onboard LED on the STM32F103C8T6 Blue Pill. The LED is connected to **PC13** and is **active-low**: writing GPIO_PIN_RESET turns the LED ON, GPIO_PIN_SET turns it OFF.

## Hardware
- Board: STM32F103C8T6 Blue Pill
- LED: Onboard PC13 (active-low, no external components needed)

## CubeMX Configuration
| Peripheral | Setting |
|------------|---------|
| PC13 | GPIO_Output, Push-Pull, No Pull, Speed: Low |
| SYS | Debug: Serial Wire |
| RCC | HSE: Crystal/Ceramic Resonator (optional — HSI works) |

No timers, no UART, no peripherals beyond the GPIO.

## How It Works
`HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13)` flips the pin state every 500 ms via `HAL_Delay(500)`. Because the LED is active-low, the pin idles HIGH (LED off) and goes LOW (LED on) on each toggle.

The initial pin state after CubeMX reset is HIGH (LED off), so the first toggle turns it ON.

## Build & Flash
1. Open STM32CubeIDE, create a new project for STM32F103C8T6.
2. Apply CubeMX settings above and generate code.
3. Copy the sections from `main_usercode.c` into `Core/Src/main.c` inside the matching `USER CODE BEGIN / END` blocks.
4. Build (Ctrl+B) and flash via ST-Link.

## Notes
- Code in this repository is **not tested on hardware**. Review before flashing.
- Blink rate: 500 ms ON / 500 ms OFF (1 Hz).
