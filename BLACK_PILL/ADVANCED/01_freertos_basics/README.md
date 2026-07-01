# Black Pill ADVANCED/01 — FreeRTOS Basics

## Overview
Introduces FreeRTOS on the STM32F411 at 100 MHz. Three tasks run concurrently: BlinkTask, CounterTask (sends to queue), and PrintTask (receives from queue and logs to USART1). Identical in structure to Nucleo ADVANCED/01 but running on the faster, USB-capable Black Pill.

## Black Pill FreeRTOS Advantages
- 128KB RAM vs 64KB on F401 → more room for task stacks and queues
- 100 MHz core → can run more tasks at higher rates
- USB CDC available for live dashboard (see ADVANCED/10)

## CubeMX Configuration
| Item | Setting |
|------|---------|
| FreeRTOS | CMSIS-V2, Heap4, 12288 bytes |
| TIM11 | HAL timebase (SysTick → FreeRTOS) |
| USART1 | PA9/PA10, 115200 8N1 |

## Tasks
| Task | Priority | Stack |
|------|----------|-------|
| BlinkTask | Normal | 128 words |
| CounterTask | AboveNormal | 128 words |
| PrintTask | Normal | 256 words |

## Key Pattern
Same CMSIS-V2 API as Nucleo ADVANCED/01. See that project for full API reference.

## Expected Output (USART1 via USB-UART adapter)
```
[RTOS] F411 task demo started
count=1 tick=250
count=2 tick=500
```
