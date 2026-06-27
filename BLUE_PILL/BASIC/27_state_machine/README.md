# 27 — Button-Driven State Machine

## Overview

This project implements a clean four-state finite state machine (FSM) in bare-metal C. A button on PA0 drives state transitions via EXTI (external interrupt). The onboard LED (PC13, active-low) behaves differently in each state:

| State | LED Behavior |
|---|---|
| IDLE | On solid (LED lit, PC13 = LOW) |
| BLINK_SLOW | 500 ms on / 500 ms off |
| BLINK_FAST | 100 ms on / 100 ms off |
| OFF | Off solid (PC13 = HIGH) |

Button presses cycle: IDLE → BLINK_SLOW → BLINK_FAST → OFF → IDLE → ...

## MCU

STM32F103C8T6 (Blue Pill), 72 MHz

## Key Concepts

- **EXTI on PA0**: Button press generates an interrupt. EXTI0 ISR sets a flag; the main loop processes it.
- **Software debounce**: A 50 ms timestamp guard prevents multiple transitions per press.
- **enum + switch**: Clean FSM pattern with no dynamic allocation.
- **Non-blocking blink**: `HAL_GetTick()` timestamps replace `HAL_Delay()` so the main loop stays responsive.

## CubeMX Configuration

| Peripheral | Setting |
|---|---|
| PC13 | GPIO Output, Push-Pull, No Pull, Low speed |
| PA0 | GPIO_EXTI0, Pull-Up (button ties PA0 to GND when pressed) |
| EXTI0 | Rising/Falling edge — use Falling if button pulls PA0 LOW |
| NVIC | EXTI0 interrupt enabled, preempt priority 1 |
| Clock | HSE 8 MHz → PLL → 72 MHz |

**Button wiring assumed**: PA0 pulled HIGH internally (Pull-Up enabled), button connects PA0 to GND. Configure EXTI for **Falling** edge trigger.

## Files

| File | Purpose |
|---|---|
| `main_usercode.c` | User code sections — enum, ISR flag, switch logic |
| `wiring_notes.md` | Hardware connections and voltage notes |

## Note

This code has not been tested on hardware.
