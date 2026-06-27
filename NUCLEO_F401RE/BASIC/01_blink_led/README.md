# BASIC/01 — Blink LED (LD2)

Blinks the onboard green LED (LD2, PA5) at 1 Hz (500 ms on, 500 ms off) using HAL_GPIO_TogglePin and HAL_Delay.

## CubeMX Configuration
- Board: NUCLEO-F401RE (select board, PA5 pre-assigned as GPIO_Output for LD2)
- PA5: GPIO_Output, Push-Pull, No Pull, High Speed, label LD2
- System Core → SYS → Debug: Serial Wire
- Clock: 84 MHz (HSI PLL, or HSE if crystal fitted)
- No additional peripherals needed

## Parts List
- Nucleo-F401RE board (USB cable for power and ST-Link)

## Wiring
No external wiring required. LD2 is onboard PA5.

NOTE: PA5 is also SPI1 SCK. Do NOT use SPI1 and this blink project simultaneously.

## Build and Flash
1. Open STM32CubeIDE, create project for NUCLEO-F401RE.
2. In CubeMX tab, PA5 is auto-configured as GPIO_Output (select board preset).
3. Generate code.
4. Copy code from main_usercode.c into the appropriate USER CODE sections in the generated main.c.
5. Build (Ctrl+B), flash via ST-Link (Run → Debug or Run).

## Expected Behavior
LD2 (green LED on the board) blinks at 1 Hz — 500 ms on, 500 ms off.
