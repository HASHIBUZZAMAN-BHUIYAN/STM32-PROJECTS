# Wiring Notes — Black Pill BASIC/15: Multiple Timer Channels

## Pin Assignment
| Black Pill Pin | Signal | Component |
|---------------|--------|----------|
| PA8 | TIM1_CH1 PWM | LED1 via 330Ω |
| PA11 | TIM1_CH4 PWM | LED2 via 330Ω |
| GND | GND | LED cathodes |
| PC13 | Onboard LED | Heartbeat |

## Wiring
```
PA8  ─── 330Ω ─── LED1(+) ─── LED1(-) ─── GND
PA11 ─── 330Ω ─── LED2(+) ─── LED2(-) ─── GND
```

## Pin Conflict Table (TIM1 on Black Pill)
| Channel | Pin | Conflict |
|---------|-----|---------|
| CH1 | PA8 | FREE — use this |
| CH2 | PA9 | USART1_TX (conflict if UART needed) |
| CH3 | PA10 | USART1_RX (conflict if UART needed) |
| CH4 | PA11 | USB_OTG_DM (conflict if USB needed) |
| CH1N | PB13 | FREE |

For this project: use CH1 (PA8) and CH4 (PA11). If you need UART debugging, disable TIM1_CH2/CH3 and use a logic analyzer on PA8/PA11.

## Oscilloscope Verification
Connect PA8 and PA11 to an oscilloscope to observe the opposite-phase fading PWM waveforms. Both channels share the same frequency (1 kHz) but have independently controlled duty cycles.

## Parts List
| Component | Qty |
|-----------|-----|
| Black Pill (STM32F411CEU6) | 1 |
| LED (any color) | 2 |
| 330Ω resistor | 2 |
| Breadboard + jumpers | 1 set |
| USB-C cable | 1 |
