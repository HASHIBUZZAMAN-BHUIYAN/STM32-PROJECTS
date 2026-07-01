# Wiring Notes — Black Pill BASIC/08: UART RX Interrupt

## Pin Assignment
| Black Pill Pin | Signal | USB-UART Adapter |
|---------------|--------|-----------------|
| PA9 | USART1_TX | RX |
| PA10 | USART1_RX | TX |
| GND | GND | GND |
| PC13 | LED (active-LOW, onboard) | — |

## No Extra Components
Uses only onboard PC13 LED and external USB-UART adapter for communication.

## Terminal Usage
1. Open terminal (PuTTY, TeraTerm, VSCode Serial Monitor)
2. Set 115200 baud, 8N1, no flow control
3. Enable local echo (so you see what you type)
4. Type `help` + Enter to see commands

## re-arm Pattern Critical Note
After `HAL_UART_Receive_IT()` fires once, it automatically disables the interrupt. You MUST call `HAL_UART_Receive_IT()` again at the end of `HAL_UART_RxCpltCallback()` to receive the next byte. Forgetting this is the most common UART RX interrupt bug.

## Parts List
| Component | Qty |
|-----------|-----|
| Black Pill (STM32F411CEU6) | 1 |
| USB-UART adapter (3.3V logic) | 1 |
| USB-C cable | 1 |
| Jumper wires | 3 |
