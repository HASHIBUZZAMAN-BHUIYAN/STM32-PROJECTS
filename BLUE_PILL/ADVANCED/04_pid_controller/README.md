# 04 — PID Controller (LED Brightness via PWM + LDR Feedback)

## Overview

Implements a software PID controller that regulates LED brightness using light
feedback from an LDR (light-dependent resistor). The PID runs in a timer interrupt
at 100 Hz (TIM4), ensuring a fixed sample period regardless of other activity.

```
Setpoint (UART) ──► PID ──► TIM1 CH1 PWM ──► LED ──► LDR ──► ADC1 ──► PID
```

The operator sets a target ADC value (0–4095) over UART. The PID drives the LED
PWM duty cycle until the LDR reading matches the setpoint. Kp, Ki, and Kd are
also tunable via UART commands at runtime.

## PID Equation

```
error = setpoint - measured
integral += error * dt
derivative = (error - prev_error) / dt

output = Kp*error + Ki*integral + Kd*derivative
```

Anti-windup clamps the integral term to `[-WINDUP_LIMIT, +WINDUP_LIMIT]`.
Output is clamped to `[0, PWM_PERIOD]` (0–100 % duty cycle).

## Peripherals

| Peripheral | Pin | Function                                     |
|------------|-----|----------------------------------------------|
| TIM1 CH1   | PA8 | PWM output → LED (via 220 Ω resistor)        |
| TIM4       | —   | 100 Hz period interrupt (PID tick)            |
| ADC1 Ch1   | PA1 | Analog input from LDR voltage divider         |
| USART1     | PA9/PA10 | UART commands and debug output          |

## UART Commands

| Command            | Example         | Action                           |
|--------------------|-----------------|----------------------------------|
| `S <value>\r\n`   | `S 2048\r\n`    | Set setpoint (0–4095)            |
| `P <value>\r\n`   | `P 1.50\r\n`    | Set Kp (float)                   |
| `I <value>\r\n`   | `I 0.10\r\n`    | Set Ki (float)                   |
| `D <value>\r\n`   | `D 0.05\r\n`    | Set Kd (float)                   |
| `R\r\n`           | `R\r\n`         | Print current PID state          |

## CubeMX Configuration

1. **TIM1** — Channel 1 PWM, PSC=0, ARR=719 → 100 kHz PWM at 72 MHz
2. **TIM4** — Period interrupt at 100 Hz: PSC=7199, ARR=99 (72 MHz / 7200 / 100 = 100 Hz)
3. **ADC1** — Channel 1 (PA1), software trigger, single conversion
4. **USART1** — 115200 baud, 8N1, RX interrupt enabled
5. Clock: 72 MHz

## Files

| File               | Purpose                                         |
|--------------------|-------------------------------------------------|
| `main_usercode.c`  | User code sections to paste into CubeMX main.c |
| `wiring_notes.md`  | Hardware connections and voltage warnings       |

## Notes

- Code is **not hardware-tested**. Validate on your board.
- PID tuning is application-specific. Start with Kp=1.0, Ki=0.0, Kd=0.0 and
  increase Ki slowly until steady-state error is eliminated.
- The LDR must face the LED directly for the feedback loop to close.
