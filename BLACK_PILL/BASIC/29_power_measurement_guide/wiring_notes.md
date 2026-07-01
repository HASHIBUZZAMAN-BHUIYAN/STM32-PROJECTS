# Wiring Notes — Black Pill BASIC/29: Power Measurement Guide

## Hardware Required
| Component | Qty | Notes |
|-----------|-----|-------|
| Black Pill (STM32F411CEU6) | 1 | |
| USB power meter (optional) | 1 | UM25C or similar |
| Bench PSU 3.3V (optional) | 1 | For accurate µA measurement |
| Multimeter with µA range | 1 | For Standby measurement |
| USB-UART adapter | 1 | For USART1 debug |
| USB-C cable | 1 | |

## Measurement Setup (USB Power Meter)
```
PC USB ─── USB Power Meter ─── Black Pill USB-C
```
Read current on the meter display. Add or subtract:
- USB circuit: ~8mA (always present via USB)

## Measurement Setup (Bench PSU — Most Accurate)
```
Bench PSU 3.3V ─── [1Ω shunt] ─── Black Pill 3V3 pin
Bench PSU GND  ───────────────── Black Pill GND
Multimeter across shunt: V/R = I
```

Power the Black Pill from its 3V3 header pin (bypassing the onboard USB LDO):
1. Do NOT connect USB-C while using bench PSU
2. Do NOT exceed 3.6V on the 3V3 pin

## No Extra Wiring for Firmware
The firmware uses onboard PA0 button and PC13 LED. USB-UART adapter on PA9 for output.

## 3.3V Safety
Never connect more than 3.6V to the 3V3 pin. The onboard LDO (AMS1117-3.3) regulates down from 5V — bypassing it means the MCU is directly powered, so overvoltage protection is gone.
