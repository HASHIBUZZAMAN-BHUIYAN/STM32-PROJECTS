# Black Pill BASIC/08 — UART RX Interrupt + Command Parser

## Overview
Uses USART1 in interrupt mode (HAL_UART_Receive_IT) to receive commands without blocking the main loop. The main loop blinks the PC13 LED and prints uptime; the UART ISR accumulates characters until `\r` and sets a flag for command processing.

## CubeMX Configuration
| Item | Setting |
|------|---------|
| USART1 | Asynchronous, 115200 8N1, Global IT enabled |
| NVIC | USART1 global interrupt, priority 1 |
| PC13 | GPIO_Output (LED active-LOW) |

## Commands
| Command | Action |
|---------|--------|
| `led on` | Turn PC13 LED on (RESET = on) |
| `led off` | Turn PC13 LED off (SET = off) |
| `status` | Print uptime + LED state |
| `help` | Print command list |

## Key Pattern: Interrupt-Driven Receive
```c
// Setup: call once
HAL_UART_Receive_IT(&huart1, &rx_byte, 1);

// In callback: re-arm for next byte
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    // accumulate into buffer
    // when '\r' received: set cmd_ready = 1
    HAL_UART_Receive_IT(huart, &rx_byte, 1); // MUST re-arm
}

// In main loop: process cmd_ready
if (cmd_ready) { cmd_ready=0; process_command(); }
```

## Expected Output
```
Black Pill UART RX interrupt demo.
Uptime: 1s  LED: off
Uptime: 2s  LED: off
> led on
OK: LED on
Uptime: 3s  LED: on
```
