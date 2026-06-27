# 15 — Internal RTC Basics

## Overview
Configures the STM32F103 internal RTC and reads the current time/date once per
second, formatting the result as a human-readable string and transmitting it
over UART1.

The Blue Pill typically lacks an external 32.768 kHz crystal (LSE), so the
**LSI (~40 kHz)** oscillator is used as the RTC clock source.  LSI is less
accurate than LSE (~1–2 % drift) but requires no external component.

## Hardware
- Board : STM32F103C8T6 (Blue Pill)
- UART  : USART1, PA9 TX, 115200 8N1

## CubeMX Settings
| Peripheral | Setting |
|------------|---------|
| RTC | Activated |
| RTC Clock Source | LSI (internal ~40 kHz) |
| USART1 | 115200 8N1, TX = PA9 |
| SYS | SysTick as timebase |

> **LSE note:** If your Blue Pill board has a 32.768 kHz crystal fitted to
> X1/X2, switch the RTC clock source to LSE in CubeMX for better accuracy.
> Change `RCC_RTCCLKSOURCE_LSI` → `RCC_RTCCLKSOURCE_LSE` in the generated
> `SystemClock_Config`.

## Files
| File | Purpose |
|------|---------|
| `main_usercode.c` | RTC init, time/date set, read loop, UART print |
| `wiring_notes.md` | Pin connections and hardware caveats |

## Initial Time / Date
Set in USER CODE Init:
- Time : 12:00:00
- Date : Wednesday 2025-01-01

## Output (UART1, 115200)
```
2025-01-01  12:00:01
2025-01-01  12:00:02
...
```

## PC13 / BKP Quirk
If PC13 (onboard LED) misbehaves after enabling RTC, clear the ASOE bit in
`BKP->RTCCR` to prevent the RTC from driving a calibration signal on PC13.
See `wiring_notes.md` for details.

## Note
Code is provided as a reference template. It has not been tested on hardware.
