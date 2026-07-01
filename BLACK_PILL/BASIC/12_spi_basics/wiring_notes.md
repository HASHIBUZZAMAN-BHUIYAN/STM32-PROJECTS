# Wiring Notes — Black Pill BASIC/12: SPI Basics (MAX7219)

## Pin Assignment
| Black Pill Pin | Signal | MAX7219 Module |
|---------------|--------|---------------|
| PA5 | SPI1_SCK | CLK |
| PA7 | SPI1_MOSI | DIN |
| PA4 | GPIO CS | CS (LOAD) |
| 5V (VBUS) | VCC | VCC |
| GND | GND | GND |

Note: MISO (PA6) is not used by MAX7219 (write-only). Configure SPI1 as Full-Duplex but MISO pin is unused.

## SPI1 on Black Pill — No LED Conflict
Unlike Nucleo-F401RE where PA5 is the LD2 LED, the Black Pill's LED is on PC13. PA5 (SPI1_SCK) is freely available for SPI use.

## MAX7219 Power
MAX7219 requires 5V (not 3.3V) for proper LED current. Connect VCC to Black Pill's 5V (VBUS) pin.
The SPI signals (CLK, DIN, CS) from the STM32 are 3.3V but MAX7219 inputs are 5V-tolerant → 3.3V signals are accepted.

## SPI Speed
MAX7219 max clock: 10 MHz. With SPI1 prescaler /16 at 100 MHz → 6.25 MHz — safe.

## Wiring Diagram
```
Black Pill PA5 ─── MAX7219 CLK
Black Pill PA7 ─── MAX7219 DIN
Black Pill PA4 ─── MAX7219 CS
Black Pill 5V  ─── MAX7219 VCC
Black Pill GND ─── MAX7219 GND
```

## Parts List
| Component | Qty | Notes |
|-----------|-----|-------|
| Black Pill (STM32F411CEU6) | 1 | |
| MAX7219 8×8 LED matrix module | 1 | |
| USB-C cable | 1 | |
| Jumper wires | 5 | |
