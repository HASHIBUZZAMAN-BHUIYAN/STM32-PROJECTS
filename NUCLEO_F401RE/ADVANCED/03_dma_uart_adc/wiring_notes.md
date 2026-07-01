# Wiring Notes — Nucleo-F401RE ADVANCED/03: DMA UART + ADC

## Pin Assignment
| Nucleo Pin | Signal | Notes |
|-----------|--------|-------|
| PA0 | ADC1_IN0 | Analog input — connect pot or leave open (reads noise) |
| PA2/PA3 | USART2 TX/RX | ST-Link VCP, automatic |
| PA5 | LD2 | Heartbeat blink |

## Test Setup
Connect a 10kΩ potentiometer between 3.3V and GND, wiper to PA0. Rotating the pot sweeps ADC output 0–4095 (0–3.3V).

```
3.3V ─── pot pin 1
         pot wiper ─── PA0
GND ──── pot pin 3
```

## DMA Channel Conflict Rule
Each DMA stream can be assigned to only one channel/peripheral at a time. On F401:
- DMA2 Stream0: shared between ADC1 CH0 and others — verify in CubeMX there are no conflicts
- DMA1 Stream6: USART2_TX CH4 — check no other peripheral claims this stream

CubeMX DMA tab highlights conflicts in red. Resolve before generating code.

## uart_dma_busy Flag
`HAL_UART_Transmit_DMA` returns immediately but the DMA is still running. Calling it again before TX completes corrupts the transmit buffer. The `uart_dma_busy` flag (cleared in `HAL_UART_TxCpltCallback`) guards against this.

## Parts List
| Component | Qty | Notes |
|-----------|-----|-------|
| Nucleo-F401RE | 1 | |
| 10kΩ potentiometer | 1 | Test analog input |
| Breadboard + jumpers | 1 set | |
| USB cable | 1 | |

## 3.3V Safety
PA0 is an analog input — NEVER exceed 3.3V. Absolute max is VDD+0.3V ≈ 3.6V. A pot between 3.3V and GND is safe.
