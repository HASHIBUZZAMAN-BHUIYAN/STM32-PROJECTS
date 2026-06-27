# BASIC/02 — Push Button (B1) with LED Toggle

Reads the onboard user button B1 (PC13, active-LOW) and toggles LD2 (PA5) on each press. Software debounce using HAL_GetTick() with a 50 ms debounce window.

## CubeMX Configuration
- Board: NUCLEO-F401RE
- PC13: GPIO_Input, Pull-up (board already has hardware pull-up; enabling internal pull-up is harmless)
  - Label: B1_Pin
- PA5: GPIO_Output (pre-assigned as LD2 when selecting board preset)
- System Core → SYS → Debug: Serial Wire
- Clock: 84 MHz

## Parts List
- Nucleo-F401RE board
- USB cable

## Wiring
No external wiring required. B1 (PC13) and LD2 (PA5) are both onboard.

## Build and Flash
1. Create CubeMX project for NUCLEO-F401RE.
2. Confirm PC13 is GPIO_Input with Pull-up, PA5 is GPIO_Output.
3. Generate code, paste main_usercode.c sections, build, flash.

## Expected Behavior
- Press B1: LD2 toggles state (on→off or off→on).
- Software debounce prevents multiple toggles per press.
- LED state persists until next button press.
