# Wiring Notes — 08_bootloader_concepts

## CRITICAL: 3.3V Logic
**ALL GPIO pins on the STM32F103C8T6 Blue Pill are 3.3V logic.**
Do NOT apply 5V signals to any GPIO pin without a level shifter or voltage divider.

---

## PB0 — Bootloader Entry Button
| Signal | Blue Pill Pin | Component         |
|--------|---------------|-------------------|
| Input  | PB0           | Push button → GND |
| GND    | GND           | Other side of button |

- PB0 is configured as Input with internal Pull-up in CubeMX.
- At reset, if PB0 is held LOW (button pressed to GND), the bootloader stays active.
- If PB0 is HIGH (button released / floating with pull-up), the bootloader jumps to the application.
- A 100 nF decoupling capacitor between PB0 and GND is recommended for debouncing.

### Note on Boot Pins (BOOT0, BOOT1)
- The Blue Pill has a dedicated BOOT0 pin (and a jumper). This project does NOT use the hardware boot mechanism — it uses the bootloader's own PB0 check instead.
- Keep BOOT0 tied LOW (GND, jumper position 0) so the STM32 always boots from flash at 0x08000000 (your bootloader).

---

## UART1 — Bootloader Communication
| Signal | Blue Pill Pin | USB-Serial Adapter |
|--------|---------------|--------------------|
| TX     | PA9           | RX of adapter      |
| RX     | PA10          | TX of adapter      |
| GND    | GND           | GND of adapter     |

- Use a 3.3V USB-to-UART adapter. 5V adapters may damage PA10 if not 5V-tolerant.
- Baud rate: 115200 8N1.
- The bootloader sends status messages over UART1. In an extended implementation this channel also receives firmware binary data.

---

## Onboard LED — PC13
- Active-LOW: PC13 driven LOW = LED ON.
- Bootloader blinks rapidly (100 ms) on error (no valid application found).
- Application blinks at 100 ms to confirm it is running.

---

## Flashing Procedure
### Using STM32CubeProgrammer
1. Connect via SWD (PA13=SWDIO, PA14=SWDCLK, GND, 3.3V from programmer).
2. Flash bootloader:
   - Start address: `0x08000000` (default)
   - File: `bootloader.bin` or `bootloader.hex`
3. Flash application:
   - Start address: `0x08001000`
   - File: `application.bin` or `application.hex`
   - In CubeProgrammer: "Erasing & Programming" tab → set "Start address" to `0x08001000`.

### Using OpenOCD / ST-Link
```bash
# Flash bootloader
openocd -f interface/stlink.cfg -f target/stm32f1x.cfg \
  -c "program bootloader.elf verify reset exit"

# Flash application (preserves bootloader pages)
openocd -f interface/stlink.cfg -f target/stm32f1x.cfg \
  -c "init; reset halt; flash write_image erase application.bin 0x08001000; verify_image application.bin 0x08001000; reset run; exit"
```

---

## SWD Debug Header
| Signal | Blue Pill Pin | ST-Link V2 |
|--------|---------------|------------|
| SWDIO  | PA13          | SWDIO      |
| SWDCK  | PA14          | SWDCLK     |
| GND    | GND           | GND        |
| 3.3V   | 3V3           | 3.3V (optional, if powering from ST-Link) |

---

## Power
- Power the Blue Pill via USB or a 5V bench supply on the 5V pin.
- Do not power from ST-Link 3.3V and USB simultaneously.

---

## Quick-Test Checklist
- [ ] BOOT0 jumper set to 0 (GND position)
- [ ] Push button between PB0 and GND installed
- [ ] UART adapter connected (PA9→RX, PA10←TX, GND)
- [ ] Bootloader flashed at 0x08000000
- [ ] Application flashed at 0x08001000 (linker script updated)
- [ ] `SCB->VTOR = 0x08001000` in application startup (USER CODE BEGIN 2)
- [ ] Serial terminal open at 115200: observe `[BL]` and `[APP]` messages
- [ ] Hold PB0 at reset → stays in bootloader; release → jumps to app (fast blink)
