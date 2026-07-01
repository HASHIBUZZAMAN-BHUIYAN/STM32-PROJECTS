# Wiring Notes — Black Pill BASIC/04: EXTI Button

## No Extra Wiring Required
Uses onboard resources only:
- PA0 = user button (on WeAct Black Pill; some boards may label it KEY)
- PC13 = onboard LED (active-LOW)

## USART1 Debug Output
USART1 uses PA9 (TX) and PA10 (RX). The Black Pill has no built-in ST-Link, so connect a USB-UART adapter:

| Black Pill | USB-UART Adapter |
|-----------|----------------|
| PA9 (TX) | RX |
| PA10 (RX) | TX |
| GND | GND |

**Note**: Do NOT connect VCC from the USB-UART adapter to the Black Pill 3.3V — power the Black Pill via its USB-C connector separately.

## PA10 Pull-up (Optional)
If UART RX appears unreliable, add a 10kΩ resistor from PA10 to 3.3V.

## PC13 LED Logic
PC13 is active-LOW on WeAct Black Pill:
- `HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET)` → LED OFF
- `HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET)` → LED ON
- `HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13)` → toggles

## PA0 Button Logic
PA0 button: pressed = 3.3V (logic HIGH). Configure as Pull-down + Rising Edge trigger.

## Parts List
| Component | Qty | Notes |
|-----------|-----|-------|
| Black Pill (STM32F411CEU6) | 1 | |
| USB-UART adapter (CP2102, CH340) | 1 | For USART1 debug |
| USB-C cable | 1 | Power |
| Jumper wires | 2 | PA9→RX, GND→GND |
