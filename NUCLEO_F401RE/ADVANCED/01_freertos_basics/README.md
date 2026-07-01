# Nucleo-F401RE ADVANCED/01 — FreeRTOS Basics

## Overview
Introduces FreeRTOS on the STM32F401RE using CubeMX CMSIS-V2 wrapper. Three tasks run concurrently: a blink task, a counter task, and a print task. A queue passes data between tasks demonstrating the most fundamental RTOS patterns.

## Tasks
| Task | Priority | Stack | Function |
|------|----------|-------|----------|
| BlinkTask | Normal | 128 words | Toggles LD2 every 500ms |
| CounterTask | AboveNormal | 128 words | Increments counter every 250ms, sends to queue |
| PrintTask | Normal | 256 words | Waits on queue, prints count + tick via UART |

## CubeMX Configuration
| Item | Setting |
|------|---------|
| FreeRTOS | CMSIS-V2, Heap4 |
| Heap size | 8192 bytes |
| USART2 | 115200 8N1, PA2/PA3 |
| LD2 GPIO | PA5 Output |
| TIMEBASE | TIM11 (FreeRTOS takes SysTick) |

**Important**: When FreeRTOS is enabled, CubeMX moves HAL timebase from SysTick to another timer (TIM11 recommended). SysTick is used exclusively by the RTOS scheduler.

## Expected Terminal Output
```
[RTOS] Task demo started
count=1 tick=250
count=2 tick=500
count=3 tick=750
...
```

## Key CMSIS-V2 API Used
- `osThreadNew()` — create task
- `osMessageQueueNew()` / `osMessageQueuePut()` / `osMessageQueueGet()` — queue
- `osDelay()` — RTOS-aware delay (yields CPU, does not busy-wait)
- `osMutexNew()` / `osMutexAcquire()` / `osMutexRelease()` — for UART protection

## Why FreeRTOS Instead of Bare-Metal Loops?
- **Deterministic timing**: osDelay gives each task its exact period without affecting others
- **Priority preemption**: higher-priority tasks preempt lower ones
- **Clean separation of concerns**: each task has one job
- **Resource sharing**: queues/mutexes prevent data corruption from concurrent access

## Safety Note
Stack overflow is the most common FreeRTOS pitfall. Enable `configCHECK_FOR_STACK_OVERFLOW = 2` in FreeRTOSConfig.h and implement `vApplicationStackOverflowHook()` to catch it early.
