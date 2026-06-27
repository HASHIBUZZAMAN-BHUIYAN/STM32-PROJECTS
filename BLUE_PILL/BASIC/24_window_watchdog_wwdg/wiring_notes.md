# Wiring Notes — 24: Window Watchdog (WWDG)

## CRITICAL: 3.3 V Logic
All STM32F103 GPIO pins operate at **3.3 V**. Do not connect any 5 V signal directly to any GPIO pin.

## Overview
This project requires no external components beyond a USB-TTL serial adapter.
The WWDG is entirely on-chip.

## Pin Connections

| Blue Pill Pin | USB-TTL Adapter | Notes |
|---------------|-----------------|-------|
| PA9 (TX) | RX | UART1 transmit |
| PA10 (RX) | TX | UART1 receive (optional) |
| GND | GND | Common ground |
| PC13 | — | Onboard LED (active-low, no external wiring) |

## UART1 (USB-TTL Adapter)
Terminal settings: **115200 baud, 8N1, no flow control**.

USB-TTL adapter logic level must be **3.3 V**.

## WWDG Behaviour to Expect on UART

**Boot 1 (too-early scenario):**
```
=== WWDG Demo — Power-on / SW Reset ===
Mode: EARLY refresh (expect WWDG reset in ~1 ms)
Window=0x50  Counter=0x7F
WWDG started.
Refreshing WWDG too early...
*** WWDG Reset detected! ***        <-- next boot starts here
Mode: CORRECT refresh (expect normal operation)
...
WWDG refreshed in window. System OK.
WWDG refreshed in window. System OK.
...
```

## Debugging WWDG with ST-Link
- WWDG resets the MCU during debug halts unless `DBG_WWDG_STOP` is configured.
- In CubeMX: **System Core → SYS → Debug = Serial Wire** and enable
  **"Stop WWDG counter when CPU is halted"** (sets DBGMCU_CR.DBG_WWDG_STOP).
- Without this, any breakpoint will trigger an unwanted WWDG reset.

## Backup Register (BKP) Dependency
- The demo uses `BKP->DR1` to alternate between scenarios across resets.
- CubeMX must enable: **RCC → Peripheral Clock → BKP** and **PWR**.
- The BKP domain is powered by VDD (not VBAT in this application), so the
  register value survives a system reset but NOT a full power cycle.
- On a cold power-on, `BKP->DR1` may read 0x0000, which selects the "good"
  scenario. After one reset cycle the too-early scenario will run.

## Power
- Power the Blue Pill from USB.
- Do not power-cycle between resets when testing the WWDG reset scenario —
  the BKP register must retain its value.
