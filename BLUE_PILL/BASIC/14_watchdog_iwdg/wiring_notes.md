# Wiring Notes — 14_watchdog_iwdg

## Voltage Warning
> **ALL Blue Pill GPIO pins operate at 3.3 V logic.**
> The UART TX output (PA9) is 3.3 V.
> If your USB-to-UART adapter is 5 V tolerant on its RX input (most are),
> no level shifting is needed.  Do NOT connect a 5 V signal to any Blue Pill
> GPIO pin without a level shifter.

---

## Pin Connections

### UART1 (optional — for monitoring reset source)
| Blue Pill Pin | Signal | USB-UART Adapter | Notes |
|---------------|--------|-----------------|-------|
| PA9 | UART1 TX | RX | 3.3 V logic |
| GND | GND | GND | Common ground |

PA10 (RX) is not used in this demo — only TX is needed to observe output.

### Onboard LED (PC13)
| Blue Pill Pin | Function | Notes |
|---------------|----------|-------|
| PC13 | Onboard LED (active-low) | Blinks every 500 ms when IWDG is fed |

No other external connections are required for this project.

---

## IWDG Timeout Calculation

The IWDG clock is derived from the internal **LSI oscillator (~40 kHz)**.
LSI frequency can vary between 30–60 kHz across temperature and MCU samples.
The timeout is approximate.

```
Timeout = (Reload + 1) × (Prescaler / LSI_Hz)
        = (2499 + 1) × (32 / 40000)
        = 2500 × 0.0008 s
        = 2.0 s  (nominal)

Worst case (LSI = 30 kHz): 2500 × (32/30000) ≈ 2.67 s
Worst case (LSI = 60 kHz): 2500 × (32/60000) ≈ 1.33 s
```

Adjust Prescaler and Reload in CubeMX if a more precise timeout is needed.
Available prescalers: /4, /8, /16, /32, /64, /128, /256.

---

## IWDG Behaviour

| Condition | Result |
|-----------|--------|
| HAL_IWDG_Refresh() called within timeout | MCU continues normally |
| HAL_IWDG_Refresh() NOT called in time | MCU resets; RCC_FLAG_IWDGRST set |
| Once started, IWDG cannot be stopped | True — even in debug mode by default |

### Disabling IWDG during debug (optional)
In CubeMX → Debug → DBG_IWDG_STOP: enabled.
This pauses the IWDG counter when the core is halted at a breakpoint,
preventing spurious resets during a debug session.
In CubeMX: System Core → DBGMCU → DBG_IWDG_STOP → Enabled.

---

## Demonstration Procedure

1. Flash the firmware with `HAL_IWDG_Refresh(&hiwdg)` **active**.
   - LED blinks every 500 ms.
   - UART shows "IWDG OK  loop=N".

2. Comment out `HAL_IWDG_Refresh(&hiwdg)`, rebuild, and reflash.
   - LED blinks once, pauses ~2 s, then restarts — repeating.
   - UART shows "Reset cause: ** IWDG RESET **" on each boot.

---

## Common Issues

| Symptom | Likely Cause | Fix |
|---------|-------------|-----|
| MCU resets immediately on boot | IWDG starts too early; code too slow | Check that MX init order puts IWDG last, or reduce init overhead |
| MCU resets during debugger halt | IWDG not paused in debug | Enable DBG_IWDG_STOP in CubeMX DBGMCU settings |
| UART shows wrong reset cause | Flags not cleared in previous run | Call `__HAL_RCC_CLEAR_RESET_FLAGS()` after reading |
| Reset cause shows "Unknown" | No flags set (first power-on) | POR and reset-pin flags are not always reliable after debugger flash |

---

## PC13 / BKP Quirk
This project does not use the RTC.  PC13 should function normally as an
active-low LED.  If PC13 misbehaves (e.g., driven by RTC calibration output
from a previously-flashed project), clear the ASOE bit:
```c
HAL_PWR_EnableBkUpAccess();
BKP->RTCCR &= ~BKP_RTCCR_ASOE;
HAL_PWR_DisableBkUpAccess();
```
