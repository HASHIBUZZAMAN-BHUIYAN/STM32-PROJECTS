# Black Pill BASIC/12 — SPI Basics (SPI1, Max7219 LED Matrix)

## Overview
Demonstrates SPI1 on the Black Pill (PA5=SCK, PA6=MISO, PA7=MOSI). Drives a MAX7219 8×8 LED matrix display. Unlike the Nucleo, SPI1 on the Black Pill has NO conflict with the LED (PC13), so PA5 is free for SPI.

## Black Pill SPI1 Advantage
On Nucleo-F401RE, PA5 is LD2 LED — SPI1 conflicts with it, forcing use of SPI2.
On Black Pill, PC13 is the LED — **SPI1 on PA5/PA6/PA7 is conflict-free**.

## MCU Facts
| Item | Value |
|------|-------|
| MCU | STM32F411CEU6 |
| SPI | SPI1, PA5=SCK, PA6=MISO, PA7=MOSI |
| CS | PA4 (GPIO Output) |
| Target | MAX7219 LED Matrix controller |

## CubeMX Configuration
| Item | Setting |
|------|---------|
| SPI1 | Full-Duplex Master, 8-bit, CPOL=0, CPHA=0 |
| Prescaler | /8 → ~12.5 MHz (MAX7219 max: 10 MHz — use /16 for safety) |
| PA4 | GPIO Output (CS) |
| USART1 | PA9/PA10, 115200 8N1 |

## MAX7219 Protocol
- 16-bit SPI frames: [8-bit register address | 8-bit data]
- CS (LOAD) goes LOW → send 16 bits → CS HIGH
- Daisy-chainable: multiple MAX7219 in series on MOSI

## Expected Behavior
Scrolls a byte pattern across each row of the 8×8 matrix.

## Parts List
- STM32F411 Black Pill
- MAX7219 LED Matrix module (8×8, SPI)
- Jumper wires, USB-C cable
