# Nucleo-F401RE BASIC/29 — Power Consumption Measurement Guide

## Overview
A guide project explaining how to measure the Nucleo-F401RE's current consumption in different power modes. No novel firmware — the demo firmware from BASIC/20 (Sleep/Stop/Standby) is reused here as test firmware. The primary content is the measurement methodology.

## The IDD Measurement Jumper
The Nucleo-F401RE has an **IDD measurement jumper** (labeled JP6 on some board revisions, or "IDD" near the STM32 chip). It is a 2-pin solder bridge in series with the STM32's VDD supply:

1. **Remove the jumper** (or cut the solder bridge)
2. **Insert a multimeter** in series (µA/mA range)
3. The multimeter now measures only the STM32's supply current

**Note**: The ST-Link subsystem has its own separate power path and is NOT included in the IDD measurement. This gives a clean MCU-only reading.

## Expected Current Readings

| Mode | Expected (MCU only) | Notes |
|------|---------------------|-------|
| Active, 84 MHz, all peripherals on | 25–35 mA | Varies by peripheral usage |
| Sleep mode (WFI) | 8–12 mA | CPU halted; peripherals on |
| Stop mode (LP regulator) | 0.4–1.2 mA | Most peripherals off |
| Standby mode | 2–6 µA | Only RTC and wakeup logic on |

Add ~8–15mA for the ST-Link chip if measuring from the USB 5V rail instead of the IDD jumper.

## Comparison with Other Boards

| Board | Active (typical) | Stop/Standby |
|-------|----------------|-------------|
| Blue Pill (F103, 72 MHz) | 25–35 mA | ~5µA standby |
| Nucleo-F401RE (84 MHz) | 25–35 mA | ~2µA standby |
| Black Pill (F411, 100 MHz) | 30–40 mA | ~2µA standby |

The F4/F411 standby current is generally lower than F103 because the backup domain is more efficient.

## CubeMX Configuration (for test firmware)
Same as BASIC/20: RTC, PWR, GPIO PC13 EXTI, USART2.

## How to Measure
1. Flash BASIC/20 firmware (or use main_usercode.c below)
2. Open IDD jumper, insert multimeter in series
3. Open serial terminal — firmware prints current mode before entering it
4. Observe multimeter reading in each mode
5. Stop and Standby modes: expect to see µA readings; meter must support µA range

## Safety
- Do NOT short the IDD jumper location — this bypasses the MCU power supply measurement
- Use a high-quality multimeter with a µA range for Standby measurements (cheap meters may read unreliably below 1mA)
