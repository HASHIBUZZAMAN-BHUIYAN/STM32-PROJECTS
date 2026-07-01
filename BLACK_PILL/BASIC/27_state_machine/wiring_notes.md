# Wiring Notes — Black Pill BASIC/27: State Machine Traffic Light

## Pin Assignment
| Black Pill Pin | Signal | Component |
|---------------|--------|----------|
| PA5 | Red LED | 330Ω → LED → GND |
| PA6 | Amber LED | 330Ω → LED → GND |
| PA7 | Green LED | 330Ω → LED → GND |
| PA0 | Emergency button | Onboard user button |
| PC13 | Status LED | Onboard (active-LOW) |
| PA9 | USART1_TX | USB-UART adapter RX |

## Wiring Diagram
```
PA5 ─── 330Ω ─── Red LED (+) ─── GND
PA6 ─── 330Ω ─── Amber/Yellow LED (+) ─── GND
PA7 ─── 330Ω ─── Green LED (+) ─── GND
```

## LED Colors
Use:
- Red LED for PA5 (V_f ≈ 2.0V)
- Yellow/Orange LED for PA6 (V_f ≈ 2.0V)
- Green LED for PA7 (V_f ≈ 2.1V)

All with 330Ω current-limiting resistors.

## PA0 Button (Onboard)
PA0 is the WeAct Black Pill's user button — no extra wiring needed for emergency input.

## PA5/PA6/PA7 Note (No SPI Conflict Needed)
PA5/PA6/PA7 are SPI1 pins on STM32F411. Since we're using them as GPIO here, SPI1 must be disabled in CubeMX. If you want to run SPI1 and this project together, choose different GPIO pins.

## Terminal Output
115200 baud, see state transitions:
```
→ RED
→ RED+AMBER
→ GREEN
→ AMBER
→ RED
→ EMERGENCY  ← when PA0 pressed
→ RED
```

## Parts List
| Component | Qty |
|-----------|-----|
| Black Pill (STM32F411CEU6) | 1 |
| Red LED | 1 |
| Yellow/Orange LED | 1 |
| Green LED | 1 |
| 330Ω resistors | 3 |
| USB-UART adapter | 1 |
| USB-C cable | 1 |
| Breadboard + jumpers | 1 set |
