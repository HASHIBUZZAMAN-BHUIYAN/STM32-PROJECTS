# Wiring Notes — BASIC/05 ADC Potentiometer

## Pin Diagram
```
    3.3V (CN8 pin 4)
      │
     [10kΩ Pot]── Wiper ──► PA0 / A0 (CN8 pin 1)
      │
    GND (CN8 pin 6)
```

## Parts List
| Item | Qty | Notes |
|------|-----|-------|
| Nucleo-F401RE | 1 | |
| 10kΩ potentiometer | 1 | Any value 1k–100k works |
| Breadboard | 1 | |
| Jumper wires | 3 | 3.3V, GND, signal |

## Safety Notes
⚠ ALL GPIO pins operate at 3.3V logic. The ADC reference voltage is 3.3V.
- NEVER apply more than 3.3V to PA0. Applying 5V WILL damage the MCU.
- If using a 5V potentiometer supply, use a voltage divider to scale to 0–3.3V.
- The 10kΩ pot draws ~0.33 mA from the 3.3V rail — acceptable.

## Connector Reference (Nucleo-F401RE)
- CN8 pin 1 = A0 = PA0 (ADC input)
- CN8 pin 4 = 3.3V supply
- CN8 pin 6 = GND
