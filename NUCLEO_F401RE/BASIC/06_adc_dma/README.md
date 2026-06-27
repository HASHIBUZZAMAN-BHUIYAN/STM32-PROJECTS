# BASIC/06 — ADC with DMA (Circular Buffer)

Reads ADC1 Channel 0 (PA0) continuously using DMA in circular mode into a 32-sample buffer. The main loop computes the average every 500 ms and prints the result via UART2.

## CubeMX Configuration
- Board: NUCLEO-F401RE
- Analog → ADC1:
  - IN0: IN0 Single-Ended
  - Resolution: 12 bits
  - Scan Conversion Mode: Disabled
  - Continuous Conversion Mode: Enabled
  - DMA Continuous Requests: Enabled
  - End Of Conversion: EOC flag at end of single channel conversion
  - Sampling Time: 84 Cycles
- ADC1 → DMA Settings → Add:
  - DMA Request: ADC1
  - Stream: DMA2 Stream 0
  - Channel: Channel 0
  - Direction: Peripheral To Memory
  - Priority: Low
  - Mode: Circular
  - Data Width: Word (peripheral) → Half Word (memory) — use uint16_t buffer
  - Memory Increment: Enabled
- Connectivity → USART2: Async, 115200 baud, 8N1
- Clock: 84 MHz

## Parts List
- Same as project 05 (potentiometer on PA0)

## Wiring
Same as project 05: potentiometer wiper → PA0.

## Expected Behavior
Terminal prints averaged ADC value every 500 ms. Value updates smoothly as pot is turned. DMA fills buffer autonomously — CPU load near zero for acquisition.
