# Nucleo-F401RE BASIC/20 — Sleep/Stop/Standby Power Mode Comparison

## Overview
Demonstrates and compares all three F4 low-power modes: Sleep (~5-10 mA), Stop (~0.5 mA), and Standby (~3 µA). Each mode is entered in sequence; the user wakes from each using B1 (PC13 EXTI13) or an RTC alarm for Standby.

## CubeMX Configuration

| Peripheral | Setting |
|-----------|---------|
| GPIO PC13 | EXTI13, rising+falling edge, Pull-up (B1 button) |
| GPIO PA5 | Output (LD2) |
| USART2 | 115200 8N1, PA2/PA3 |
| RTC | LSI clock, Alarm A enabled |
| PWR | Low-power run, Stop, Standby |

**NVIC**: EXTI15_10 enabled (priority 1).

## Power Mode Summary

| Mode | CPU | Peripherals | Wakeup Source | Typical Current |
|------|-----|-------------|---------------|----------------|
| Sleep | Halted | Active | Any interrupt | ~10 mA |
| Stop | Halted | Most off | EXTI, RTC alarm | ~0.5 mA |
| Standby | Off | Off (RTC only) | WKUP pin, RTC | ~3 µA |

## Parts List
| Component | Qty | Notes |
|-----------|-----|-------|
| Nucleo-F401RE | 1 | |
| USB cable | 1 | For ST-Link + VCP |
| Multimeter (µA range) | 1 | Optional: measure via IDD jumper |

## Expected Behavior
1. Prints "Entering Sleep" → B1 press wakes it
2. Prints "Entering Stop" → B1 press wakes it (clock re-init needed)
3. Prints "Entering Standby" → RTC alarm wakes after 10s (board resets)
Cycle repeats.
