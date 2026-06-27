# Wiring Notes — 22: Hardware CRC Peripheral

## CRITICAL: 3.3 V Logic
All STM32F103 GPIO pins operate at **3.3 V**. Do not connect any 5 V signal directly to any GPIO pin.

## Overview
This project requires no external components beyond a USB-TTL serial adapter.
The CRC computation is entirely on-chip.

## Pin Connections

| Blue Pill Pin | USB-TTL Adapter | Notes |
|---------------|-----------------|-------|
| PA9 (TX) | RX | UART1 transmit |
| PA10 (RX) | TX | UART1 receive (optional for this demo) |
| GND | GND | Common ground |
| PC13 | — | Onboard LED (active-low, no external wiring needed) |

## UART1 (USB-TTL Adapter)
Terminal settings: **115200 baud, 8N1, no flow control**.

The USB-TTL adapter's TX/RX lines must be **3.3 V logic**. Most CP2102 / CH340 / FT232
modules can switch between 3.3 V and 5 V via a jumper — ensure it is set to **3.3 V**.

## CubeMX Notes
- Enable the **CRC** peripheral under Peripherals → Computing → CRC.
- No clock or pin configuration is required for the CRC unit itself.
- Ensure `MX_CRC_Init()` is called in main.c (generated automatically by CubeMX).
- The `hcrc` handle used in user code is declared by CubeMX in main.c.

## Power
- Power the Blue Pill from USB (5 V to VBUS pin or via Micro-USB).
- The onboard 3.3 V regulator supplies the MCU.
