# Wiring Notes — Nucleo-F401RE BASIC/19: USB CDC Application

## ⚠️ 3.3V Logic Warning
USB D+/D− are 3.3V signals. Do not connect to 5V USB directly without protection.

## USB Connections
The F401RE's USB OTG FS is on PA11 (D−) and PA12 (D+). These are on the morpho connector CN10:
- CN10 pin 14 = PA12 (USB_OTG_FS_DP)
- CN10 pin 12 = PA11 (USB_OTG_FS_DM)

Wire to a USB type-A female breakout or USB connector:
| USB Signal | Nucleo |
|-----------|--------|
| D− (white) | PA11 (CN10 pin 12) |
| D+ (green) | PA12 (CN10 pin 14) |
| GND (black) | GND |
| VBUS (red) | Leave unconnected or connect via diode — optional |

**Pull-up**: USB Full-Speed requires 1.5 kΩ pull-up on D+ (PA12) to 3.3V. Some boards include this; check if enumeration fails without it.

## Two COM Ports
After flashing this project, you will see TWO COM ports:
1. **ST-Link VCP** (CN1 USB) — for debug UART2 at 115200
2. **Application CDC** (user USB connector) — for USB CDC data

## Parts List
| Component | Qty |
|-----------|-----|
| Nucleo-F401RE | 1 |
| USB type-A female breakout | 1 |
| 1.5 kΩ resistor | 1 (D+ pull-up, if needed) |
| USB cables | 2 |
