# Wiring Notes — Black Pill BASIC/14: High Clock Timing

## No Extra Hardware Required
Uses onboard PC13 LED and USART1 via USB-UART adapter.

## USART1 Connection
| Black Pill | USB-UART Adapter |
|-----------|----------------|
| PA9 (TX) | RX |
| GND | GND |

## TIM2 Configuration for µs
TIM2 is a 32-bit timer. Configuration:
- PSC = 99 → 100 MHz / (99+1) = 1 MHz counter
- ARR = 0xFFFFFFFF → 32-bit free-running (wraps at ~4295 seconds)
- `__HAL_TIM_GET_COUNTER(&htim2)` reads current µs count

## FPU Note
The STM32F411 has a hardware FPU (Cortex-M4F). Ensure your compiler flags include:
- `-mfpu=fpv4-sp-d16`
- `-mfloat-abi=hard`

CubeMX sets these automatically for F4 targets. The FPU runs single-precision sinf() in ~14 cycles vs ~300 cycles on a softfloat M0/M3.

## 100 MHz PLL Setup in CubeMX
```
HSE (25 MHz crystal on WeAct board)
  → PLL: PLLM=25, PLLN=200, PLLP=2
  → SYSCLK = 25 × 200 / (25 × 2) = 100 MHz
APB1: /2 = 50 MHz (I2C, TIM2-7)
APB2: /1 = 100 MHz (SPI1, USART1, TIM1)
```

## Parts List
| Component | Qty |
|-----------|-----|
| Black Pill (STM32F411CEU6) | 1 |
| USB-UART adapter | 1 |
| USB-C cable | 1 |
| Jumper wires | 2 |
