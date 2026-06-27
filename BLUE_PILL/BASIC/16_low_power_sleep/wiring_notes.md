# Wiring Notes — 16 Low Power Sleep

## Voltage Warning
**All GPIO pins on the Blue Pill operate at 3.3 V logic.**
Do NOT connect 5 V signals directly to any GPIO pin. Use a level shifter if interfacing with 5 V devices.

## Pin Connections

| Signal | Blue Pill Pin | Notes |
|--------|--------------|-------|
| Wake Button | PA0 | Connect button between PA0 and GND. Enable internal pull-up in CubeMX (or add 10 kΩ external pull-up to 3.3 V). EXTI0 configured for falling edge. |
| UART TX | PA9 | Connect to USB-UART adapter RX |
| UART RX | PA10 | Connect to USB-UART adapter TX |
| Onboard LED | PC13 | No external wiring needed — active-low, built-in |
| GND | GND | Common ground between Blue Pill and USB-UART adapter |
| 3.3 V | 3.3 V | Power rail reference |

## USB-UART Adapter
- Use a 3.3 V-compatible adapter (e.g., CP2102, CH340 in 3.3 V mode, or FTDI with 3.3 V jumper).
- Set host terminal to 115200 baud, 8N1, no flow control.

## Button Debounce
Hardware debounce is not implemented in this project. If spurious wakes occur, add a 100 nF capacitor between PA0 and GND in parallel with the button.

## EXTI0 Trigger Direction
CubeMX should configure EXTI0 as **falling edge** (button press pulls PA0 LOW). If using active-high logic, change to rising edge and adjust the pull resistor accordingly.

## SysTick Timebase Note
CubeMX defaults to SysTick as the HAL timebase. In sleep mode SysTick continues to fire every 1 ms, which wakes the MCU immediately. To avoid this, reassign the HAL timebase to TIM1 in CubeMX (System Core → SYS → Timebase Source → TIM1).

## Power Consumption (approximate, Blue Pill @ 3.3 V)
| State | Typical Current |
|-------|----------------|
| Active (72 MHz) | ~25 mA |
| Sleep (WFI, all peripherals on) | ~5–10 mA |
| Stop mode (deeper, separate project) | ~100 µA |
