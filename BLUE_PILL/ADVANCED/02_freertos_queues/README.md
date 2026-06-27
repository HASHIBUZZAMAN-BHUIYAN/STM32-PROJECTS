# 02 — FreeRTOS Queues

## Overview

Demonstrates inter-task communication using a FreeRTOS queue. A producer task
(SensorTask) samples the ADC at a fixed rate and posts values to a queue.
A consumer task (DisplayTask) blocks on the queue and forwards readings over UART1.
The queue decouples the sensor sample rate from the display/transmission rate.

## Tasks

| Task        | Priority        | Period        | Action                                      |
|-------------|-----------------|---------------|---------------------------------------------|
| SensorTask  | osPriorityHigh  | 200 ms        | Read ADC1 Ch1 (PA1), post to queue          |
| DisplayTask | osPriorityNormal| blocks on Q   | Format ADC value as ASCII, send via UART1   |

Queue depth is 8 elements (`uint16_t`). If the queue is full (display too slow),
`osMessageQueuePut` returns `osErrorResource` — the sample is dropped and a counter
increments to track overflow.

## CubeMX Configuration

1. **FreeRTOS** — CMSIS_V2 interface, heap >= 4096 bytes
2. Define two tasks in the FreeRTOS config tab (or in USER CODE):
   - `SensorTask`  — stack 128 words, priority High
   - `DisplayTask` — stack 256 words, priority Normal
3. **ADC1** — Channel 1 (PA1), software trigger, single conversion mode
4. **USART1** — 115200 baud, 8N1, TX=PA9, RX=PA10
5. Clock: 72 MHz via HSE + PLL

## Queue Design

```
SensorTask                    DisplayTask
  (200 ms)                    (blocked)
    |                              |
    | osMessageQueuePut()          |
    |──────────────────► Queue ───►| osMessageQueueGet()
                       depth=8     |
                      uint16_t     | HAL_UART_Transmit()
                                   |──────────────────► UART1
```

## Expected Serial Output

```
[ADC] PA1 = 1234  (0x04D2)
[ADC] PA1 = 1256  (0x04E8)
[ADC] PA1 = 1198  (0x04AE)
```

## Files

| File               | Purpose                                         |
|--------------------|-------------------------------------------------|
| `main_usercode.c`  | User code sections to paste into CubeMX main.c |
| `wiring_notes.md`  | Hardware connections and voltage warnings       |

## Notes

- Code is **not hardware-tested**. Validate on your board.
- `osMessageQueueGet` with `osWaitForever` puts DisplayTask into Blocked state,
  freeing the CPU for SensorTask and the idle task.
- Never call `HAL_Delay()` inside FreeRTOS tasks; use `osDelay()`.
