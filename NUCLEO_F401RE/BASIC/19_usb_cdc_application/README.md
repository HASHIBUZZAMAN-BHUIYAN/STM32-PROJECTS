# Nucleo-F401RE BASIC/19 — USB CDC Application Port

## Overview
Creates a USB CDC (Communication Device Class) virtual COM port directly from the STM32F401 application — separate from the ST-Link VCP. The F401RE application USB (PA11/PA12) enumerates as a second COM port on the PC. Demonstrates the STM32 USB Device stack via CubeMX middleware.

## CubeMX Configuration

| Peripheral | Setting | Pins |
|-----------|---------|------|
| USB_OTG_FS | Device mode | PA11 (DM), PA12 (DP) |
| USB_DEVICE | CDC class | — |
| USART2 | 115200 8N1 | PA2/PA3 (ST-Link VCP for debug) |

**Middleware**: Under Middleware → USB_DEVICE → Class = CDC (Virtual Port Com). CubeMX generates `usb_device.c`, `usbd_cdc_if.c`, etc. Your transmit calls go in USER CODE of `usbd_cdc_if.c` or call `CDC_Transmit_FS()` from `main.c`.

**Clock**: USB requires exactly 48 MHz on the USB clock. CubeMX enforces this; ensure HCLK is at 84 MHz and USB clock = 48 MHz (separate PLL48CK path).

## Parts List
| Component | Qty | Notes |
|-----------|-----|-------|
| Nucleo-F401RE | 1 | |
| USB cable | 2 | One for CN1 (ST-Link), one for CN5 or user USB pins |
| 1.5 kΩ pull-up resistor | 1 | PA12 to 3.3V — needed if board doesn't have it |

## Wiring
PA11/PA12 are available on the Nucleo morpho connectors (CN10). Wire to a USB type-A socket (or use a breakout): PA12→D+, PA11→D−, GND→GND. A 1.5 kΩ pull-up on PA12 is usually required for USB Full-Speed enumeration.

## Expected Behavior
After flashing, a second COM port appears in Device Manager. Open it at any baud (CDC ignores baud). Device sends "USB CDC Nucleo F401RE\r\n" and the tick counter every 500ms.
