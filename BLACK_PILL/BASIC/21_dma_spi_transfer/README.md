# Black Pill BASIC/21 — DMA SPI Transfer

## Overview
Uses SPI1 with DMA to transfer data to a W25Q32 (or similar) SPI flash chip without CPU involvement during the transfer. Demonstrates the DMA TX-only pattern for SPI peripherals, measuring transfer time with TIM2.

## SPI1 DMA on Black Pill
| Peripheral | DMA | Stream | Channel | Direction |
|-----------|-----|--------|---------|-----------|
| SPI1_TX | DMA2 | Stream3 | Channel 3 | Memory→Periph |
| SPI1_RX | DMA2 | Stream0 | Channel 3 | Periph→Memory |

## CubeMX Configuration
| Item | Setting |
|------|---------|
| SPI1 | Full-Duplex Master, 8-bit, CPOL=0 CPHA=0 |
| SPI1 baud | /8 = 12.5 MHz |
| DMA2 Stream3 | SPI1_TX, Normal, Byte→Byte |
| DMA2 Stream0 | SPI1_RX, Normal, Byte→Byte |
| GPIO PA4 | CS (Output) |
| TIM2 | 1 MHz for timing |
| USART1 | PA9/PA10, 115200 8N1 |

## SPI DMA Pattern
```c
// TX only (reading dummy RX):
volatile uint8_t dummy_rx[256];
HAL_SPI_TransmitReceive_DMA(&hspi1, tx_buf, (uint8_t*)dummy_rx, 256);
// Wait for callback
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) { tx_done = 1; }
```

## W25Q32 Commands Demonstrated
- Write Enable (0x06)
- Read Status Register (0x05)
- Page Program (0x02) — write 256 bytes
- Read Data (0x03) — read back and verify
- Chip Erase (0x60) — 30 seconds, skip in demo

## Expected Output
```
SPI1 DMA TX test: 256 bytes
DMA TX time: 24us (vs ~200us polling at same speed)
W25Q32: manufacturer=EF device=4016
Write+Read OK: 256 bytes verified
```
