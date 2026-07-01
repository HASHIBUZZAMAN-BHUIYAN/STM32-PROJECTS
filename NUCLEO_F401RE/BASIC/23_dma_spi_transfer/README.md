# Nucleo-F401RE BASIC/23 — DMA SPI Transfer

## Overview
Sends a 256-byte buffer to a SPI device using DMA, freeing the CPU to do other work during the transfer. Uses SPI2 (PB13-15) + DMA1 Stream4 Channel0 (SPI2_TX). CPU blinks LD2 independently during DMA transfer to prove non-blocking operation.

## CubeMX Configuration

| Peripheral | Setting | Pins |
|-----------|---------|------|
| SPI2 | Full-Duplex Master, 8-bit, CPOL=0 CPHA=0 | PB13 (SCK), PB14 (MISO), PB15 (MOSI) |
| DMA | SPI2_TX → DMA1 Stream4 Ch0, Byte, Memory-to-Periph | |
| GPIO PB12 | Output (CS) | |
| GPIO PA5 | Output (LD2) | |
| USART2 | 115200, PA2/PA3 | |

**DMA callback**: Override `HAL_SPI_TxCpltCallback` in USER CODE.

## Expected Behavior
- LD2 blinks at ~200ms while SPI DMA transfer runs
- UART prints "DMA SPI TX done" on completion callback
- If loopback (MISO=MOSI), optionally receive simultaneously with `HAL_SPI_TransmitReceive_DMA`
