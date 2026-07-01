# Wiring Notes — Nucleo-F401RE BASIC/29: Power Measurement Guide

## The IDD Jumper
Locate the IDD jumper on your Nucleo board (check the board's user manual — UM1724 for Nucleo-F401RE):
- It's typically a 2-pin solder bridge near the STM32 chip on the bottom of the board
- Remove the jumper / bridge and insert a multimeter in series (µA mode for Stop/Standby)

## Current Measurement Setup
```
USB 5V → voltage regulator → [MULTIMETER] → STM32 VDD
                                                ↑
                                          Break this path
                                          Insert meter here
```

## No Extra Components Required
Only the Nucleo board and a multimeter are needed. B1 button (PC13) cycles through modes.

## Practical Tips
1. **Auto-ranging meter**: Use a meter with auto-range for µA. Standby current is 2-6 µA — most cheap meters struggle below 1 mA
2. **ST-Link current**: Don't measure from the USB 5V rail — that includes the ST-Link chip (~10mA). Use the IDD jumper to measure MCU only
3. **Stop mode quirk**: After Stop, `SystemClock_Config()` must be called to restore 84 MHz — the clock falls back to HSI 16 MHz on Stop wakeup
4. **Standby wakeup**: Looks like a reset — board re-runs from `main()`. Use RTC backup register to detect Standby wakeup vs cold boot

## Parts List
| Component | Qty |
|-----------|-----|
| Nucleo-F401RE | 1 |
| Multimeter (µA range) | 1 |
| USB cable | 1 |
