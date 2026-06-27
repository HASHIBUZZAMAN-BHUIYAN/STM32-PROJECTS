# 29 — Power Measurement Guide

## Overview

This guide project explains how to measure the current draw of the Blue Pill in Active mode (72 MHz running) vs Sleep mode (WFI with peripherals off). The `main_usercode.c` file provides functional sleep-mode firmware: the MCU enters sleep, wakes on a button press (PA0 EXTI), prints its state via USART1, then goes back to sleep.

---

## Why Measure Power?

Understanding current consumption is critical for:
- Battery-powered designs (estimating runtime)
- Confirming sleep mode is actually working
- Identifying leaky peripherals that draw unexpected current

---

## How to Measure Current on the Blue Pill

### Method: Multimeter in Series with the 3.3 V Supply

The most accurate method for the Blue Pill is to break the 3.3 V supply path and insert a multimeter in series (set to DC mA mode).

#### Step-by-Step

1. **Do NOT use the USB port for power during measurement.**
   USB has a 3.3 V LDO onboard that draws its own quiescent current (~5–8 mA), which inflates your reading.

2. **Supply 3.3 V directly to the 3V3 pin** from a bench supply or a known-good LDO external to the board:
   ```
   External 3.3V supply (+) --> [multimeter A+ / mA] --> [multimeter A- / COM] --> Blue Pill 3V3 pin
   External 3.3V supply (-) --> Blue Pill GND pin
   ```

3. Set multimeter to **DC mA** range (200 mA range or auto-range).

4. The multimeter now measures all current flowing through the 3.3 V rail into the MCU and any onboard 3.3 V peripherals.

#### Isolating the MCU Only

The Blue Pill's onboard LED (PC13) draws ~5–7 mA when lit. For accurate MCU-only readings:
- Keep the LED off during measurement (drive PC13 HIGH in firmware before entering sleep).
- The USB-C connector pull resistors also add a small static draw. Disconnect USB data lines if present.

---

## Cutting the BOOT0 Jumper Trace (Optional Power Isolation)

The BOOT0 jumper has a small resistor/trace that connects it to the supply rail in some Blue Pill variants. If you are trying to measure absolute minimum current:

- Inspect the BOOT0 jumper pads under a magnifier.
- If there is a 0-ohm resistor or a PCB trace shorting BOOT0 to a pull-down resistor network, you can remove/cut it.
- This is an advanced modification and is **not required** for basic current measurement.
- For typical hobbyist measurement, leaving BOOT0 at LOW (normal boot position) and measuring at the 3V3 pin is sufficient.

---

## Expected Current Draw (STM32F103C8T6 at 3.3 V)

These are typical values from the STM32F103 datasheet (Table 21–22). Actual measurements will vary with voltage, temperature, and peripheral state.

| Mode | Typical Current | Notes |
|---|---|---|
| Run mode at 72 MHz (HSE PLL) | 25–35 mA | All peripherals clocked, Flash prefetch on |
| Run mode at 8 MHz (HSI) | 8–12 mA | No PLL |
| Sleep mode (WFI, 72 MHz base) | 5–10 mA | CPU halted, peripherals still clocked |
| Sleep mode (peripherals gated off) | 2–5 mA | Disable APB/AHB clocks in firmware |
| Stop mode | ~20–80 µA | Only LSI/RTC running; wakes on EXTI |
| Standby mode | ~2–5 µA | Only RTC and Backup domain running |

This project implements **Sleep** mode (WFI), not Stop or Standby. Sleep halts the Cortex-M3 CPU but keeps peripheral clocks running. To reach the lower Sleep numbers, disable unused peripheral clocks in `main_usercode.c` before calling `__WFI()`.

---

## Sleep Mode Implementation

`main_usercode.c` demonstrates the cycle:
1. Print `"Entering sleep\r\n"` via USART1.
2. Flush UART TX (wait for transmission to complete).
3. Drive PC13 HIGH (LED off — reduces current draw).
4. Call `HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI)`.
5. CPU halts here until EXTI0 (PA0 button) fires an interrupt.
6. On wake: print `"Woke from sleep\r\n"`, blink LED once, repeat.

---

## CubeMX Configuration

| Peripheral | Setting |
|---|---|
| PC13 | GPIO Output, Push-Pull |
| PA0 | GPIO_EXTI0, Pull-Up, Falling edge trigger |
| NVIC | EXTI0 enabled |
| USART1 | Asynchronous, 115200, 8N1 |
| Clock | HSE 8 MHz → PLL → 72 MHz |
| PWR | No special CubeMX setting needed for Sleep; Stop/Standby require PWR peripheral enabled |

---

## Files

| File | Purpose |
|---|---|
| `main_usercode.c` | Sleep/wake firmware with UART state reporting |
| `wiring_notes.md` | Measurement setup and voltage notes |

## Note

This code has not been tested on hardware.
