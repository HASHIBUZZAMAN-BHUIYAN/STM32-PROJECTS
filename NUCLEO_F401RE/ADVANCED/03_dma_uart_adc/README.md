# Nucleo-F401RE ADVANCED/03 — DMA: UART TX + ADC Circular

## Overview
Demonstrates two DMA use cases simultaneously: ADC1 in DMA circular mode filling a double-buffer of 64 samples, and UART TX using DMA so the CPU is free while data is transmitted. A TIM2 trigger fires ADC conversions at 1 kHz. Every 100ms the CPU computes an average from the completed DMA half-buffer and sends a formatted string over UART DMA.

## DMA Channels (STM32F401)
| Peripheral | DMA | Stream | Channel | Direction |
|-----------|-----|--------|---------|-----------|
| ADC1 | DMA2 | Stream 0 | Channel 0 | Periph→Memory |
| USART2_TX | DMA1 | Stream 6 | Channel 4 | Memory→Periph |

## CubeMX Configuration
| Peripheral | Settings |
|-----------|---------|
| ADC1 IN0 | PA0, Continuous+DMA, TIM2 TRGO trigger, 12-bit |
| DMA2 Stream0 | ADC1, Circular, Half-word→Half-word |
| USART2 | PA2/PA3, 115200 8N1, DMA TX enabled |
| DMA1 Stream6 | USART2_TX, Normal (not circular) |
| TIM2 | MasterMode=Update (TRGO), 1 kHz |

## Double-Buffer ADC Pattern
```c
uint16_t adc_buf[128]; // DMA fills this continuously (circular)
// HAL_ADC_ConvHalfCpltCallback: first 64 samples ready
// HAL_ADC_ConvCpltCallback:     second 64 samples ready
// Never read same half that DMA is currently writing
```

## Expected Output
```
ADC avg=2048 (1.65V) tick=100
ADC avg=2051 (1.65V) tick=200
```

## CPU Load Comparison
| Method | CPU during ADC | CPU during UART TX |
|--------|---------------|-------------------|
| Polling | 100% (busy-wait) | 100% (busy-wait) |
| Interrupt | ~5% (ISR overhead) | ~5% |
| DMA (this project) | ~1% (half/full callbacks) | ~0.1% |

At 1 kHz ADC + UART TX every 100ms, DMA frees ~90% of CPU time vs polling.
