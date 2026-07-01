# Black Pill BASIC/18 — Low Power Modes Comparison

## Overview
Cycles through Sleep, Stop, and Standby modes on the STM32F411, printing current mode to USART1 before entering. Button PA0 wakes Sleep and Stop modes; RTC alarm wakes Standby. Demonstrates power management for battery-powered Black Pill applications.

## F411 Power Modes
| Mode | CPU | Peripherals | RAM | Wake sources | Est. current |
|------|-----|-------------|-----|-------------|-------------|
| Run (100MHz) | Running | Active | Retained | — | ~30mA |
| Sleep (WFI) | Halted | Clock-gated | Retained | Any interrupt | ~8mA |
| Stop (LP) | Halted | Off | Retained | EXTI, RTC | ~0.5mA |
| Standby | Off | Off | Lost | RTC, WKUP | ~2µA |

## CubeMX Configuration
| Peripheral | Setting |
|-----------|---------|
| PWR | Enable |
| RTC | LSE or LSI, Alarm A |
| GPIO PA0 | EXTI0, rising edge, pull-down |
| USART1 | PA9/PA10, 115200 8N1 |
| PC13 | Output (LED) |

## After Stop Mode: Re-init Clock!
Stop mode halts all PLLs. The CPU wakes on HSI 16 MHz. Must call `SystemClock_Config()` to restore 100 MHz:
```c
HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
SystemClock_Config(); // CRITICAL — call before any peripheral use
```

## Standby vs Blue Pill/Nucleo
F411 Standby current (~2µA) matches F401. The F103 (Blue Pill) typically achieves ~5µA in Standby due to a less efficient backup domain.
