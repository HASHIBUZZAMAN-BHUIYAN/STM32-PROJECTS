# 24 — Window Watchdog (WWDG)

## Overview
Demonstrates the STM32F103 Window Watchdog (WWDG). Unlike the Independent Watchdog (IWDG),
the WWDG enforces both a maximum AND a minimum refresh time. Refreshing too early causes a reset
just as much as refreshing too late.

This project demonstrates two scenarios:
1. **Too-early refresh** — the main loop refreshes the WWDG inside the forbidden window (counter
   still above the window value). The MCU resets almost immediately.
2. **Correct refresh** — the main loop waits until the counter falls into the refresh window,
   then refreshes. The system runs continuously without reset.

Results are indicated on the onboard LED (PC13) and via UART1.

## Hardware
- MCU: STM32F103C8T6 (Blue Pill), 72 MHz
- No external components beyond a USB-TTL adapter on UART1.

## CubeMX Configuration

### WWDG
| Parameter | Value | Notes |
|-----------|-------|-------|
| WWDG State | Enabled | |
| Prescaler | WWDG_PRESCALER_8 | Divides PCLK1 (36 MHz) |
| Window value | 0x50 | Refresh allowed when counter < 0x50 |
| Counter value | 0x7F | Maximum (reset occurs below 0x40) |

**Effective timing at PCLK1 = 36 MHz, PSC = 8:**
- WWDG clock = 36 MHz / 4096 / 8 ≈ 1098 Hz (period ≈ 0.91 ms per count)
- Counter range: 0x7F (127) down to 0x40 (64) = 63 counts → timeout ≈ 57 ms
- Window = 0x50 (80) → refresh allowed only when counter ∈ [0x40, 0x50) → window ≈ 27 ms

### USART1
- Baud: 115200, 8N1

### GPIO
| Pin | Mode |
|-----|------|
| PA9 | USART1_TX |
| PA10 | USART1_RX |
| PC13 | GPIO_Output (active-low LED) |

## WWDG Refresh Rule
```
Refresh is VALID only when:  T[6:0] < Window value (0x50)
Refresh is TOO EARLY when:   T[6:0] >= Window value (0x50)
Refresh is TOO LATE when:    T[6:0] < 0x40 (already reset)
```

## Notes
- This code has NOT been tested on hardware.
- The WWDG cannot be disabled once enabled in software. A hardware reset is required.
- WWDG is stopped during debug halt (if DBGMCU_CR DBG_WWDG_STOP is set via CubeMX debug config).
- Enable "Debug in stop mode" in CubeMX → System Core → SYS → Debug = Serial Wire to prevent
  WWDG resets during step debugging.
