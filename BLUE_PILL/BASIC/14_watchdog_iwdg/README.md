# 14 — Independent Watchdog (IWDG)

## Overview
Demonstrates the STM32 Independent Watchdog (IWDG).  The IWDG runs on the
internal ~40 kHz LSI oscillator and is independent of the main clock — it
cannot be stopped by software once started.

A ~2-second timeout is configured.  The main loop refreshes the watchdog and
blinks the LED slowly.  If the refresh is disabled (comment out
`HAL_IWDG_Refresh`), the MCU resets within 2 seconds.  The RCC reset-source
flags are checked at startup and printed via UART so the cause of each reset is
visible.

## Hardware
- Board : STM32F103C8T6 (Blue Pill)
- LED   : PC13 (onboard, active-low)
- UART  : USART1, PA9 TX, 115200 8N1

## CubeMX Settings
| Peripheral | Setting |
|------------|---------|
| IWDG | Enabled |
| IWDG Prescaler | /32 |
| IWDG Reload | 2499 |
| USART1 | 115200 8N1, TX = PA9 |
| SYS | SysTick as timebase |

### Timeout Calculation
```
LSI ≈ 40 000 Hz
Tick period = Prescaler / LSI = 32 / 40000 = 0.8 ms
Timeout = (Reload + 1) × tick = 2500 × 0.8 ms = 2000 ms ≈ 2 s
```

## Files
| File | Purpose |
|------|---------|
| `main_usercode.c` | IWDG refresh loop, reset-source detection |
| `wiring_notes.md` | Pin connections and hardware caveats |

## Demonstration
- Normal: LED blinks every 500 ms; UART prints "IWDG OK" every loop.
- Watchdog trigger: comment out `HAL_IWDG_Refresh(&hiwdg);` in the loop,
  reflash, observe reset every ~2 s; UART shows "** IWDG RESET **" on restart.

## Note
Code is provided as a reference template. It has not been tested on hardware.
