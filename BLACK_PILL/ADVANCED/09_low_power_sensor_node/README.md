# Black Pill ADVANCED/09 — Low-Power Sensor Node

## Overview
Battery-operated sensor node: wake from Standby via RTC alarm, read BMP280 in forced mode (single shot), transmit JSON over USART1, write to W25Q32 flash log, then return to Standby. Entire active window ≈800ms.

## Power Budget (3.3V, CR2032 ~200mAh)
| Phase | Current | Duration | Energy/cycle |
|-------|---------|----------|-------------|
| Standby | ~8µA | 60s | 480µAs |
| Boot+init | ~25mA | 100ms | 2500µAs |
| BMP280 read | ~25mA | 200ms | 5000µAs |
| UART TX | ~27mA | 50ms | 1350µAs |
| SPI flash | ~26mA | 100ms | 2600µAs |
| Sleep off | ~25mA | 50ms | 1250µAs |
| **Cycle total** | — | ~60.5s | **~13200µAs** |

Theoretical battery life: 200mAh × 3600 / 13200µAs × 60s/cycle ÷ 3600 = **~2000 hours**

## Key Differences from Nucleo ADVANCED/09
- W25Q32 SPI flash for local log (SPI1 PA5/PA6/PA7 — free on Black Pill)
- USART1 instead of USART2
- USB is on PA11/PA12 (can wake from USB if enabled, not implemented here)
- 100 MHz max vs 84 MHz

## Boot Sequence
1. Check `RCC_CSR` for Standby wakeup flag
2. If cold boot: init RTC, set magic in BKP_DR0
3. If Standby wakeup: increment wake_count (BKP_DR1)
4. Read BMP280 (forced mode — single shot)
5. TX JSON to USART1
6. Write record to W25Q32 flash
7. Set RTC alarm for +60s
8. Enter Standby

## RTC Backup Registers
| Register | Content |
|----------|---------|
| BKP_DR0 | Magic (0xDEADBEEF) |
| BKP_DR1 | Wake count |
| BKP_DR2 | Error count |
