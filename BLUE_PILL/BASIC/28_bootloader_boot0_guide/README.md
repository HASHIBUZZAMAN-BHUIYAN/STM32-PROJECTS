# 28 — STM32F103 Built-in Bootloader & BOOT0 Guide

## Overview

This is a guide project explaining how the STM32F103's factory ROM bootloader works, how BOOT0 controls boot mode, and how to flash firmware without a debugger using only a USB-to-serial adapter and STM32CubeProgrammer.

The `main_usercode.c` file contains a minimal blink sketch used as a smoke test to confirm that a freshly bootloader-flashed firmware is running correctly.

---

## How the STM32F103 Boot Process Works

### Boot Mode Selection (sampled at reset)

The STM32F103 reads the BOOT0 and BOOT1 pins at the moment of reset (or power-on) to decide where to fetch the reset vector:

| BOOT0 | BOOT1 | Boot Source |
|---|---|---|
| 0 (LOW) | X | User Flash at 0x08000000 — **normal operation** |
| 1 (HIGH) | 0 (LOW) | System Memory (ROM) at 0x1FFFF000 — **factory bootloader** |
| 1 (HIGH) | 1 (HIGH) | Embedded SRAM at 0x20000000 — rarely used |

On the Blue Pill:
- **BOOT0 jumper**: Onboard jumper selects BOOT0. Default position = LOW (0) = User Flash.
- **BOOT1** is PA2 / PB2 on some variants. On the Blue Pill it is typically tied LOW by default.

### Factory ROM Bootloader (System Memory)

- **Address**: 0x1FFFF000 (read-only, programmed at ST factory, cannot be erased)
- **Protocol on STM32F103**: USART1 (PA9/PA10) at auto-baud (typically 115200 8E1 — note **Even parity**)
- The bootloader waits for a 0x7F sync byte on USART1, then responds with 0x79 (ACK).
- After sync, STM32CubeProgrammer issues commands to erase/write/verify Flash.

---

## Flashing Without a Debugger (UART Bootloader Flow)

### Hardware Needed

- Blue Pill board
- USB-to-serial adapter (CP2102, CH340, or FTDI) at 3.3 V
- Two jumper wires

### Step-by-Step

1. **Wire the serial adapter**
   - Adapter TX → Blue Pill PA10 (USART1 RX)
   - Adapter RX → Blue Pill PA9 (USART1 TX)
   - Adapter GND → Blue Pill GND
   - Power the Blue Pill via USB or 5 V pin (not from the adapter)

2. **Enter bootloader mode**
   - Move the **BOOT0 jumper** from 0 → 1 (slide it to the HIGH position).
   - BOOT1 must remain 0 (default on Blue Pill).
   - Press the **RESET** button (or power-cycle). The board now runs the ROM bootloader.

3. **Open STM32CubeProgrammer**
   - Select connection method: **UART**
   - Select the correct COM port
   - Set baud rate: **115200**, Parity: **Even**, Stop Bits: 1
   - Click **Connect** — programmer sends 0x7F, board ACKs with 0x79.

4. **Flash the firmware**
   - Click **Open File**, select your `.hex` or `.bin` file.
   - For `.bin` files set Start Address to **0x08000000**.
   - Click **Download** (Erase → Program → Verify).

5. **Return to normal boot**
   - Move **BOOT0 jumper** back to 0 (LOW position).
   - Press **RESET**. Board boots from user Flash at 0x08000000.
   - The blink smoke-test firmware should run immediately.

---

## SWD vs UART Bootloader — When to Use Which

| Method | Requires | Advantages | Disadvantages |
|---|---|---|---|
| SWD (ST-Link) | ST-Link V2 or clone | Fast, supports debugging, live variable watch | Needs debugger hardware |
| UART Bootloader | USB-serial adapter only | Very cheap — any CP2102/CH340 works | Slower, no debug capability, Even parity quirk |

For production flashing or debugging, use SWD. For classroom/hobbyist use where no debugger is available, UART bootloader is sufficient.

---

## Smoke Test Firmware

`main_usercode.c` contains a simple blink that:
- Blinks the onboard LED (PC13) at 1 Hz (500 ms on / 500 ms off).
- Sends `"Bootloader flash OK - running user code\r\n"` via USART1 at startup.

This lets you confirm within seconds that the bootloader flash succeeded and the MCU is executing from user Flash.

---

## Files

| File | Purpose |
|---|---|
| `main_usercode.c` | Minimal blink + UART smoke test |
| `wiring_notes.md` | BOOT0 jumper positions and UART connections |

## Note

This code has not been tested on hardware.
