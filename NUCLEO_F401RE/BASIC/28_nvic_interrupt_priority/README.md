# Nucleo-F401RE BASIC/28 — NVIC Interrupt Priority Management

## Overview
Demonstrates NVIC (Nested Vectored Interrupt Controller) priority configuration with three simultaneous interrupt sources: TIM2 at 1 kHz (highest priority 0), USART2 RX interrupt (priority 1), and EXTI13 B1 button (priority 2). Shows that TIM2 can preempt the UART ISR, but B1 cannot preempt either.

## CubeMX Configuration

| Peripheral | Setting | Priority |
|-----------|---------|---------|
| TIM2 | Up counter, PSC=83, ARR=999 → 1 kHz interrupt | 0 (highest) |
| USART2 | 115200 8N1, RX interrupt | 1 |
| GPIO PC13 | EXTI13, falling edge | 2 |
| NVIC Priority Group | Group 4 (4 bits preemption, 0 sub) | |

**NVIC Priority Group**: Set in CubeMX → System Core → NVIC → Priority Group = 4.

## Expected Behavior
- TIM2 updates a 1ms timestamp counter every ISR (preempts all)
- Typing in USART2 terminal echoes characters back; TIM2 ISR fires inside UART ISR
- B1 press message printed after UART and TIM2 ISRs complete
- UART prints preemption evidence: TIM count increments during UART ISR
