# Nucleo-F401RE ADVANCED/09 — Low Power Sensor Node

## Overview
A battery-friendly sensor node that reads BMP280 temperature/pressure, transmits a compact JSON payload over USART, then enters Standby mode. The RTC alarm wakes it every 60 seconds. A backup register counts wake cycles across power cycles (Standby is effectively a full reset).

## Power Budget (Estimated)
| Phase | Duration | Current | Energy |
|-------|----------|---------|--------|
| Boot + init | ~50ms | 30mA | 1.5mJ |
| BMP280 read | ~10ms | 28mA | 0.28mJ |
| UART TX | ~5ms | 28mA | 0.14mJ |
| Standby sleep | ~59.9s | 4µA | 0.024mJ |
| **Total per minute** | 60s | avg ~0.1mA | ~1.9mJ |

At 1000mAh LiPo: ~1000 hours ≈ 41 days (theoretical; actual depends on UART active and LED)

## CubeMX Configuration
| Peripheral | Settings |
|-----------|---------|
| RTC | Enable, LSE or LSI clock, Alarm A enabled |
| PWR | Enable low-power mode |
| I2C1 | PB8/PB9, BMP280 |
| USART2 | PA2/PA3, 115200 |
| GPIO PA5 | LD2, Output |

## Standby vs Stop Mode
| Mode | Wakeup | RAM retained | Code continues |
|------|--------|-------------|---------------|
| Stop | EXTI, RTC, UART | Yes | Yes (from next line) |
| Standby | RTC alarm, WKUP pin | No | No (full reset) |

Standby uses less power (2µA vs 400µA for Stop). Use RTC backup registers to pass info across Standby resets.

## RTC Backup Registers (F401)
The F401 has 20 backup registers (RTC_BKP_DR0 … DR19) in the backup domain powered by VBAT. They survive Standby resets.

```c
// Store data
HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, value);
// Read data
uint32_t v = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0);
```

## Expected UART Output (every 60s)
```
{"wake":1,"t":24.35,"p":101325}
{"wake":2,"t":24.36,"p":101320}
```
