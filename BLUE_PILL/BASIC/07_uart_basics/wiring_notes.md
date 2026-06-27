# Wiring Notes — 07_uart_basics

## CRITICAL: 3.3 V Logic

**All STM32F103C8T6 GPIO pins operate at 3.3 V.**
The USART1 TX/RX pins (PA9, PA10) are 3.3 V logic.
Use a **3.3 V USB-to-TTL adapter**. A 5 V adapter will damage PA10 (RX).
If you only have a 5 V adapter, add a voltage divider on the adapter TX → PA10 line.

## USB-to-TTL Adapter Connections

| USB-TTL Adapter Pin | Blue Pill Pin | Note |
|---------------------|---------------|------|
| GND | GND | Common ground — always connect first |
| RX | PA9 (USART1 TX) | Adapter receives what MCU sends |
| TX | PA10 (USART1 RX) | Adapter sends what MCU receives |
| 3V3 (optional) | — | Do not power the board from adapter 3V3 AND USB simultaneously |

## Terminal Settings

| Parameter | Value |
|-----------|-------|
| Baud Rate | 115200 |
| Data bits | 8 |
| Parity | None |
| Stop bits | 1 |
| Flow control | None |

## Cross-over Reminder

TX connects to RX and RX connects to TX — this is intentional.
MCU transmits on PA9, adapter receives on its RX.
Adapter transmits on its TX, MCU receives on PA10.

## Common Mistakes

- Swapping TX and TX (connecting TX to TX) — no data will appear.
- Using a 5 V FTDI adapter without level shifting on the RX line.
- Forgetting the common GND wire — serial will not work without it.
- Wrong baud rate in the terminal (must be 115200).

## ST-Link Coexistence

ST-Link uses SWD (PA13/PA14 and SWCLK/SWDIO pins), not UART.
USART1 on PA9/PA10 can be used at the same time as ST-Link for debugging.
