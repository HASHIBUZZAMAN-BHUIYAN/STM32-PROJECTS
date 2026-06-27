# Wiring Notes — 23: Internal Flash Read / Write

## CRITICAL: 3.3 V Logic
All STM32F103 GPIO pins operate at **3.3 V**. Do not connect any 5 V signal directly to any GPIO pin.

## Overview
This project requires no external components beyond a USB-TTL serial adapter.
All Flash operations are entirely on-chip.

## Pin Connections

| Blue Pill Pin | USB-TTL Adapter | Notes |
|---------------|-----------------|-------|
| PA9 (TX) | RX | UART1 transmit |
| PA10 (RX) | TX | UART1 receive (optional) |
| GND | GND | Common ground |
| PC13 | — | Onboard LED (active-low) — blinks during normal operation |

## UART1 (USB-TTL Adapter)
Terminal settings: **115200 baud, 8N1, no flow control**.

USB-TTL adapter logic level must be **3.3 V** (not 5 V). Check your module's jumper.

## Flash Safety Warnings

1. **Endurance**: STM32F103 Flash is rated for **10,000 erase cycles** per page.
   Do NOT erase on every loop iteration. The demo erases once per power cycle.

2. **Linker script**: Ensure your linker script does NOT place code or read-only data
   in page 63 (`0x0800FC00`–`0x0800FFFF`). Verify in the `.map` file generated at build.
   For a stock CubeMX project with <63 KB of code this is not an issue.

3. **Flash erase during execution**: The CPU cannot fetch from Flash while it is being
   erased on the same bank. On STM32F103 (single-bank), code MUST run from RAM during
   erase, or the erase must complete before the next fetch. The HAL driver handles this
   correctly — do not interrupt an erase operation.

4. **Voltage**: Flash erase/program requires VDD ≥ 2.7 V. USB-powered Blue Pill
   typically provides a stable 3.3 V.

## CubeMX Notes
- No special peripheral configuration is needed.
- HAL Flash drivers (`stm32f1xx_hal_flash.c`, `stm32f1xx_hal_flash_ex.c`) are included
  by default in CubeMX-generated projects for STM32F1.

## Power
- Power the Blue Pill from USB via Micro-USB or the 5 V pin.
- Do not remove power during a Flash write or erase — this can corrupt the page.
