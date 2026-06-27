# Wiring Notes — Project 29: Power Measurement Guide

## Voltage Warning

**ALL Blue Pill GPIO pins operate at 3.3 V logic.**
All external connections — including the serial adapter for UART output and the button pull-up — must use 3.3 V logic levels. Never connect a 5 V signal to a GPIO pin.

---

## Power Measurement Setup

### Recommended Connection (3.3 V rail insertion)

```
External 3.3 V (+) ──► [Multimeter mA+ ] ──► [Multimeter COM] ──► Blue Pill 3V3 pin
External 3.3 V (−) ──────────────────────────────────────────────► Blue Pill GND pin
```

1. Set your multimeter to **DC mA** (use the 200 mA range initially, then switch to µA if needed for sleep modes).
2. Do NOT power the Blue Pill via its USB port during measurement — the onboard AMS1117 LDO draws 5–8 mA of quiescent current that inflates readings.
3. Supply 3.3 V directly to the **3V3 pin** (not the 5V pin — using 5V routes through the LDO and adds its quiescent draw).

### What to Observe

| Firmware Phase | Expected Current |
|---|---|
| Startup / UART printing | ~30–35 mA (72 MHz, UART active) |
| Sleep mode (WFI, LED off) | ~5–10 mA (CPU halted, SysTick + peripherals running) |
| 2-second active window after wake | ~25–35 mA |

These figures assume the LED is off (PC13 = HIGH). If the LED is ON, add ~5–7 mA.

---

## Button (PA0)

| Blue Pill Pin | Connection | Notes |
|---|---|---|
| PA0 | One leg of momentary push button | |
| GND | Other leg of push button | PA0 is LOW when pressed |

CubeMX: enable internal Pull-Up on PA0, configure EXTI0 Falling edge trigger. No external resistor required with internal pull-up.

If you add an external pull-up, use **10 kΩ to 3.3 V** — never 5 V.

---

## USART1 Serial Output (Optional but Recommended)

Connecting a serial terminal lets you see the sleep/wake cycle messages, which helps correlate multimeter readings with firmware state.

| Blue Pill Pin | USB-Serial Adapter |
|---|---|
| PA9 (TX) | Adapter RX |
| GND | GND |

PA10 (RX) is not needed — this project only transmits. Adapter must be 3.3 V compatible.

Settings: 115200 baud, 8N1.

---

## Tips for Lower Readings in Sleep Mode

To push sleep current closer to the lower end (~2–3 mA):

1. Disable APB peripheral clocks before WFI:
   ```c
   __HAL_RCC_GPIOA_CLK_DISABLE();  // If PA pins not needed during sleep
   __HAL_RCC_USART1_CLK_DISABLE(); // Disable UART clock during sleep
   ```
   Re-enable after wake before using those peripherals.

2. Disable SysTick before WFI (this means `HAL_Delay` will not work during sleep, which is expected):
   ```c
   HAL_SuspendTick();
   HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
   HAL_ResumeTick();
   ```

3. For Stop mode (~20–80 µA), Stop Standby mode (~2–5 µA), refer to the STM32F103 reference manual sections on low-power modes and the `HAL_PWR_EnterSTOPMode` / `HAL_PWR_EnterSTANDBYMode` functions.
