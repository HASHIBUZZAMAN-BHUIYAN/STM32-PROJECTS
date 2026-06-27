# Wiring Notes — BASIC/01 Blink LED

## Pin Diagram
```
Nucleo-F401RE
+---------------------------+
| PA5 ──► LD2 (onboard LED) |
|                           |
| USB (CN1) ──► PC (power + |
|              ST-Link)     |
+---------------------------+
```

## Parts List
| Item | Qty | Notes |
|------|-----|-------|
| Nucleo-F401RE | 1 | Includes onboard LED LD2 |
| USB-A to Mini-B cable | 1 | Power + programming |

## Safety Notes
⚠ ALL GPIO pins on the STM32F401RE operate at 3.3V logic. Do NOT connect 5V signals directly to any GPIO pin.
- LD2 is onboard — no external connections needed for this project.
- PA5 is shared with SPI1 SCK. Do not use SPI1 and this project simultaneously.

## Notes
- No breadboard or external components required.
- Onboard ST-Link provides power via USB.
