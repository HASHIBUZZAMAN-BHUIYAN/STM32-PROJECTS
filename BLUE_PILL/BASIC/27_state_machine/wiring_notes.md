# Wiring Notes — Project 27: Button-Driven State Machine

## Voltage Warning

**ALL Blue Pill GPIO pins operate at 3.3 V logic.**
Do NOT connect 5 V signals directly to any STM32 pin. If your button circuit uses an external pull-up, use a 3.3 V rail, not 5 V.

---

## Connections

### Onboard LED (PC13)

No external wiring needed. PC13 drives the onboard LED with an active-low polarity:
- `GPIO_PIN_RESET` (PC13 = LOW) → LED ON
- `GPIO_PIN_SET` (PC13 = HIGH) → LED OFF

### Button (PA0)

| Blue Pill Pin | Button Connection | Notes |
|---|---|---|
| PA0 | One side of momentary button | |
| GND | Other side of momentary button | PA0 pulled LOW when pressed |

CubeMX must enable the **internal Pull-Up** on PA0 so the pin reads HIGH when the button is released. Configure EXTI0 for **Falling edge** trigger (HIGH → LOW transition on press).

External wiring option if internal pull-up is not used:

```
3.3V ---[10kΩ]--- PA0 ---[button]--- GND
```

Do NOT use a 5 V pull-up — this would exceed the 3.3 V GPIO maximum input voltage.

---

## State Summary (for testing)

Each button press advances the state:

1. Power-on → **IDLE**: LED solid ON
2. Press 1 → **BLINK_SLOW**: LED 500 ms on / 500 ms off
3. Press 2 → **BLINK_FAST**: LED 100 ms on / 100 ms off
4. Press 3 → **OFF**: LED solid OFF
5. Press 4 → back to **IDLE**

---

## Debounce

Software debounce of 50 ms is implemented in the main loop. Mechanical buttons typically bounce for 5–20 ms, so 50 ms provides a comfortable margin. No hardware RC filter is strictly required, but adding a 100 nF capacitor between PA0 and GND is good practice for noisy environments.

---

## No RTOS Required

This project uses HAL_GetTick() for non-blocking timing. All logic runs in the main while(1) loop. No FreeRTOS or scheduler is needed.
