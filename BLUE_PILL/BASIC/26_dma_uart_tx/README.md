# 26 — DMA UART TX (ADC Readings via DMA)

## Overview

This project demonstrates how to use DMA (Direct Memory Access) to transmit UART data without blocking the CPU. ADC readings from PA0 (ADC1 Channel 0) are sampled, formatted into a 256-byte string buffer, and sent out USART1 (PA9 TX) using `HAL_UART_Transmit_DMA`. The CPU is free to do other work while the DMA controller handles the transfer.

## MCU

STM32F103C8T6 (Blue Pill), 72 MHz

## Key Concepts

- DMA1 Channel 4 is the hardware-mapped channel for USART1 TX on the STM32F103.
- `HAL_UART_Transmit_DMA` is non-blocking: it kicks off the transfer and returns immediately.
- `HAL_UART_GetState` is polled to check `HAL_UART_STATE_READY` before starting a new transfer.
- ADC is polled (blocking) with `HAL_ADC_PollForConversion`; DMA is only on the UART side.

## CubeMX Configuration

| Peripheral | Setting |
|---|---|
| USART1 | Mode: Asynchronous, Baud: 115200, 8N1 |
| USART1 DMA | Add DMA Request: USART1_TX, Channel: DMA1 Ch4, Direction: Memory-to-Peripheral, Mode: Normal |
| ADC1 | Channel 0 (PA0), 12-bit, software trigger |
| GPIO | PA0 set to Analog input |
| Clock | HSE 8 MHz → PLL → 72 MHz |

## Expected Behavior

Every 500 ms the firmware:
1. Samples ADC1 Ch0 10 times and averages the readings.
2. Formats a line: `ADC[0]: XXXX (Xmv)\r\n` into the TX buffer.
3. Waits until any previous DMA transfer is complete.
4. Fires off `HAL_UART_Transmit_DMA`.

Open a serial terminal at **115200 8N1** on the USART1 TX pin (PA9) to see output.

## Files

| File | Purpose |
|---|---|
| `main_usercode.c` | User code sections only (paste into CubeMX-generated main.c) |
| `wiring_notes.md` | Hardware connections and voltage notes |

## Note

This code has not been tested on hardware. Verify DMA initialization in CubeMX matches DMA1 Channel 4 for USART1 TX before building.
