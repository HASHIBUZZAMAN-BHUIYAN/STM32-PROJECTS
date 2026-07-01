# Black Pill BASIC/14 — High Clock Speed Timing (100 MHz)

## Overview
Demonstrates timing precision at 100 MHz using TIM2 as a microsecond timer. Measures the execution time of common operations (GPIO toggle, I2C write, memcpy) using TIM2 capture to quantify the effect of the higher clock speed compared to Blue Pill (72 MHz) and Nucleo (84 MHz).

## Why 100 MHz Matters
| Board | Clock | GPIO toggle period | Min delay |
|-------|-------|-------------------|-----------|
| Blue Pill F103 | 72 MHz | 13.9ns | ~14ns |
| Nucleo F401RE | 84 MHz | 11.9ns | ~12ns |
| Black Pill F411 | 100 MHz | 10ns | ~10ns |

The F411 also has an FPU (hardware float), making it noticeably faster for signal processing.

## CubeMX Configuration
| Item | Setting |
|------|---------|
| TIM2 | PSC=99, ARR=0xFFFFFFFF → 1 MHz tick (1µs per count) |
| USART1 | PA9/PA10, 115200 8N1 |

## Microsecond Timer Pattern
```c
// Using TIM2 at 1 MHz:
uint32_t t0 = __HAL_TIM_GET_COUNTER(&htim2);
// ... operation ...
uint32_t elapsed_us = __HAL_TIM_GET_COUNTER(&htim2) - t0;
```

## FPU Demo
The F411 Cortex-M4F has a hardware FPU. CubeMX enables it by default via `-mfpu=fpv4-sp-d16 -mfloat-abi=hard`. A floating-point multiply runs in 1–2 cycles vs ~30 cycles on softfloat.

## Expected Output
```
GPIO toggle: 3us
memcpy 1KB: 10us
sin(1.234): 2us (FPU)
UART TX "hello": 86us
I2C byte write: 12us
```
All timings at 100 MHz. Multiply by 100/72 ≈ 1.39 to compare with Blue Pill equivalent times.
