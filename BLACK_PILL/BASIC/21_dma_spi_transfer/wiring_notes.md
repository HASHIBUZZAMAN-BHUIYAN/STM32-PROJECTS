# Wiring Notes — Black Pill BASIC/21: DMA SPI Transfer

## Pin Assignment
| Black Pill Pin | Signal | W25Q32 Module |
|---------------|--------|--------------|
| PA5 | SPI1_SCK | CLK |
| PA6 | SPI1_MISO | DO (MISO) |
| PA7 | SPI1_MOSI | DI (MOSI) |
| PA4 | CS (GPIO Output) | CS |
| 3.3V | VCC | VCC |
| GND | GND | GND |

## W25Q32 Module
The W25Q32 is a 32Mbit (4MB) SPI NOR flash chip. Common breakout modules include the chip, decoupling caps, and sometimes a level shifter.

```
Black Pill PA5 ─── W25Q32 CLK
Black Pill PA6 ─── W25Q32 DO
Black Pill PA7 ─── W25Q32 DI
Black Pill PA4 ─── W25Q32 CS
Black Pill 3.3V ── W25Q32 VCC + HOLD + WP (tie HOLD and WP to VCC)
Black Pill GND ─── W25Q32 GND
```

## W25Q32 HOLD and WP Pins
- **HOLD** (pin 7): Active-LOW; tie to VCC (3.3V) to disable
- **WP** (write protect, pin 3): Active-LOW; tie to VCC to allow writing
If these float low, the chip may lock up or refuse writes.

## SPI1 on Black Pill — No LED Conflict
PA5 (SPI1_SCK) is free on Black Pill (LED is PC13). No conflict.

## DMA2 for SPI1
On STM32F4, SPI1 DMA uses DMA2 (not DMA1). CubeMX enforces this automatically.

## Parts List
| Component | Qty | Notes |
|-----------|-----|-------|
| Black Pill (STM32F411CEU6) | 1 | |
| W25Q32 SPI flash module | 1 | Or W25Q64/W25Q128 — same pinout |
| USB-UART adapter | 1 | |
| USB-C cable | 1 | |
| Breadboard + jumpers | 1 set | |

## 3.3V Safety
W25Q32 operates at 2.7–3.6V. SPI signals from Black Pill are 3.3V — compatible directly.
