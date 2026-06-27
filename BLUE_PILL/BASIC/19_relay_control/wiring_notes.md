# Wiring Notes — 19 Relay Control

## Voltage Warning
**All GPIO pins on the Blue Pill operate at 3.3 V logic.**
Standard relay modules with an onboard optocoupler and transistor driver accept 3.3 V signals on their input pin. Verify your relay module's datasheet — some cheap modules require 5 V to reliably trigger the optocoupler. If needed, power the module from 5 V (USB VCC) and use a 3.3 V → 5 V level shifter on the signal line, or confirm the module's minimum logic HIGH voltage is below 3.3 V.

## MAINS SAFETY — CRITICAL
- The relay load-side terminals (COM, NO, NC) may be connected to **mains AC voltage (110 V or 230 V)** or high DC voltages.
- **Never touch load-side wiring while energized.**
- Use appropriately rated wire (typically 1.5 mm² / 16 AWG minimum for 10 A loads).
- Enclose any mains wiring in a suitable rated enclosure.
- The relay contact rating (e.g., 10 A / 250 VAC) must exceed the actual load.
- The Blue Pill's 3.3 V side is galvanically isolated from the mains side through the relay coil — maintain this isolation; do not bridge it.

## Control-Side Pin Connections

| Signal | Blue Pill Pin | Relay Module Pin | Notes |
|--------|--------------|-----------------|-------|
| Relay Signal | PB5 | IN (or S) | HIGH = relay ON (active-high module) |
| Module VCC | 5 V (USB) | VCC | Most relay modules need 5 V coil supply |
| Common GND | GND | GND | Shared ground |

## Relay Load-Side Terminals (mains side — use with caution)

| Terminal | Meaning |
|----------|---------|
| COM | Common — connect one side of load or supply |
| NO | Normally Open — connects to COM when relay is energized |
| NC | Normally Closed — connects to COM when relay is de-energized |

For a simple "load ON when relay energized" application: use COM and NO.

## Active-HIGH vs Active-LOW Modules
- **Active-HIGH module** (most common): IN=HIGH → relay energized. Code uses `RELAY_ON = GPIO_PIN_SET`.
- **Active-LOW module** (some blue relay boards with JD-VCC jumper): IN=LOW → relay energized. Change `RELAY_ON = GPIO_PIN_RESET` and `RELAY_OFF = GPIO_PIN_SET` in the defines.

## Flyback / Suppression
- Relay coil is an inductive load. Relay modules include a flyback diode. If using a bare relay (no module), add a 1N4007 diode across the coil (cathode to positive).

## Onboard LED Mirror
- PC13 (active-low) mirrors relay state in the code: LED ON when relay is energized. This provides a safe visual indicator without touching the load side.
