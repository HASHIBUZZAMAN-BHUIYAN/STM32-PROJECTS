# 02 - Push Button (GPIO Input with Debounce)

## Description

Reads an external push button connected to **PA0** with the internal pull-up resistor enabled. When the button is pressed (PA0 reads LOW), the onboard LED on **PC13** toggles. A **50 ms software debounce** using `HAL_GetTick()` prevents multiple triggers from a single button press due to contact bounce.

---

## MCU / Board Facts

| Item | Value |
|---|---|
| MCU | STM32F411CEU6 |
| Core | Cortex-M4F @ 100 MHz |
| Button Input | PA0 (active-low with internal pull-up) |
| LED Output | PC13 (active-low) |
| Logic Voltage | 3.3 V |

---

## CubeMX Configuration

### Clock
- HSE: 25 MHz crystal → PLL → SYSCLK = 100 MHz (same as project 01)

### GPIO

| Pin | Mode | Pull | Speed | Label |
|---|---|---|---|---|
| PA0 | GPIO_Input | Pull-up | — | BTN |
| PC13 | GPIO_Output | No pull | Low | LED |

- PA0: Pull-up enabled internally (no external resistor required)
- PC13: Push-Pull output, initial level High (LED OFF)

---

## Parts List

| Part | Qty | Notes |
|---|---|---|
| STM32F411CEU6 Black Pill board | 1 | |
| Tactile push button (SPST NO) | 1 | Any standard 6×6 mm tact switch |
| USB-C cable | 1 | For DFU flashing / power |
| Breadboard | 1 | Optional, for easy wiring |
| Jumper wires | 2 | Male-to-male |

No external pull-up/pull-down resistors needed — internal pull-up is used.

---

## Wiring Table

| Black Pill Pin | Connect To | Notes |
|---|---|---|
| PA0 | Button pin 1 | GPIO input with internal pull-up |
| GND | Button pin 2 | Other side of button to ground |

Button type: **normally open** (SPST NO). When pressed, PA0 is pulled to GND → reads LOW.

```
Black Pill PA0 ---+--- [Button] --- GND
                  |
              (internal 40kΩ pull-up to 3.3V inside MCU)
```

---

## How to Flash (DFU — No ST-Link Needed)

1. Hold the **BOOT0** button.
2. While holding BOOT0, briefly press and release **NRST**.
3. Wait ~0.5 s, then release **BOOT0**.
4. Board appears as **"STM32 BOOTLOADER"** USB device.
5. Open **STM32CubeProgrammer** → USB mode → Connect → Download `.elf`/`.hex`.
6. Press **NRST** to run firmware.

**Alternative:** External ST-Link V2 → PA13 (SWDIO), PA14 (SWCLK), GND, 3.3V.

---

## Build Instructions

1. Generate CubeMX project with the GPIO config above.
2. Insert user code sections from `main_usercode.c` into `Core/Src/main.c`.
3. Build and flash.

---

## Expected Behavior

- LED (PC13) is OFF at startup.
- Each **press** of the button toggles the LED ON or OFF.
- Rapid multiple presses within 50 ms are ignored (debounce).
- The LED state is latched — it stays ON or OFF until the next valid press.

> **Note:** Code in this repository has **not been tested on hardware**.
