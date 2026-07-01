# Wiring Notes — Black Pill ADVANCED/03: FreeRTOS Mutex

## Hardware
Only the Black Pill board and a USB-UART adapter are needed.

## USART1 Connection
| Black Pill | USB-UART |
|-----------|---------|
| PA9 (TX) | RX |
| GND | GND |

## What to Watch
When LowTask holds `sharedMtx` and HighTask blocks, the `[LOG]` line will
briefly show `low_P` elevated to match `high_P` — that is priority inheritance
in action. After HighTask acquires and releases, `low_P` returns to Normal.

## Parts List
| Component | Qty |
|-----------|-----|
| Black Pill (STM32F411CEU6) | 1 |
| USB-UART adapter | 1 |
| Jumper wires | 2 |
