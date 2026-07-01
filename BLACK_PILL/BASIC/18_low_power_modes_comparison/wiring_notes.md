# Wiring Notes — Black Pill BASIC/18: Low Power Modes

## Hardware Required
Only onboard Black Pill components:
- PA0 = user button (press to cycle modes)
- PC13 = onboard LED (active-LOW)
- USART1 via USB-UART adapter for output

## USART1 Connection
| Black Pill | USB-UART Adapter |
|-----------|----------------|
| PA9 (TX) | RX |
| GND | GND |

## Measuring Power in Stop/Standby
The Black Pill has no dedicated IDD measurement jumper. Measure power with:
- Break USB-C VCC line, insert multimeter (mA range)
- Or use USB power meter dongle (shows current draw from USB)
- For µA standby: disconnect USB-UART adapter, use battery + dedicated µA meter

## RTC Clock for Standby
Standby wakeup requires an RTC running from a clock that survives power-down:
- **LSE** (32.768 kHz crystal): requires external crystal on PC14/PC15 (not always present on Black Pill boards)
- **LSI** (internal ~32 kHz RC): no external component needed; ±5% accuracy

If your Black Pill doesn't have the LSE crystal, use LSI in CubeMX: RCC → Low Speed Clock (LSI).

## Parts List
| Component | Qty |
|-----------|-----|
| Black Pill (STM32F411CEU6) | 1 |
| USB-UART adapter | 1 |
| USB-C cable | 1 |
| Jumper wires | 2 |
