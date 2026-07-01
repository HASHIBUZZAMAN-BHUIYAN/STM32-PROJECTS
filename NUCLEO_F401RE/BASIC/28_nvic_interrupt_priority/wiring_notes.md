# Wiring Notes — Nucleo-F401RE BASIC/28: NVIC Interrupt Priorities

## No Extra Wiring Required
Uses onboard resources only: B1 button (PC13), LD2 (PA5), ST-Link VCP (USART2).

## NVIC Priority Concepts
| Priority | Value | Source | Can preempt |
|----------|-------|--------|-------------|
| Highest | 0 | TIM2 (1 kHz) | Everything below |
| Medium | 1 | USART2 RX | Priority 2+ only |
| Lowest | 2 | EXTI13 B1 | Nothing in this demo |

**Preemption** means: if TIM2 fires while the UART ISR is running, the CPU immediately suspends the UART ISR, runs TIM2 ISR, then returns to UART ISR. This is transparent to UART code.

## CubeMX Setup
In NVIC settings:
1. Set Priority Group to "4 bits for preemption, 0 bits for sub-priority"
2. TIM2 → Preemption Priority = 0
3. USART2 global → Preemption Priority = 1
4. EXTI15_10 → Preemption Priority = 2

## Parts List
| Component | Qty |
|-----------|-----|
| Nucleo-F401RE | 1 |
| USB cable | 1 |
