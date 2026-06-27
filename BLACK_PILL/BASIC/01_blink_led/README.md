# 01 - Blink LED (PC13 Active-Low)

## Description

Blinks the onboard LED on **PC13**. The Black Pill LED is **ACTIVE-LOW**: pulling the pin LOW (`GPIO_PIN_RESET`) turns the LED **ON**, pulling it HIGH (`GPIO_PIN_SET`) turns it **OFF**. This is the standard "Hello World" for embedded systems — verifies the toolchain, flashing workflow, and basic GPIO output.

Blink rate: **1 Hz** (500 ms ON, 500 ms OFF).

---

## MCU / Board Facts

| Item | Value |
|---|---|
| MCU | STM32F411CEU6 |
| Core | Cortex-M4F @ 100 MHz |
| Flash | 512 KB |
| RAM | 128 KB |
| Onboard LED | PC13 (ACTIVE-LOW) |
| Logic Voltage | 3.3 V |

---

## CubeMX Configuration

### Clock
- HSE: Crystal/Ceramic Resonator (25 MHz onboard crystal)
- PLL: M=25, N=200, P=4 → SYSCLK = 100 MHz
- APB1 Prescaler: /2 (50 MHz), APB2 Prescaler: /1 (100 MHz)

### GPIO
| Pin | Mode | Pull | Speed | Label |
|---|---|---|---|---|
| PC13 | GPIO_Output | No pull | Low | LED |

- Output type: Push-Pull
- Initial output level: High (LED OFF at startup)

### No peripherals required (no UART, no timers, no ADC).

---

## Parts List

| Part | Qty | Notes |
|---|---|---|
| STM32F411CEU6 Black Pill board | 1 | WeAct or compatible |
| USB-C cable | 1 | For DFU flashing / power |

No external components needed — uses the onboard PC13 LED.

---

## Wiring

None required. The onboard LED is connected internally:

```
MCU PC13 --[onboard resistor]--> LED --> 3.3V rail
```

The LED anode is pulled to 3.3V through an onboard resistor. Driving PC13 LOW sinks current through the LED, turning it ON.

---

## How to Flash (DFU — No ST-Link Needed)

The Black Pill has **no onboard ST-Link**. Use USB DFU mode:

1. Hold the **BOOT0** button (on the board edge).
2. While holding BOOT0, briefly press and release **NRST** (reset button).
3. Wait approximately 0.5 seconds, then release **BOOT0**.
4. The board should appear as **"STM32 BOOTLOADER"** in Device Manager / `lsusb`.
5. Open **STM32CubeProgrammer**, select **USB** mode, click **Connect**.
6. Navigate to your `.elf` or `.hex` file and click **Download**.
7. Press **NRST** to exit DFU mode and run your new firmware.

**Alternative:** External ST-Link V2 via SWD:
- PA13 → SWDIO
- PA14 → SWCLK
- GND → GND
- 3.3V → 3.3V (if powering from ST-Link)

---

## Build Instructions

1. Open STM32CubeMX, configure as above, generate code for **Keil MDK** or **STM32CubeIDE**.
2. Copy the user code sections from `main_usercode.c` into `Core/Src/main.c` at the matching `USER CODE BEGIN/END` markers.
3. Build the project (no errors expected for this simple project).
4. Flash using DFU procedure above.

---

## Expected Behavior

- At power-on: LED is OFF (PC13 driven HIGH by CubeMX init).
- After `MX_GPIO_Init()` runs and the main loop starts: LED toggles every 500 ms.
- Result: LED blinks at **1 Hz** (0.5 s ON, 0.5 s OFF).

> **Note:** Code in this repository has **not been tested on hardware**. Verify on your own board before use in any safety-critical application.
