# Wiring Notes — 15_rtc_basics

## Voltage Warning
> **ALL Blue Pill GPIO pins operate at 3.3 V logic.**
> The UART TX output (PA9) is 3.3 V.
> Most USB-to-UART adapters tolerate 3.3 V input on their RX pin.
> Do NOT drive any Blue Pill GPIO with a 5 V signal without a level shifter.

---

## Pin Connections

### UART1 (time/date output)
| Blue Pill Pin | Signal | USB-UART Adapter | Notes |
|---------------|--------|-----------------|-------|
| PA9 | UART1 TX | RX | 3.3 V logic |
| GND | GND | GND | Common ground |

PA10 (UART1 RX) is unused in this demo.

### Onboard LED (PC13)
| Blue Pill Pin | Function | Notes |
|---------------|----------|-------|
| PC13 | Onboard LED (active-low) | Toggles every second |

No other external connections are required for this project.

---

## RTC Clock Source

### LSI (default — no external crystal)
The Blue Pill board (STM32F103C8T6 minimum system) typically **does not**
populate the 32.768 kHz crystal at positions X1/X2.

This project therefore uses the **internal LSI oscillator (~40 kHz)** as the
RTC clock source.

**LSI accuracy:** approximately ±1–2 % over temperature and voltage.
This equates to roughly ±1–2 minutes per day of drift — acceptable for
demonstration, not suitable for precision timekeeping.

### LSE (external crystal — better accuracy)
If your board has a 32.768 kHz crystal fitted at X1/X2 (marked on PCB):
1. In CubeMX: RCC → LSE → Crystal/Ceramic Resonator.
2. In CubeMX: RTC Clock Source → LSE.
3. The generated `SystemClock_Config` will use `RCC_RTCCLKSOURCE_LSE`.

LSE drift: ±20 ppm typical (~1–2 seconds per day).

---

## PC13 / BKP Quirk — IMPORTANT for this project

Enabling the RTC activates the STM32F1 Backup domain.  The BKP peripheral
includes a tamper/calibration output that **can drive PC13** via the
`BKP->RTCCR` register's ASOE (Anti-Squelch Output Enable) bit.

If the onboard LED (PC13) behaves unexpectedly (always on, flickering, or
ignoring HAL_GPIO_WritePin calls), clear the ASOE bit early in USER CODE:

```c
/* In USER CODE BEGIN 2, BEFORE or AFTER MX_RTC_Init() */
HAL_PWR_EnableBkUpAccess();
BKP->RTCCR &= ~BKP_RTCCR_ASOE;   /* stop RTC driving PC13 */
HAL_PWR_DisableBkUpAccess();
```

This is a one-time write; the bit defaults to 0 on power-on but a previously
flashed project (or a debugger session) may have set it.

---

## Time Persistence Across Resets

The STM32F1 RTC counter continues running as long as VBAT or VDD is supplied.
However, the date (Year/Month/Day) is **not stored in hardware** on the F1 —
it is emulated by the HAL in BKP registers.

On every power cycle with this demo firmware, `RTC_SetInitialDateTime()` is
called unconditionally, resetting the clock to 2025-01-01 12:00:00.

For a production application, use a BKP register as a "time set" flag:
```c
/* Only set time if BKP register 1 is not the magic value */
if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != 0xA5A5)
{
    RTC_SetInitialDateTime();
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0xA5A5);
}
```

---

## CubeMX RTC Settings Checklist

| Setting | Value |
|---------|-------|
| RTC | Activated |
| Clock Source | LSI (or LSE if crystal fitted) |
| Hour Format | 24 Hour |
| Asynchronous Predivider | Auto-computed by CubeMX for LSI |
| Output | None |
| Tamper | Not enabled |

---

## HAL F1 Date/Time Read Order

On the STM32F1 HAL, you **must call `HAL_RTC_GetTime` before `HAL_RTC_GetDate`**
in every read cycle.  Reading Time first causes the hardware to latch the
shadow registers; reading Date second returns consistent values.
Reversing the order returns stale data.

---

## Common Issues

| Symptom | Likely Cause | Fix |
|---------|-------------|-----|
| PC13 LED unresponsive | ASOE bit set in BKP->RTCCR | Clear ASOE as shown above |
| Time resets every boot | No BKP "set" flag | Add BKP_DR1 flag check (see above) |
| Time drifts by minutes/day | Using LSI | Switch to LSE if crystal available |
| HAL_RTC_SetDate returns HAL_ERROR | Backup domain not enabled | Ensure `HAL_PWR_EnableBkUpAccess()` called in SystemClock_Config |
| Date always shows 00/00/00 | GetDate called before GetTime | Always call GetTime first |
