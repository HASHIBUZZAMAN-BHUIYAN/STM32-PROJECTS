# Black Pill BASIC/23 — Independent Watchdog (IWDG)

## Overview
Configures the IWDG (Independent Watchdog) to reset the MCU if the main loop stalls. The main loop "kicks" the watchdog every second. A UART command `hang` deliberately stalls the loop to trigger a watchdog reset, demonstrating its protective function.

## IWDG Characteristics (F411)
- Clocked by LSI (~32 kHz internal RC)
- LSI frequency varies by temperature/process: 17–47 kHz typical, ~32 kHz nominal
- Prescaler: /4, /8, /16, /32, /64, /128, /256
- 12-bit reload counter: 0–4095
- Timeout = (prescaler × reload) / f_LSI
- Cannot be stopped once started
- Separate from main clock — works in Stop/Standby

## Timeout Calculation
```
// Target: 2 second timeout
// f_LSI ≈ 32000 Hz
// prescaler = 128 → tick = 128/32000 = 4ms per tick
// reload = 500 → timeout = 500 × 4ms = 2000ms = 2s
```
In CubeMX: Prescaler = 128, Reload Value = 500

## IWDG vs WWDG
| Feature | IWDG | WWDG |
|---------|------|------|
| Clock | LSI (independent) | PCLK1 |
| Window | No | Yes (cannot kick too early) |
| Stop in debug | No | No |
| Use for | Max interval watchdog | Exact timing watchdog |

## CubeMX Configuration
| Item | Setting |
|------|---------|
| IWDG | Enable, Prescaler=128, Reload=500 (2s timeout) |
| USART1 | PA9/PA10, 115200 8N1, Global IT |

## Expected Behavior
- Normal: "WDG kick #N" every second — system runs indefinitely
- After `hang` command: stalls, IWDG fires after 2s, reset occurs
- After reset: "Woke from IWDG reset" printed
