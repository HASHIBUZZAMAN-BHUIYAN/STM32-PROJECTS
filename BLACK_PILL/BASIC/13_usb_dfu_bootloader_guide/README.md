# Black Pill BASIC/13 — USB DFU Bootloader Guide

## Overview
This is a guide project (no custom firmware needed) explaining the Black Pill's built-in USB DFU (Device Firmware Upgrade) bootloader. The STM32F411 ROM bootloader accepts firmware over USB without any external programmer.

## What is DFU?
DFU is a USB standard for firmware updates. The STM32F411's internal ROM bootloader implements DFU. When activated:
1. The STM32 appears as "STM32 BOOTLOADER" USB device on the PC
2. STM32CubeProgrammer (or dfu-util) can download `.bin`/`.hex`/`.elf` files
3. No ST-Link required

## Entering DFU Mode (Black Pill / WeAct)
1. Hold the **BOOT0** button (labeled "B0" on WeAct board)
2. While holding BOOT0, briefly press and release **NRST** (or plug in USB-C)
3. Hold BOOT0 for ~0.5 seconds after NRST
4. Release BOOT0
5. PC shows new USB device: "STM32 BOOTLOADER" (VID=0483, PID=DF11)

## Exiting DFU Mode
Press **NRST** to reset and run the newly flashed firmware.

## Flashing with STM32CubeProgrammer
1. Open STM32CubeProgrammer
2. Select **USB** connection type (top-right dropdown)
3. Click **Connect** — board must be in DFU mode
4. Go to **Erasing & Programming** (left panel)
5. Browse to your `.elf` or `.hex` file
6. Click **Start Programming**
7. Press NRST to run

## Flashing with dfu-util (Linux/macOS/Windows)
```bash
# List devices
dfu-util -l

# Flash .bin file
dfu-util -a 0 -s 0x08000000:leave -D firmware.bin
```

## Flashing with STM32CubeCLI
```bash
STM32_Programmer_CLI -c port=USB1 -w firmware.elf -g 0x08000000
```

## External ST-Link Alternative
Connect via SWD for a better debugging experience:
| Black Pill | ST-Link V2 |
|-----------|-----------|
| PA13 | SWDIO |
| PA14 | SWCLK |
| GND | GND |
| 3.3V | 3.3V |

## BOOT0 Pin States
| BOOT0 | BOOT1 | Boot from |
|-------|-------|----------|
| 0 | x | User flash (normal) |
| 1 | 0 | System memory (DFU ROM) |
| 1 | 1 | SRAM |

WeAct Black Pill: BOOT0 button pulls to VDD (HIGH). BOOT1 not present on most boards (defaults to LOW via pull-down).

## Demo Firmware
The simplest DFU test firmware is BASIC/01 (blink). Flash it via DFU to verify the process works.
