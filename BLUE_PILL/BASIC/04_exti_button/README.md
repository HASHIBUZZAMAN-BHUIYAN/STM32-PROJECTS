# 04 — External Interrupt Button (PA0, EXTI0)

## Overview
Detects button presses on **PA0** using the external interrupt (EXTI) subsystem. The ISR sets a flag; the main loop checks the flag and toggles the onboard LED (PC13, active-low). A `HAL_GetTick()`-based timestamp prevents rapid re-triggers caused by contact bounce (debounce window: 50 ms).

## Hardware
- Board: STM32F103C8T6 Blue Pill
- Button: Momentary push button on PA0 (other side to GND)
- LED: Onboard PC13 (active-low)

## CubeMX Configuration
| Peripheral | Setting |
|------------|---------|
| PA0 | GPIO_EXTI0, Pull-up, Falling Edge trigger |
| PC13 | GPIO_Output, Push-Pull, No Pull, Speed: Low |
| NVIC | EXTI line0 interrupt: Enabled, Priority 0/0 |
| SYS | Debug: Serial Wire |

**Falling edge** because the pin is pulled HIGH by the internal pull-up and goes LOW when the button connects PA0 to GND.

## How It Works
1. User presses button → PA0 goes LOW → EXTI0 fires.
2. ISR (`HAL_GPIO_EXTI_Callback`) records `HAL_GetTick()` and sets `buttonFlag`.
3. Main loop checks `buttonFlag`. If the time since last accepted press exceeds 50 ms, the LED is toggled and the tick timestamp is updated.
4. `buttonFlag` is cleared regardless, preventing stale flags.

## Build & Flash
1. Create a new STM32CubeIDE project for STM32F103C8T6.
2. Apply CubeMX settings above and generate code.
3. Copy sections from `main_usercode.c` into `Core/Src/main.c`.
4. Build and flash via ST-Link.

## Notes
- Code is **not tested on hardware**. Review before flashing.
- The `volatile` qualifier on `buttonFlag` is essential — it prevents the compiler from optimising away the ISR-written value.
- EXTI0 is shared with any other EXTI0 source on the device; the callback checks `GPIO_Pin` to filter correctly.
