# Black Pill BASIC/29 — Power Consumption Measurement Guide

## Overview
Guide for measuring the Black Pill's power consumption in different modes. No dedicated IDD jumper is present on most Black Pill boards (unlike Nucleo), so measurement requires breaking the power supply line or using a USB power monitor.

## Black Pill Power Measurement Methods

### Method 1: USB Power Meter (Easiest)
Insert a USB power meter (e.g., UM25C, "ChargerLAB POWER-Z") between the USB port and the PC:
- Pros: no modification, reads current in real time
- Cons: includes USB circuit overhead (~8mA from USB pull-up resistors)

### Method 2: Cut VCC Trace + Multimeter
1. Find the 3.3V LDO on the board (AMS1117-3.3 on WeAct)
2. Carefully cut the trace between LDO output and MCU VDD
3. Insert multimeter (mA range) in series
4. Cons: permanent board modification, tricky

### Method 3: Current Shunt on External Supply
Power via 3.3V pin from bench PSU (not USB):
1. Connect bench PSU 3.3V to 3.3V pin
2. Insert 1Ω or 10Ω shunt resistor in series with 3.3V
3. Measure voltage across shunt → I = V/R
4. Accurate for µA range measurements with 10Ω shunt

## Expected Current Values
| Mode | Current (MCU only) |
|------|-------------------|
| Active, 100 MHz | 30–40 mA |
| Sleep mode | 8–12 mA |
| Stop mode | 0.5–1.5 mA |
| Standby | ~2 µA |

Excludes USB circuit (~8mA) and LDO quiescent current (~5mA at light load).

## Test Firmware
Use BASIC/18 (Low Power Modes Comparison) as the test firmware.

## USB Circuit Overhead
The WeAct Black Pill has USB D+/D- pull-up resistors that keep the USB connection alive. This adds ~5–10mA in all modes. To measure true MCU-only current, disconnect USB and power via 3.3V pin from bench PSU.
