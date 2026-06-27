# Wiring Notes — Project 28: Bootloader BOOT0 Guide

## Voltage Warning

**ALL Blue Pill GPIO pins operate at 3.3 V logic.**
The USB-to-serial adapter used for UART bootloading MUST have 3.3 V TX/RX lines. A 5 V adapter will damage the STM32 GPIO pins. CP2102 and CH340 modules often have a voltage selection jumper — confirm it is set to 3.3 V before connecting.

---

## BOOT0 Jumper Positions

The Blue Pill has two jumpers near the micro-USB connector labeled BOOT0 and BOOT1.

```
BOOT1  BOOT0
  |      |
 [0]    [0]   <- Normal: boots from User Flash (0x08000000)
 [0]    [1]   <- Bootloader: boots from ROM at 0x1FFFF000
```

To enter ROM bootloader mode:
1. Slide BOOT0 jumper to position **1** (HIGH).
2. Keep BOOT1 jumper at position **0** (LOW) — default.
3. Press RESET or power-cycle.

To return to normal operation after flashing:
1. Slide BOOT0 jumper back to position **0** (LOW).
2. Press RESET.

---

## UART Connections for Bootloading

| Blue Pill Pin | USB-Serial Adapter | Notes |
|---|---|---|
| PA9 (USART1 TX) | Adapter RX | Crossover connection |
| PA10 (USART1 RX) | Adapter TX | Crossover connection |
| GND | GND | Common ground — mandatory |

Power the Blue Pill separately via its USB port or 5 V pin. Do NOT power it from the serial adapter's 3.3 V or 5 V rail — current capability is usually insufficient.

---

## STM32CubeProgrammer UART Settings

| Parameter | Bootloader Mode | Normal User UART |
|---|---|---|
| Baud rate | 115200 | 115200 |
| Data bits | 8 | 8 |
| Parity | **Even (E)** | None (N) |
| Stop bits | 1 | 1 |

The factory bootloader uses **8E1** (Even parity). This is a common source of confusion — if you connect at 8N1, the programmer will not receive ACK from the bootloader. Switch back to 8N1 in your terminal after flashing for normal UART communication with user firmware.

---

## SWD Debug Port (Alternative Flashing)

If you have an ST-Link V2:

| Blue Pill Pin | ST-Link V2 Pin |
|---|---|
| SWDIO (PA13) | SWDIO |
| SWCLK (PA14) | SWCLK |
| GND | GND |
| 3.3V | 3.3V (optional — can power the board) |

SWD does not require changing the BOOT0 jumper. It works regardless of boot mode.

---

## No External Peripherals

This smoke test only uses:
- Onboard LED (PC13) — no wiring needed
- USART1 (PA9/PA10) — USB-serial adapter only
