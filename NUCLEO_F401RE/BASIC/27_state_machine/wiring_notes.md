# Wiring Notes — Nucleo-F401RE BASIC/27: Traffic Light State Machine

## ⚠️ 3.3V Logic Warning
LED current limiting resistors are required. GPIO pins source up to ~25mA but typical LEDs need 330Ω to limit to ~10mA at 3.3V.

## LED Wiring
| Signal | Nucleo Pin | Connection |
|--------|-----------|-----------|
| Red LED | PB5 | PB5 → 330Ω → LED anode → LED cathode → GND |
| Yellow LED | PB4 | PB4 → 330Ω → LED anode → LED cathode → GND |
| Green LED | PB3 | PB3 → 330Ω → LED anode → LED cathode → GND |

## Emergency Button
B1 (PC13) is the onboard user button — no extra button needed.

## Parts List
| Component | Qty |
|-----------|-----|
| Nucleo-F401RE | 1 |
| Red 5mm LED | 1 |
| Yellow 5mm LED | 1 |
| Green 5mm LED | 1 |
| 330 Ω resistors | 3 |
| USB cable | 1 |
