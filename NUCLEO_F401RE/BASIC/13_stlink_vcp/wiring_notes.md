# Wiring Notes — Nucleo-F401RE BASIC/13: ST-Link VCP

## No Extra Wiring Required!
This is the key advantage of the Nucleo board.

## What's Built In
| Feature | Detail |
|---------|--------|
| ST-Link chip | On-board, no extra programmer needed |
| Virtual COM port | Appears on PC as USB serial when CN1 is connected |
| USART2 | PA2 (TX) and PA3 (RX) physically wired to the ST-Link chip |
| USB connector | CN1 — connects ST-Link + VCP to PC |

## Optional: Potentiometer for ADC
| Pot | Nucleo |
|-----|--------|
| Pin 1 (end) | 3.3V |
| Pin 2 (wiper) | PA0 (Arduino A0) |
| Pin 3 (end) | GND |

Without the pot, ADC reads floating/noise values — still demonstrates the JSON output format.

## PC Side
1. Plug USB cable into **CN1** (the ST-Link USB connector, usually labeled or closer to the ST-Link chip)
2. Device Manager → Ports → find "STMicroelectronics STLink Virtual COM Port"
3. Open serial terminal at **115200 baud, 8N1, no flow control**

## Contrast with Blue Pill
| | Nucleo-F401RE | Blue Pill |
|-|--------------|-----------|
| Serial to PC | Plug in USB (CN1) — done | Need external CP2102/CH340 USB-serial adapter |
| Extra cost | $0 | ~$1-2 |
| Extra wiring | None | 3 wires (TX/RX/GND) |
