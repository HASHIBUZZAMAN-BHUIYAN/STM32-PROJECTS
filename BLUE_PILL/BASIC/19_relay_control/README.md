# 19 — Relay Control

## Overview
Drives a relay module from PB5 using a GPIO output. A relay module with built-in transistor/optocoupler accepts a 3.3 V digital signal directly. The relay toggles every 2 seconds, cycling an externally connected load (e.g., a lamp or fan) on and off.

## Hardware
- Board: Blue Pill (STM32F103C8T6)
- Relay module: Any standard 5 V relay module with optocoupler input (e.g., SRD-05VDC-SL-C)
- Control pin: PB5 (GPIO Output)
- Onboard LED: PC13 (active-low) — mirrors relay state as visual indicator

## CubeMX Configuration
| Field | Value |
|-------|-------|
| PB5 | GPIO Output Push-Pull, no pull, initial LOW |
| PC13 | GPIO Output Push-Pull (onboard LED, optional mirror) |

No timer or alternate function configuration required.

## Behavior
1. PB5 goes HIGH → relay energizes (load ON). LED toggles.
2. 2-second delay.
3. PB5 goes LOW → relay de-energizes (load OFF). LED toggles.
4. 2-second delay.
5. Repeat.

## Safety — READ CAREFULLY
- Relay modules switch the connected load circuit which may operate at **mains voltage (110/220 VAC)** or high DC voltages.
- Never touch or probe the mains-side wiring while the circuit is powered.
- Use appropriate wire gauge, terminal connectors, and enclosures rated for mains voltage.
- Ensure the relay's contact rating (current and voltage) exceeds the load requirements.
- The Blue Pill and relay module control side are isolated from the mains side via the relay coil — do NOT bridge this isolation.

## Files
| File | Purpose |
|------|---------|
| `main_usercode.c` | User code sections to paste into CubeMX-generated `main.c` |
| `wiring_notes.md` | Pin connections and safety notes |

## Disclaimer
Code is provided as a reference starting point and has not been tested on hardware.
