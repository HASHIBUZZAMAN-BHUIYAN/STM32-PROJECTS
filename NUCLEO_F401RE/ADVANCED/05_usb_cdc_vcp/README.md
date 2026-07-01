# Nucleo-F401RE ADVANCED/05 — USB CDC Virtual COM Port

## Overview
Turns the Nucleo into a USB CDC (Communications Device Class) device so it appears as a COM port on the host PC without any USB-to-UART adapter. The firmware echoes received data with a length prefix and also streams ADC readings on demand.

## How USB CDC Works on F401
The STM32F401 has a USB_OTG_FS peripheral (PA11=D-, PA12=D+). With the USB CDC middleware, it enumerates as a COM port on Windows (drivers built-in since Win10), macOS, and Linux.

**Important**: The USB connector on the Nucleo board goes through the ST-Link chip, not directly to the STM32. For USB CDC from the STM32 itself, you need an external USB connection to PA11/PA12 via a micro-USB breakout or direct connection.

## CubeMX Configuration
| Item | Setting |
|------|---------|
| USB_OTG_FS | Device Only mode |
| USB_DEVICE | CDC class |
| PA11 | USB_OTG_FS_DM (auto-assigned) |
| PA12 | USB_OTG_FS_DP (auto-assigned) |
| USART2 | 115200 (still available, separate from USB) |
| ADC1 | PA0, single conversion |

Generated files: `usbd_cdc_if.c/h`, `usb_device.c/h`, `usbd_conf.c/h`

## API Used
```c
#include "usbd_cdc_if.h"
// Send data over USB CDC:
uint8_t CDC_Transmit_FS(uint8_t *Buf, uint16_t Len);
// Receive: override CDC_Receive_FS() in usbd_cdc_if.c
```

## Expected Behavior
- Plug USB cable into PA11/PA12 breakout → PC sees new COM port
- Open any terminal at any baud rate (CDC ignores baud setting)
- Type text → STM32 echoes back with prefix: `[3 bytes] abc`
- Type `adc` → STM32 replies with current ADC1 reading

## Windows Driver
Windows 10/11: automatic (CDC ACM built-in)  
Windows 7/8: requires STM32 VCP driver from ST website

## PC Terminal vs ST-Link VCP
| | ST-Link VCP | USB CDC (this project) |
|-|------------|----------------------|
| Connector | CN1 (USB-A on board) | External wires to PA11/PA12 |
| Peripheral | ST-Link USB bridge | STM32 USB_OTG_FS |
| Baud rate | Fixed at configured rate | Ignored (always full USB speed) |
| Max throughput | ~1 Mbps | ~10 Mbps |
