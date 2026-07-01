# Wiring Notes — Nucleo-F401RE BASIC/23: DMA SPI Transfer

## ⚠️ 3.3V Logic Warning
SPI2 pins are 3.3V. SPI device must accept 3.3V logic.

## SPI2 Connections
| Signal | Nucleo Pin | Notes |
|--------|-----------|-------|
| SCK | PB13 (Arduino D13 alt) | |
| MISO | PB14 | |
| MOSI | PB15 | |
| CS | PB12 (manual GPIO) | Active LOW |

## Loopback Test
For testing without a real SPI device: jumper PB14 (MISO) to PB15 (MOSI). Use `HAL_SPI_TransmitReceive_DMA` to confirm data echoes back.

## Real SPI Device
Connect any 3.3V SPI device (MAX7219, SPI flash, SPI DAC). The DMA transfer sends 256 bytes regardless of the device — adjust `tx_buf` contents to match device protocol.

## Parts List
| Component | Qty |
|-----------|-----|
| Nucleo-F401RE | 1 |
| SPI device or jumper (loopback) | 1 |
| USB cable | 1 |
