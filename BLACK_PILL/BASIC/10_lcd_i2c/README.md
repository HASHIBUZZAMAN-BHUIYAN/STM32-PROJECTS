# Project 10 — 16x2 LCD via I2C (PCF8574 Backpack)

## Board
**STM32F411CEU6 Black Pill** (WeAct Studio)
- 100 MHz Cortex-M4F, 512 KB Flash, 128 KB RAM
- Onboard LED: PC13 (active-low)
- No onboard ST-Link — DFU or external ST-Link required

## Project Description
Drives a standard HD44780-compatible 16x2 character LCD through a **PCF8574 I2C I/O expander backpack**.
The PCF8574 converts 8-bit I2C data to 8 parallel GPIO lines that control the LCD.

This project implements a complete bare-metal LCD driver using only `HAL_I2C_Master_Transmit()` —
no external libraries needed.

Displayed content:
- Line 1: `Black Pill F411`
- Line 2: `I2C LCD Test`

## PCF8574 Bit Mapping
The LCD backpack wires the PCF8574 port pins to the LCD as follows:

| PCF8574 Pin | LCD Signal | Notes                           |
|-------------|------------|---------------------------------|
| P0          | RS         | Register Select (0=cmd, 1=data) |
| P1          | RW         | Read/Write (always 0 = write)   |
| P2          | EN         | Enable strobe                   |
| P3          | BL         | Backlight (1=on)                |
| P4          | D4         | Data bit 4                      |
| P5          | D5         | Data bit 5                      |
| P6          | D6         | Data bit 6                      |
| P7          | D7         | Data bit 7                      |

The LCD is driven in **4-bit mode** — only D4-D7 are used, data is sent as two nibbles.

## Hardware Required
- STM32F411 Black Pill
- 16x2 LCD with PCF8574 I2C backpack (very common on Amazon/AliExpress)
- Common PCF8574 backpack addresses: **0x27** (A2=A1=A0=1) or **0x3F** (PCF8574A chip)
- External 4.7 kΩ pull-up resistors if not already on backpack PCB

## Pin Assignments
| Signal | Black Pill Pin | Notes                            |
|--------|---------------|----------------------------------|
| SCL    | PB6           | I2C1 SCL — backpack pulls up     |
| SDA    | PB7           | I2C1 SDA — backpack pulls up     |
| GND    | GND           | Common ground                    |
| 5V     | 5V (VBUS)     | LCD backlight needs 5V; see note |
| 3.3V   | 3V3           | PCF8574 VCC (can run at 3.3V)   |

> **Voltage Note:** Most LCD backpacks work with the PCF8574 at 3.3 V I2C logic but the LCD contrast
> and backlight are better at 5 V. Connect LCD VCC to the 5 V (VBUS) pin on the Black Pill.
> The PCF8574 I2C lines remain at 3.3 V — this is safe because the PCF8574 is 5V-tolerant on its
> I2C pins when powered at 3.3 V. Alternatively, power the entire backpack at 3.3 V and adjust
> the contrast potentiometer fully.

## CubeMX Configuration

### Clock
- Input: HSE 25 MHz crystal
- PLL: PLLM=25, PLLN=200, PLLP=2 → SYSCLK = 100 MHz
- APB1 Prescaler: /2 → 50 MHz

### I2C1
- Mode: I2C
- Speed Mode: **Fast Mode**
- Clock Speed: **400000 Hz (400 kHz)**
- Pins: PB6 = I2C1_SCL, PB7 = I2C1_SDA
- Rise Time: 100 ns (auto-calculated)
- Fall Time: 10 ns (auto-calculated)

### GPIO
- PC13: GPIO Output (onboard LED)

### NVIC
- No interrupts required (polling mode)

## LCD I2C Address
Default PCF8574 address: **0x27**
If your backpack uses PCF8574**A** chip, address is **0x3F**
Change the `LCD_I2C_ADDR` define in the source code to match your hardware.
Use Project 09 (I2C Bus Scanner) to discover the actual address.

## How to Flash (USB DFU)
1. Hold the **BOOT0** button on the board
2. While holding BOOT0, briefly press and release **NRST**
3. Wait approximately 0.5 seconds, then release BOOT0
4. The board enumerates as **"STM32 BOOTLOADER"** on your PC's USB
5. Open **STM32CubeProgrammer**, select **USB** connection, click **Connect**
6. Go to **Erasing & Programming**, browse to your `.elf` or `.hex` file
7. Click **Start Programming**
8. After programming completes, press **NRST** to exit DFU and run firmware

**Alternative:** External ST-Link V2 via SWD
- PA13 = SWDIO, PA14 = SWCLK, GND, 3.3V

## Expected Behaviour
After power-on (allow ~500 ms for LCD init):
```
+----------------+
|Black Pill F411 |
|I2C LCD Test    |
+----------------+
```
The onboard LED (PC13) blinks at 1 Hz as a heartbeat indicator.

## Troubleshooting
- **All blocks / no text:** I2C address wrong — try 0x3F instead of 0x27
- **Garbled text:** Timing issue — increase `HAL_Delay` values in init sequence
- **No backlight:** BL bit not set in driver; check `LCD_BACKLIGHT` define
- **Contrast too low:** Adjust the small potentiometer on the backpack PCB
- **I2C stuck:** Check pull-up resistors; Fast Mode needs 2.2 kΩ or lower for long cables
