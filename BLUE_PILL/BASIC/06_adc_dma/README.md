# 06 — ADC with DMA (Continuous Conversion)

## Overview

Reads analog voltage on **PA1 (ADC1 Channel 1)** using DMA in circular mode.
The DMA engine fills a 16-element `uint32_t` buffer continuously without CPU involvement.
The main loop averages the buffer and blinks the onboard LED to indicate activity.

## Hardware

| Signal | Pin |
|--------|-----|
| Analog Input | PA1 (ADC1 CH1) |
| Onboard LED | PC13 (active-low) |

## CubeMX Configuration

### ADC1
- Mode: Independent, Continuous Conversion Mode ON
- Channel 1 (PA1), Sampling Time: 239.5 cycles (for stable reading)
- DMA Request: ADC1 → DMA1 Channel 1
- DMA Mode: Circular
- Data Width: Word (32-bit) → Word (32-bit)
- Scan Conversion Mode: Disabled (single channel)
- EOC Selection: EOC flag at end of single channel conversion

### DMA1 Channel 1
- Direction: Peripheral to Memory
- Mode: Circular
- Peripheral increment: Disabled
- Memory increment: Enabled
- Data width: Word / Word

### Clock
- System Clock: 72 MHz (HSE + PLL recommended)
- ADC Prescaler: /6 → 12 MHz ADC clock (must be ≤ 14 MHz)

### NVIC
- No ADC interrupt needed (DMA handles transfer)

## Build & Flash

1. Generate code with CubeMX into a project (HAL, C, Makefile or STM32CubeIDE).
2. Copy the user code sections from `main_usercode.c` into the matching `/* USER CODE BEGIN/END */` blocks in the generated `main.c`.
3. Build and flash via ST-Link.

## Expected Behavior

- LED blinks at ~1 Hz.
- Averaged ADC value (0–4095) is computed each loop iteration.
- Connect PA1 to a potentiometer wiper (between 3.3V and GND) to see the value change.
- Add a UART print (project 07) to observe the value over serial if desired.

## Notes

- Code is **not tested on hardware**. Verify DMA handle name (`hdma_adc1`) matches your CubeMX output.
- ADC resolution is 12-bit (0 = 0 V, 4095 = 3.3 V).
- Do NOT connect PA1 to voltages above 3.3 V — see `wiring_notes.md`.
