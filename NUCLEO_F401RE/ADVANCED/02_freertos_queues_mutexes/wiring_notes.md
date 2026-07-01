# Wiring Notes — Nucleo-F401RE ADVANCED/02: FreeRTOS Queues & Mutexes

## No Extra Hardware Required
Onboard USART2 via ST-Link VCP only.

## Priority Inheritance Explained
Without mutex priority inheritance, a scenario called **priority inversion** can occur:
1. Low-priority StatTask acquires uart_mutex
2. High-priority ConsumerTask needs uart_mutex → blocks
3. Medium-priority ProducerA preempts StatTask (same priority or higher)
4. StatTask never releases mutex → ConsumerTask starves

FreeRTOS mutexes include priority inheritance: when ConsumerTask blocks on the mutex, StatTask temporarily inherits Consumer's HIGH priority, preempts ProducerA, finishes quickly, releases the mutex, then drops back to LOW priority.

## Queue Depth Sizing
`queue_depth = max_burst_rate × consumer_period`
- ProducerA: 10 Hz, ProducerB: 5 Hz → 15 messages/sec combined
- ConsumerTask wakes instantly on each message
- 10-slot queue handles any burst below 667ms

## Parts List
| Component | Qty |
|-----------|-----|
| Nucleo-F401RE | 1 |
| USB cable | 1 |
