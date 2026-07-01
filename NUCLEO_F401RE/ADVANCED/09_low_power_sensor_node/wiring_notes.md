# Wiring Notes — Nucleo-F401RE ADVANCED/09: Low Power Sensor Node

## Pin Assignment
| Nucleo Pin | Signal | Component |
|-----------|--------|----------|
| PB8 | I2C1_SCL | BMP280 SCL |
| PB9 | I2C1_SDA | BMP280 SDA |
| PA2/PA3 | USART2 TX/RX | ST-Link VCP |
| PA5 | LD2 | Heartbeat during active phase |
| 3.3V | VCC | BMP280 VCC |
| GND | GND | BMP280 GND |

## Standby Wakeup Behavior
Standby mode is essentially a reset with RTC running. On wakeup:
1. MCU reboots from reset vector
2. Firmware checks `PWR_FLAG_SB` to detect Standby wakeup vs cold power-on
3. Reads wake_count from RTC backup register
4. Runs sensor read + UART TX cycle
5. Sets next alarm → enters Standby again

## RTC Clock Source
The RTC needs a clock that survives Standby:
- **LSE** (32.768 kHz crystal): most accurate, needs physical crystal on PC14/PC15
- **LSI** (~32 kHz internal RC): no crystal needed, ±5% accuracy

On Nucleo-F401RE: LSE crystal X2 is included on the board. Enable LSE in CubeMX → RTC Clock Source.

## Current Measurement During Standby
Use the IDD jumper (see BASIC/29) to measure Standby current: expect 2–6µA. The ST-Link is powered separately and not included.

## Wake Interval Limitation
This demo uses 60-second alarm by adding 60 to current seconds (wraps at 60). For intervals >59 seconds, add hours/minutes to the alarm mask. The code caps at 59s for simplicity.

## Parts List
| Component | Qty | Notes |
|-----------|-----|-------|
| Nucleo-F401RE | 1 | LSE crystal on board |
| BMP280 I2C module | 1 | |
| Jumper wires | 4 | |
| USB cable | 1 | |

## 3.3V Safety
I2C lines (PB8/PB9) are 3.3V. BMP280 module is 3.3V logic. No level shifting needed.
