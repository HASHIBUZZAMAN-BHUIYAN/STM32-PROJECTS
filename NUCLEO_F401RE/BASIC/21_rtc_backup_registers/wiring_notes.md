# Wiring Notes — Nucleo-F401RE BASIC/21: RTC Backup Registers

## No Extra Wiring Required
This project uses only onboard resources.

## Key Concept: Backup Domain
The STM32F401 backup domain contains:
- RTC (real-time clock)
- 20 backup registers (BKP_DR1..DR20), each 32-bit
- Backup SRAM (optional, needs separate enable)

These persist across all reset types (NRST, software reset, IWDG reset) as long as VBAT or VDD is applied. They are cleared only by a full power cycle (VDD + VBAT removed).

## Important RTC Note
Call `HAL_RTC_GetDate()` after every `HAL_RTC_GetTime()` — even if you don't need the date. The F4 RTC uses a shadow register that is only unlocked by reading BOTH time and date. Skipping GetDate will return stale time values.

## LSI Accuracy
Using LSI (~32 kHz, ±5%) means the RTC drifts. For accurate time-keeping, add an external 32.768 kHz crystal to PC14/PC15 and switch CubeMX to LSE. The Nucleo-F401RE PCB has footprints for this crystal.

## Parts List
| Component | Qty |
|-----------|-----|
| Nucleo-F401RE | 1 |
| USB cable | 1 |
