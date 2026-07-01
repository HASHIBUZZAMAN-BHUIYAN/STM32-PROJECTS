# Black Pill ADVANCED/06 — UART Framed Binary Protocol

## Overview
Implements a length-prefixed binary framing protocol over USART1. Demonstrates CRC16-validated frames, DMA RX in circular mode for zero-copy reception, and a command dispatch table — patterns found in real embedded products.

## Frame Format
```
[0x55][0xAA][LEN_H][LEN_L][TYPE][PAYLOAD...][CRC16_H][CRC16_L]
  SOF         length       cmd   0..252 bytes   CRC16/CCITT
```
- SOF: 2-byte sync (0x55, 0xAA)
- LEN: total payload+type length (big-endian uint16)
- TYPE: command byte
- CRC covers everything after SOF (LEN + TYPE + PAYLOAD)

## Command Types
| TYPE | Name | Payload | Response |
|------|------|---------|----------|
| 0x01 | PING | none | ACK + uptime ms |
| 0x02 | READ_TEMP | none | ACK + float32 temperature |
| 0x03 | READ_ALL | none | ACK + struct: temp+pressure+tick |
| 0x04 | SET_LED | 1 byte (0=off,1=on) | ACK |
| 0xFF | NACK | 1 byte error code | (sent by device on error) |

## DMA RX Strategy
- USART1 DMA1 Stream2 circular, 64-byte ring buffer
- Software head pointer tracks where new data arrived
- No ISR overhead — main loop polls DMA NDTR to detect new bytes

## CubeMX
USART1 DMA RX (circular, 64 bytes), I2C1 PB6/PB7 for BMP280, TIM11 timebase.
