# Wiring Notes — 01 Blink LED

## No External Wiring Required

This project uses **only the onboard PC13 LED**. No breadboard, no external components.

---

## Onboard LED Schematic (Simplified)

```
STM32 PC13 ----[R onboard]---- LED Anode
                                LED Cathode ---- 3.3V rail
```

The LED cathode connects to 3.3V through the PCB traces. When PC13 is pulled LOW by the MCU, current flows from 3.3V through the LED and resistor into PC13, turning the LED ON. This is why it is **ACTIVE-LOW**.

---

## Active-Low Truth Table

| PC13 State | HAL Constant | LED State |
|---|---|---|
| LOW (0 V) | `GPIO_PIN_RESET` | **ON** |
| HIGH (3.3 V) | `GPIO_PIN_SET` | OFF |

---

## Power Connections

| Connection | Notes |
|---|---|
| USB-C to host PC | Powers the board at 5 V; onboard LDO regulates to 3.3 V |
| No external 3.3V needed | Powered through USB |

---

## Parts List

| Part | Qty | Specification |
|---|---|---|
| STM32F411CEU6 Black Pill | 1 | WeAct Studio or compatible clone |
| USB-C cable | 1 | USB 2.0 or higher, data-capable |

---

## Safety Notes

> **WARNING — 3.3 V LOGIC**
> All GPIO pins on the STM32F411CEU6 operate at **3.3 V logic levels**.
> Never apply 5 V to any GPIO pin. The MCU is **NOT 5 V tolerant** on most pins.
> Damage will be permanent and not covered under warranty.

- The onboard LED current is limited by the PCB resistor. Do not bypass it.
- USB-C cable must carry data lines (not charge-only) for DFU mode to work.

---

## No Onboard ST-Link

> **NOTE:** The Black Pill board does **NOT** have an onboard ST-Link programmer.
> You cannot flash it with a USB cable alone using OpenOCD's ST-Link backend.
> Use **USB DFU mode** (BOOT0 + NRST procedure) or an **external ST-Link V2** dongle
> connected to PA13 (SWDIO) and PA14 (SWCLK).
