# 03 - PWM LED Fade (TIM1 CH1 on PA8)

## Description

Generates a **PWM signal on PA8** using **TIM1 Channel 1** in PWM Mode 1. An external LED connected through a current-limiting resistor fades smoothly from 0% to 100% duty cycle and back, creating a breathing effect. The PWM frequency is **1 kHz** at 100 MHz system clock with PSC=99, ARR=999.

---

## MCU / Board Facts

| Item | Value |
|---|---|
| MCU | STM32F411CEU6 |
| Core | Cortex-M4F @ 100 MHz |
| PWM Pin | PA8 (TIM1 CH1, alternate function AF1) |
| PWM Frequency | 1 kHz |
| Duty Resolution | 0–1000 (0.1% steps) |
| Logic Voltage | 3.3 V |

---

## PWM Frequency Calculation

```
f_PWM = f_SYSCLK / (PSC + 1) / (ARR + 1)
f_PWM = 100,000,000 / (99 + 1) / (999 + 1)
f_PWM = 100,000,000 / 100 / 1000
f_PWM = 1,000 Hz  (1 kHz)
```

Duty cycle = CCR / (ARR + 1) = CCR / 1000
- CCR = 0    → 0% duty
- CCR = 500  → 50% duty
- CCR = 1000 → 100% duty

---

## CubeMX Configuration

### Clock
- HSE: 25 MHz → PLL → SYSCLK = 100 MHz

### Timers

**TIM1:**
| Parameter | Value |
|---|---|
| Channel 1 | PWM Generation CH1 |
| Prescaler (PSC) | 99 |
| Counter Period (ARR) | 999 |
| PWM Mode | PWM Mode 1 |
| Pulse (CCR1) | 0 (start at 0%) |
| CH Polarity | High |
| Auto-reload preload | Enable |

### GPIO (auto-configured by CubeMX when TIM1 CH1 enabled)

| Pin | Mode | Alternate Function |
|---|---|---|
| PA8 | Alternate Function Push-Pull | AF1 (TIM1_CH1) |

### PC13 (optional — can use for status indication)
| Pin | Mode | Pull | Label |
|---|---|---|---|
| PC13 | GPIO_Output | No pull | LED_STATUS |

---

## Parts List

| Part | Qty | Notes |
|---|---|---|
| STM32F411CEU6 Black Pill board | 1 | |
| External LED (any colour) | 1 | 3 mm or 5 mm standard LED |
| 330 Ω resistor | 1 | 1/4 W, limits LED current to ~10 mA |
| USB-C cable | 1 | Power + DFU flashing |
| Breadboard | 1 | |
| Jumper wires | 2 | |

**Alternative:** Connect PA8 to an oscilloscope to observe the PWM waveform without an LED.

---

## Wiring Table

| Black Pill Pin | Connects To | Notes |
|---|---|---|
| PA8 | 330 Ω resistor (pin 1) | PWM output, 3.3 V max |
| 330 Ω resistor (pin 2) | LED Anode (+) | Current limiter |
| LED Cathode (−) | GND | Complete the circuit |
| GND | GND rail | Common ground |

```
PA8 ---[330Ω]---[LED+  LED-]--- GND
```

LED forward voltage: ~2.0 V (red) to ~3.0 V (blue/white).
With 3.3 V supply and 330 Ω resistor, current ≈ (3.3 − 2.0) / 330 ≈ 4 mA (safe).

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

1. Generate CubeMX project with TIM1 CH1 PWM configured as above.
2. Insert user code sections from `main_usercode.c` into `Core/Src/main.c`.
3. Build and flash.

---

## Expected Behavior

- LED fades **from dark to full brightness** over approximately 1 second.
- Then fades **from full brightness back to dark** over approximately 1 second.
- Continuous breathing/fade loop.
- Each step changes duty by 1% (CCR += 10 per 10 ms = 101 steps × 10 ms ≈ 1 s per direction).

> **Note:** Code in this repository has **not been tested on hardware**.
