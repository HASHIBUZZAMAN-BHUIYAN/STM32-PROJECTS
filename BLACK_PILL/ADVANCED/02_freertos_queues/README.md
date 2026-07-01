# Black Pill ADVANCED/02 — FreeRTOS Queues (Multi-Producer)

## Overview
Two producer tasks at different rates feed sensor data into a single queue. A consumer task drains it and logs to USART1. Focuses on queue depth sizing, drop counting, and comparing queue vs direct-task-notify patterns.

## Architecture
```
SensorA (100ms) ──┐
                   ├→ DataQueue(16) → Consumer → USART1
SensorB (250ms) ──┘
                     StatTask(2s) → USART1 (drops, queue depth)
```

## Queue Depth Sizing
- SensorA: 10 Hz, SensorB: 4 Hz → 14 msgs/sec
- Consumer wakes immediately → processes in <1ms each
- 16-slot queue: ~1.1 second buffer before drops start

## Black Pill Advantage
128KB RAM allows larger queues and message structs without memory pressure.

## CubeMX Configuration
Same as ADVANCED/01 with FreeRTOS enabled and TIM11 timebase.

## Expected Output
```
[A] seq=5 val=500
[B] seq=2 val=250
[STAT] q=0 drops=0 cons=7
```
