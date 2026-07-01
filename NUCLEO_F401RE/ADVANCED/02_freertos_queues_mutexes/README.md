# Nucleo-F401RE ADVANCED/02 — FreeRTOS Queues & Mutexes

## Overview
Deep-dives into FreeRTOS inter-task communication. Two producer tasks send sensor-like data into a queue; one consumer task drains the queue and logs to UART with a mutex protecting the shared UART handle. A second mutex protects a shared accumulator, demonstrating priority inheritance.

## Architecture
```
ProducerA (AboveNormal) ─┐
                          ├─→ DataQueue (10 items) ─→ ConsumerTask (High) ─→ UART (mutex)
ProducerB (Normal)      ─┘
                                StatTask (Low) ─→ UART (same mutex, lower priority)
```

## CubeMX Configuration
Same as ADVANCED/01: FreeRTOS CMSIS-V2, Heap4, TIM11 timebase, USART2 115200.

## Key Concepts Demonstrated
1. **Queue full behavior**: `osMessageQueuePut` with timeout 0 (drop) vs timeout osWaitForever (block)
2. **Mutex priority inheritance**: if StatTask holds UART mutex and ConsumerTask (high priority) needs it, StatTask temporarily inherits Consumer's priority to avoid inversion
3. **Queue depth**: choosing queue size = max burst messages expected between Consumer wake-ups
4. **osMessageQueueGetCount**: check queue occupancy in StatTask

## Expected Output
```
[A] seq=1 val=1023
[B] seq=1 val=512
[A] seq=2 val=1024
[STAT] q=0 drops=0 cons=3
```

## Differences: Queue vs Semaphore vs Mutex
| | Queue | Semaphore | Mutex |
|-|-------|-----------|-------|
| Carries data | Yes | No (count only) | No |
| Ownership | None | None | Yes (only acquirer releases) |
| Priority inherit | No | No | Yes |
| Use for | Passing data between tasks | Signaling events | Protecting shared resources |
