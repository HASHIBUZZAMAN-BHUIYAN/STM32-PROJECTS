# Wiring Notes — Nucleo-F401RE BASIC/20: Power Mode Comparison

## No Extra Wiring Required
All components are onboard.

## Onboard Resources Used
| Resource | Notes |
|----------|-------|
| B1 button (PC13) | Wakes Sleep and Stop modes |
| LD2 (PA5) | Off during low-power modes |
| ST-Link VCP (USART2) | UART output before entering each mode |
| RTC + LSI | Wakes from Standby after 10s |

## Measuring Current (Optional)
The Nucleo-F401RE has an IDD measurement jumper (typically labeled on the PCB near the STM32 chip — check your board revision's schematic).
- Locate the 2-pin jumper/solder bridge on the 3.3V supply line
- Remove the default jumper bridge, insert your multimeter (µA/mA mode) in series
- Measure current draw in each mode

Expected readings:
| Mode | Expected |
|------|---------|
| Active 84 MHz | ~25-30 mA (includes ST-Link) |
| Sleep | ~10-15 mA |
| Stop | ~0.5-1 mA |
| Standby | ~3-10 µA |

The ST-Link chip itself consumes power. For accurate MCU-only measurement, power the STM32 independently and disconnect ST-Link.

## Standby Wake Sequence
After Standby, the MCU full-resets. The firmware restarts from the beginning of main(). demo_step resets to DEMO_SLEEP. This is normal Standby behavior — use RTC backup registers to save state across Standby if needed (see BASIC/21).
