# 03 — FreeRTOS Mutex (Shared I2C Bus)

## Overview

Demonstrates protecting a shared hardware resource (I2C1) with a FreeRTOS mutex.
Two tasks both need to use the same I2C bus:

- **Task1** reads temperature from a **BMP280** sensor (I2C address 0x76 or 0x77).
- **Task2** writes display data to an **SSD1306 OLED** (I2C address 0x3C or 0x3D).

Without the mutex, concurrent I2C transactions corrupt each other (bus collisions,
NAK errors, garbled data). With the mutex, only one task holds the bus at a time.

## Tasks

| Task  | Period  | Priority        | I2C Operation                       |
|-------|---------|-----------------|-------------------------------------|
| Task1 | 500 ms  | osPriorityNormal| Acquire mutex → BMP280 read → release|
| Task2 | 300 ms  | osPriorityNormal| Acquire mutex → SSD1306 write → release|

Both tasks share the same priority to demonstrate round-robin scheduling while
competing for the mutex.

## Why Mutex vs Critical Section

A critical section (`taskENTER_CRITICAL`) disables interrupts — fine for very
short operations but not acceptable for I2C transactions that can take hundreds
of microseconds. A mutex lets other tasks (and the scheduler) run while one task
holds I2C; the bus is protected without starving the system.

## Without Mutex (What Goes Wrong)

If both tasks call `HAL_I2C_Master_Transmit()` simultaneously:
- The I2C state machine in HAL becomes re-entrant — undefined behavior.
- Bus arbitration may lock SCL/SDA lines low.
- One or both transactions return `HAL_ERROR` or `HAL_BUSY`.

## CubeMX Configuration

1. **FreeRTOS** — CMSIS_V2, heap >= 4096 bytes
2. **I2C1** — Standard mode (100 kHz), SCL=PB6, SDA=PB7
3. **USART1** — 115200 baud, 8N1 (for debug output)
4. Clock: 72 MHz

## Files

| File               | Purpose                                         |
|--------------------|-------------------------------------------------|
| `main_usercode.c`  | User code sections to paste into CubeMX main.c |
| `wiring_notes.md`  | Hardware connections and voltage warnings       |

## Expected Serial Output

```
[T1] BMP280 temp raw: 0x5A3F20
[T2] SSD1306 write OK
[T1] BMP280 temp raw: 0x5A3F22
[T2] SSD1306 write OK
```

## Notes

- Code is **not hardware-tested**. Validate on your board.
- BMP280 register parsing (compensation formula) is omitted for brevity —
  see the BMP280 datasheet section 4.2.3.
- SSD1306 command bytes shown are minimal; a full driver library is recommended.
