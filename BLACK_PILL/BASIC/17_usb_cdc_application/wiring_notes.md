# Wiring Notes — Black Pill BASIC/17: USB CDC Application

## Key Advantage of Black Pill
The Black Pill's USB-C connector connects DIRECTLY to STM32 PA11 (D-) and PA12 (D+). Unlike the Nucleo where USB CDC requires an external connector, the Black Pill's built-in USB port works as a CDC device out of the box.

## No Extra Wiring Required
Just plug the USB-C cable into the Black Pill — the MCU appears as a USB CDC device.

## Pin Assignment (Internal)
| Black Pill Pin | Signal | Notes |
|---------------|--------|-------|
| PA11 | USB_OTG_FS_DM | Connected to USB-C D- |
| PA12 | USB_OTG_FS_DP | Connected to USB-C D+ |
| PA0 | ADC1_IN0 | Analog input (pot or leave floating) |
| PC13 | LED (active-LOW) | Blinks on USB TX |

## Optional Analog Test
Connect a 10kΩ pot to PA0 (wiper) with 3.3V/GND on the ends to vary the ADC reading.

## USB Clock Configuration Note
STM32F411 USB requires exactly 48 MHz for the USB peripheral. With 25 MHz HSE:
- A common valid setup: PLLM=25, PLLN=336, PLLP=4 → SYSCLK=84 MHz (not 100 MHz), PLLQ=7 → 48 MHz USB
- OR: PLLM=25, PLLN=200, PLLP=2 → SYSCLK=100 MHz, then PLLQ must give 48 MHz

CubeMX will warn if USB clock is not 48 MHz. Adjust PLLQ until the USB clock shows 48 MHz in the Clock Configuration tab.

## Windows CDC Driver
Windows 10/11: automatic — "USB Serial Device" appears in Device Manager under Ports.

## Terminal
Open any serial terminal (PuTTY, TeraTerm, Arduino Serial Monitor) at ANY baud rate (CDC ignores baud). The firmware streams ADC readings every 500ms.

## Parts List
| Component | Qty |
|-----------|-----|
| Black Pill (STM32F411CEU6) | 1 |
| USB-C cable | 1 |
| 10kΩ pot (optional) | 1 |
