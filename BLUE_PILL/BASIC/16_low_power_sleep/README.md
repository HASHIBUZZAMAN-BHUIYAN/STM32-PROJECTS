# 16 — Low Power Sleep Mode

## Overview
Demonstrates STM32F103C8T6 SLEEP mode using the HAL Power library. The MCU sends a UART message, enters sleep via WFI (Wait For Interrupt), and wakes on an external interrupt triggered by a button on PA0 (EXTI0). On wake, the onboard LED toggles and a UART message is sent before sleeping again.

## Hardware
- Board: Blue Pill (STM32F103C8T6)
- Onboard LED: PC13 (active-low)
- Wake button: PA0 → EXTI0 (falling edge, pulled up internally or via external pull-up)
- UART1: PA9 (TX), PA10 (RX) — 115200 baud

## CubeMX Configuration
| Peripheral | Setting |
|-----------|---------|
| PA0 | GPIO_EXTI0, falling edge trigger, pull-up |
| PC13 | GPIO Output Push-Pull (onboard LED) |
| USART1 | Async, 115200 8N1, PA9 TX / PA10 RX |
| PWR | Enable in Peripherals (for HAL_PWR_*) |
| NVIC | EXTI0 interrupt enabled |
| SYS | Timebase: TIM1 (free SysTick for sleep mode) |

> Tip: Set the SysTick timebase source to TIM1 in CubeMX so SysTick does not wake the MCU from sleep on every 1 ms tick.

## Behavior
1. On reset: prints "Ready. Press PA0 to wake from sleep." via UART.
2. Prints "Sleeping..." then enters SLEEP mode (WFI).
3. Button press on PA0 fires EXTI0 ISR → HAL_GPIO_EXTI_Callback wakes MCU.
4. Toggles PC13 LED, prints "Awake!", short delay, then sleeps again.

## Power Notes
- SLEEP mode keeps CPU clock stopped but all peripherals running. Current draw drops from ~25 mA (active) to ~5–10 mA.
- For deeper savings use STOP or STANDBY mode (requires additional wakeup configuration).
- MAINREGULATOR_ON preserves SRAM; use LOWPOWERREGULATOR_ON for lower current at the cost of slower wakeup.

## Files
| File | Purpose |
|------|---------|
| `main_usercode.c` | User code sections to paste into CubeMX-generated `main.c` |
| `wiring_notes.md` | Pin connections and safety notes |

## Disclaimer
Code is provided as a reference starting point and has not been tested on hardware.
