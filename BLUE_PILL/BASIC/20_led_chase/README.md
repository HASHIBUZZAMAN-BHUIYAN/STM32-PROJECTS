# 20 — LED Chase (Knight Rider)

## Overview
Classic "Knight Rider" LED chase pattern across 8 LEDs connected to PB8–PB15. A single lit LED sweeps left then right continuously, creating a back-and-forth scanning effect. Uses HAL_GPIO_WritePin with bitmask to update all 8 LEDs in one call per step.

## Hardware
- Board: Blue Pill (STM32F103C8T6)
- LEDs: 8× standard 5 mm LED (any color)
- Current limiting resistors: 8× 330 Ω (for 3.3 V GPIO)
- LED pins: PB8, PB9, PB10, PB11, PB12, PB13, PB14, PB15

## CubeMX Configuration
| Field | Value |
|-------|-------|
| PB8–PB15 | GPIO Output Push-Pull, no pull, Speed: Low |
| Initial output level | LOW (all LEDs off at reset) |

Configure all 8 pins as GPIO Output in CubeMX. No timers or alternate functions needed.

## LED Array Mapping
| Index | Pin | Position |
|-------|-----|---------|
| 0 | PB8 | Left |
| 1 | PB9 | |
| 2 | PB10 | |
| 3 | PB11 | Centre-left |
| 4 | PB12 | Centre-right |
| 5 | PB13 | |
| 6 | PB14 | |
| 7 | PB15 | Right |

## Behavior
1. LED at position 0 (PB8) lights, all others off.
2. Steps right: PB9, PB10 … PB15 (each for `CHASE_DELAY_MS`).
3. Steps left: PB14, PB13 … PB8.
4. Repeat.

## Files
| File | Purpose |
|------|---------|
| `main_usercode.c` | User code sections to paste into CubeMX-generated `main.c` |
| `wiring_notes.md` | Pin connections and safety notes |

## Disclaimer
Code is provided as a reference starting point and has not been tested on hardware.
