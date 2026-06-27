# Wiring Notes — 02 Push Button

## Pin Connections

| Black Pill Pin | Connects To | Wire Colour (suggestion) |
|---|---|---|
| PA0 | Button terminal 1 | Yellow |
| GND | Button terminal 2 | Black |
| 5V / USB-C | Power only | — |

No external pull-up or pull-down resistors are needed. The internal pull-up is enabled in CubeMX.

---

## Breadboard Layout (Conceptual)

```
Black Pill
  PA0 --------+----[BUTTON]---- GND (Black Pill)
              |
          (internal 40kΩ pull-up inside STM32 to 3.3V)

Button open  → PA0 = 3.3V (HIGH, GPIO_PIN_SET)  → no action
Button closed → PA0 = 0V  (LOW,  GPIO_PIN_RESET) → toggle LED
```

---

## Parts List

| Part | Qty | Specification |
|---|---|---|
| STM32F411CEU6 Black Pill | 1 | WeAct Studio or compatible |
| Tactile push button (SPST NO) | 1 | 6×6 mm standard tact switch |
| USB-C cable | 1 | Data-capable |
| Breadboard | 1 | Half-size or larger |
| Jumper wires | 2 | Male-to-male |

---

## Safety Notes

> **WARNING — 3.3 V LOGIC**
> The STM32F411CEU6 GPIO pins are **3.3 V logic**. Do **NOT** connect the button
> to a 5 V supply rail. Only connect one button terminal to PA0 and the other to **GND**.
> Applying 5 V to PA0 will permanently damage the MCU.

- The internal pull-up is approximately 40 kΩ. This is weak enough that mechanical bounce can produce multiple edges per press — the 50 ms software debounce in firmware handles this.
- Do not leave PA0 floating (unconnected) in a real application — the internal pull-up prevents floating in this design.
- Button terminals on a 6×6 mm tact switch: pins 1-2 are shorted together on one side, pins 3-4 on the other. Connect across the two sides (not the same side).

---

## No Onboard ST-Link

> **NOTE:** The Black Pill does **NOT** include an onboard ST-Link programmer.
> To flash firmware, use **USB DFU mode** (BOOT0 + NRST sequence) with
> STM32CubeProgrammer, or connect an **external ST-Link V2** to:
> - PA13 → SWDIO
> - PA14 → SWCLK
> - GND  → GND
> - 3.3V → 3.3V (optional, only if powering from ST-Link)
