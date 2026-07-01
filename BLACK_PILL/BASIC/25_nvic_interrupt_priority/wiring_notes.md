# Wiring Notes — Black Pill BASIC/25: NVIC Interrupt Priority

## No Extra Wiring Required
Uses onboard resources:
- PA0 = user button (active-HIGH, rising edge EXTI)
- PC13 = onboard LED (active-LOW)
- USART1 via USB-UART adapter

## USART1 Connection
| Black Pill | USB-UART Adapter |
|-----------|----------------|
| PA9 (TX) | RX |
| PA10 (RX) | TX |
| GND | GND |

## NVIC Priority Settings in CubeMX
1. System Core → NVIC
2. Set Priority Group: 4 bits preemption, 0 bits subpriority
3. TIM2: Preemption Priority = 0
4. USART1 global: Preemption Priority = 1
5. EXTI0: Preemption Priority = 2

## CubeMX Settings
| Timer | Settings |
|-------|----------|
| TIM2 | PSC=99, ARR=999 → 1 kHz at 100 MHz |
| USART1 | 115200 8N1, global IT enabled |
| PA0 | GPIO_EXTI0, rising edge, pull-down |

## Expected Output
```
NVIC preemption: TIM2(P0)>UART(P1)>PA0(P2)
TIM2 count=2000
a  ← echo of typed 'a'
TIM2 fired N times during UART ISR
```
N will be > 0, proving TIM2 preempted the UART ISR.

## Parts List
| Component | Qty |
|-----------|-----|
| Black Pill (STM32F411CEU6) | 1 |
| USB-UART adapter | 1 |
| USB-C cable | 1 |
| Jumper wires | 3 |
