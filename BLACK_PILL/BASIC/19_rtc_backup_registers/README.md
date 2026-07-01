# Black Pill BASIC/19 — RTC + Backup Registers

## Overview
Demonstrates the STM32F411's RTC peripheral and 20 backup registers. Sets the RTC time/date on first boot, reads it back on every boot, and uses a backup register to count total reset events. The backup domain is powered by VBAT (or VDD) and survives system resets and power-off (with battery).

## STM32F411 RTC Features
- Shadow registers (must read Date after Time to unlock)
- 20 backup registers (RTC_BKP_DR0 … DR19)
- Alarms A and B
- Wakeup timer
- Tamper detection pins

## Critical F4 RTC Read Pattern
```c
// ALWAYS read Date after Time — F4 RTC has shadow registers
// that only update after you read both Time and Date
HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN); // MANDATORY
```
Failing to call GetDate causes the next GetTime to return stale data.

## Backup Register Magic
```c
#define MAGIC 0x12345678UL
// Detect first boot vs reset:
if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0) != MAGIC) {
    // First boot: set time, write magic
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, MAGIC);
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0); // reset count = 0
} else {
    // Subsequent boot: increment reset count
    uint32_t cnt = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) + 1;
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, cnt);
}
```

## CubeMX Configuration
| Item | Setting |
|------|---------|
| RTC | Enable, LSI clock source |
| RTC Calendar | Enable |
| USART1 | PA9/PA10, 115200 8N1 |

## Expected Output
```
Boot #1 (first): RTC set to 2025-01-01 12:00:00
Boot #2: 2025-01-01 12:00:05  resets=1
Boot #3: 2025-01-01 12:00:15  resets=2
```
