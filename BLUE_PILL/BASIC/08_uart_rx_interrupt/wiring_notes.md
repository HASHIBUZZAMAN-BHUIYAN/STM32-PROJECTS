# Wiring Notes — 08_uart_rx_interrupt

## CRITICAL: 3.3 V Logic

**All STM32F103C8T6 GPIO pins operate at 3.3 V.**
PA9 (TX) and PA10 (RX) are 3.3 V signals.
Use a **3.3 V USB-to-TTL adapter only**. A 5 V adapter TX line will damage PA10.
If your adapter only supports 5 V output, add a 1 kΩ / 2 kΩ voltage divider
on the adapter TX → PA10 path to bring the high level down to ~3.3 V.

## Connections

| USB-TTL Adapter | Blue Pill Pin | Note |
|-----------------|---------------|------|
| GND | GND | Common ground — connect first |
| RX | PA9 | Adapter receives MCU transmit |
| TX | PA10 | Adapter transmits to MCU receive |
| VCC (3.3 V only) | — | Optional; do not power board here if USB is connected |

## Terminal Settings

| Parameter | Value |
|-----------|-------|
| Baud Rate | 115200 |
| Data bits | 8 |
| Parity | None |
| Stop bits | 1 |
| Flow control | None |
| Local echo | On (recommended, so you can see what you type) |

## NVIC Reminder

This project requires the USART1 global interrupt to be **enabled** in CubeMX.
Without it, `HAL_UART_Receive_IT` arms the hardware but the callback is never called.

Steps in CubeMX:
1. Connectivity → USART1 → NVIC Settings tab
2. Check "USART1 global interrupt"
3. Re-generate code

## Interrupt Priority Note

SysTick (used by HAL_Delay) runs at priority 15 by default.
Set USART1 interrupt priority to 0 or any value lower than SysTick's priority
(lower numeric value = higher priority) to ensure the callback is not blocked
by the SysTick handler.

## Common Mistakes

- Forgetting to re-arm `HAL_UART_Receive_IT` inside the callback — receive stops after the first byte.
- Calling `HAL_Delay` or other blocking functions inside the callback — this can deadlock.
- Not enabling the USART1 NVIC interrupt in CubeMX.
- Swapping TX/RX connections between adapter and MCU.
