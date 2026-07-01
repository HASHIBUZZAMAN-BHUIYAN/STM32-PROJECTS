# Nucleo-F401RE ADVANCED/06 — PID Motor Speed Control

## Overview
Implements a discrete-time PID controller that regulates a DC motor's speed using PWM output and encoder feedback. The PID runs at 100 Hz in a TIM3 interrupt. A UART command interface sets the target RPM. Anti-windup clamps the integrator.

## Hardware Setup
- **DC motor**: small geared DC motor with encoder (e.g., GA12-N20 with 12 CPR encoder)
- **H-bridge driver**: L298N or DRV8833 module (3.3V compatible control signals)
- **PWM output**: TIM1_CH1 on PA8 → motor speed
- **Direction**: PA9 (GPIO output) → H-bridge direction pin
- **Encoder**: TIM4 encoder mode — PB6=CH1 (A), PB7=CH2 (B)

## CubeMX Configuration
| Peripheral | Settings |
|-----------|---------|
| TIM1_CH1 | PA8, PWM Generation, PSC=0, ARR=839 (100 kHz at 84 MHz) |
| TIM4 | Encoder mode, CH1=PB6, CH2=PB7, Counter Period=65535 |
| TIM3 | 100 Hz interrupt (PSC=839, ARR=999) |
| GPIO PA9 | Output Push-Pull, motor direction |
| USART2 | PA2/PA3, 115200 8N1, global IT |

## PID Algorithm (100 Hz)
```
error = setpoint - measured_rpm
integral += error * dt
integral = clamp(integral, -max_int, +max_int)  // anti-windup
derivative = (error - prev_error) / dt
output = Kp*error + Ki*integral + Kd*derivative
output = clamp(output, 0, 100)  // duty cycle 0-100%
```

## Encoder RPM Calculation
At 100 Hz PID tick:
```
count = TIM4->CNT (signed 16-bit wrap handled)
delta = count - prev_count  // encoder ticks per 10ms
rpm = delta * 100 * 60 / CPR  // CPR = encoder counts per revolution
```
For GA12-N20 with 12 CPR at 100:1 gearbox → 1200 CPR at shaft.

## UART Commands
- `set 120` — set target RPM to 120
- `pid 1.5 0.2 0.05` — set Kp Ki Kd
- `stop` — set target to 0
- `status` — print current RPM, error, output duty

## Safety
- Motor supply voltage: check motor rating — typical 3–12V
- H-bridge: do NOT apply 5V control signals to 3.3V-only H-bridges
- DRV8833: 3.3V control OK, supply up to 10.8V
- L298N: 5V control → use 3.3V→5V level converter on PA8/PA9 signals
