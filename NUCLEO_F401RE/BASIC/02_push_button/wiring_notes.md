# Wiring Notes — BASIC/02 Push Button

## Pin Diagram
```
Nucleo-F401RE
+-------------------------------+
| PC13 ◄── B1 (onboard button)  |
|           (active-LOW, pull-up)|
|                               |
| PA5  ──► LD2 (onboard LED)    |
+-------------------------------+
```

## Parts List
| Item | Qty | Notes |
|------|-----|-------|
| Nucleo-F401RE | 1 | B1 and LD2 both onboard |
| USB cable | 1 | |

## Safety Notes
⚠ ALL GPIO pins operate at 3.3V logic. Do NOT apply 5V to any GPIO pin.
- B1 and LD2 are onboard — no external wiring needed.
- PC13 has a hardware pull-up resistor on the Nucleo board; enabling the internal pull-up in CubeMX is safe but redundant.

## Debounce
- Software debounce: 50 ms window via HAL_GetTick().
- If bounce is still observed, increase DEBOUNCE_MS to 80–100.
