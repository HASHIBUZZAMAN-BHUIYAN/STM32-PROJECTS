# Black Pill ADVANCED/04 — PID Controller (Software)

## Overview
Software PID controller running at 100 Hz via TIM3 interrupt. Controls a simulated plant (first-order lag) rather than real motor hardware, letting you tune Kp/Ki/Kd and observe transient response via USART1 CSV output.

## Why Simulated Plant
Real DC motor + encoder requires extra hardware. Simulated plant `y[k+1] = 0.9*y[k] + 0.1*u[k]` (τ≈1s) captures the real dynamics and lets you focus on PID tuning — tune here first, then port to a real plant.

## PID Algorithm
```
e[k] = setpoint - y[k]
integral += e[k] * dt          (anti-windup clamp ±MAX_INT)
derivative = (e[k] - e[k-1]) / dt
u[k] = Kp*e + Ki*integral + Kd*derivative
u[k] = clamp(u[k], 0.0, 100.0)
```
dt = 0.01 s (100 Hz)

## UART Commands
| Command | Action |
|---------|--------|
| `set <val>` | Set setpoint (0.0-100.0) |
| `pid <Kp> <Ki> <Kd>` | Update gains |
| `status` | Print current gains + state |
| `reset` | Zero integrator and state |

## CSV Output (every 100ms)
`tick,setpoint,plant_y,u,error\r\n`

## Default Gains
Kp=2.0, Ki=0.5, Kd=0.1 — gives ~3s settling, ~10% overshoot on the simulated plant.

## CubeMX
TIM3 → 100 Hz interrupt (PSC=999, ARR=999 at 100 MHz), USART1 115200, PC13 LED, TIM11 timebase.
