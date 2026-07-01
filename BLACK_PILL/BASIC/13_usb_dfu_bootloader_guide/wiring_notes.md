# Wiring Notes — Black Pill BASIC/13: USB DFU Bootloader Guide

## Hardware Required
| Component | Qty | Notes |
|-----------|-----|-------|
| Black Pill (STM32F411CEU6) | 1 | |
| USB-C cable | 1 | Connect to PC for DFU |
| USB-UART adapter (optional) | 1 | For USART1 output on PA9 |

## No Extra Wiring for DFU
DFU uses the USB-C connector directly — no extra hardware needed.

## USART1 Debug (Optional)
| Black Pill | USB-UART |
|-----------|---------|
| PA9 (TX) | RX |
| GND | GND |

## BOOT0 Button Location
On WeAct Black Pill boards, the BOOT0 button is labeled **"B0"** and is located near the USB connector. The NRST button is labeled **"RST"** or **"NRST"**.

## DFU Driver (Windows)
Windows 10/11: DFU device shows as "STM32 BOOTLOADER" — drivers included with STM32CubeProgrammer installation.

If Windows shows "Unknown Device": install Zadig → select STM32 BOOTLOADER → install WinUSB driver.

## Confirming Successful Flash
1. After DFU completes, press NRST
2. If USART1 adapter connected: see firmware version message
3. PC13 LED should blink at 0.5Hz

## SWD Alternative (Better for Development)
DFU is convenient but slow for iteration. For regular development, use ST-Link V2 via SWD:
- PA13 = SWDIO
- PA14 = SWCLK
- GND = GND
- 3.3V = 3.3V

ST-Link allows programming AND debugging (breakpoints, variable watch) — DFU is program-only.
