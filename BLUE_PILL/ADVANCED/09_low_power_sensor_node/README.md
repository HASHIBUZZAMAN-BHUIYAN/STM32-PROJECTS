# 09 — Low Power Sensor Node

## Overview
Battery-powered sensor node using STM32 Standby mode. The RTC wakes the MCU every 30 seconds via an RTC alarm (driven by the LSI oscillator). On wake-up: reads BMP280 temperature/pressure via I2C, transmits the reading over UART1, increments a wakeup counter stored in an RTC backup register, then re-enters Standby.

## Power Modes Used
| Mode    | Current Draw (typical) | Wake Source |
|---------|------------------------|-------------|
| Run     | ~10–15 mA             | N/A         |
| Standby | ~2–5 µA               | RTC Alarm   |

In Standby mode, RAM and most registers are lost; only the RTC and backup registers retain state.

## Wake-Up Sequence
```
Standby sleep (LSI RTC ticking)
      │
      └──(RTC Alarm fires every 30 s)──► NRST released → Full reset-like wake
                                                │
                                                ▼
                                    Read BMP280 (I2C)
                                                │
                                                ▼
                                    Transmit over UART1
                                                │
                                                ▼
                                    Increment RTC backup reg (BKP_DR1)
                                                │
                                                ▼
                                    Set next RTC alarm (+30 s)
                                                │
                                                ▼
                                    Enter Standby (HAL_PWR_EnterSTANDBYMode)
```

## RTC Notes
- LSI oscillator used (no external 32.768 kHz crystal required).
- LSI frequency: nominally 40 kHz on STM32F1, but may vary ±40%. Acceptable for a 30-second period.
- RTC on STM32F103 is a simple 32-bit counter — no calendar. Alarm = counter + 30.
- BKP_DR1 (16-bit backup data register 1) stores the wake-up count. Survives Standby but is cleared by a full power loss unless VBAT is connected.
- PWR and BKP access requires `HAL_PWR_EnableBkUpAccess()`.

## BMP280 I2C Address
- 0x76 (SDO to GND) or 0x77 (SDO to VCC).

## CubeMX Configuration
| Peripheral | Setting                                                    |
|------------|------------------------------------------------------------|
| RTC        | Enable, clock source = LSI, Alarm A enabled               |
| PWR        | Enable Power peripheral (for Standby)                     |
| I2C1       | Standard mode 100 kHz, PB6=SCL, PB7=SDA                  |
| USART1     | 115200 8N1, TX polling (UART is powered down in Standby)  |
| BKP        | Enabled (accessed via RTC backup registers)               |
| SYS        | SWD, no RTOS                                              |

## Output (UART, per wake-up)
```
[Wake #3] BMP280: 24.87 C, 1013.2 hPa
```

## Files
| File              | Purpose                          |
|-------------------|----------------------------------|
| `main_usercode.c` | All user code sections           |
| `wiring_notes.md` | Hardware connection guide        |

## Power Tip
Remove the onboard Blue Pill power LED and the PC13 LED (or do not drive PC13 LOW) to minimize idle current. The onboard power LED draws ~1 mA continuously.

## Not Tested
This code has not been compiled or run on hardware. It is provided as a reference implementation for study and adaptation.
