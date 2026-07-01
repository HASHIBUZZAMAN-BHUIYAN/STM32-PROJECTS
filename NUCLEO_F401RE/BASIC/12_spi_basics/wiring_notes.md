# Wiring Notes — Nucleo-F401RE BASIC/12: SPI Basics

## ⚠️ 3.3V Logic Warning
All SPI pins are 3.3V. SPI devices must be 3.3V compatible.

## Why SPI2 (not SPI1)?
SPI1 uses PA5 (SCK), but PA5 is also the onboard LD2 LED on Nucleo-F401RE. Using SPI1 and the LED simultaneously is impossible. SPI2 on PB13/PB14/PB15 has no conflict.

## Loopback Test Wiring
Simply jumper **PB14 (MISO)** to **PB15 (MOSI)** with a short wire.

| Signal | Nucleo Pin |
|--------|-----------|
| SCK | PB13 |
| MISO | PB14 ← bridge to MOSI |
| MOSI | PB15 → bridge to MISO |
| CS | PB12 (GPIO output) |

## MAX7219 LED Driver (optional)
| MAX7219 | Nucleo |
|---------|--------|
| VCC | 5V (MAX7219 needs 5V supply) |
| GND | GND |
| DIN | PB15 (MOSI) |
| CLK | PB13 (SCK) |
| LOAD/CS | PB12 |

Note: MAX7219 DIN accepts 3.3V logic even with 5V supply. No level shift needed.

## Parts List
| Component | Qty |
|-----------|-----|
| Nucleo-F401RE | 1 |
| Jumper wire (loopback) | 1 |
| MAX7219 8×8 LED matrix (optional) | 1 |
| USB cable | 1 |
