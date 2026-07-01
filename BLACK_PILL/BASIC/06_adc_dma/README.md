# Black Pill BASIC/06 — ADC DMA Continuous Sampling

## Overview
Configures ADC1 to sample two channels (PA1=IN1 and the internal temperature sensor=IN18) continuously using DMA circular mode. The CPU never touches the ADC — it just reads the DMA buffer at will. Prints both readings to USART1 every second.

## CubeMX Configuration
| Item | Setting |
|------|---------|
| ADC1 | IN1 (PA1) + IN18 (internal temp), Scan mode, Continuous |
| DMA2 Stream0 | ADC1, Circular, Half-word→Half-word |
| USART1 | PA9/PA10, 115200 8N1 |

## Multi-Channel ADC Scan Order
CubeMX assigns rank:
- Rank 1: IN1 (PA1) → adc_buf[0]
- Rank 2: IN18 (temp sensor) → adc_buf[1]

DMA fills both slots, then wraps and overwrites continuously.

## Internal Temperature Sensor
STM32F411 has a built-in temperature sensor connected to ADC1_IN18:
```c
// Temperature formula from STM32F411 datasheet:
// T(°C) = ((V_SENSE - V25) / Avg_Slope) + 25
// V25 ≈ 0.76V, Avg_Slope ≈ 2.5mV/°C
float v_sense = (adc_buf[1] * 3300.0f) / 4096.0f; // mV
float temp_c  = ((v_sense - 760.0f) / 2.5f) + 25.0f;
```

## Expected Output
```
PA1=2048(1650mV)  MCU_temp=27.3C
PA1=3100(2500mV)  MCU_temp=27.4C
```

## ADC DMA on F411
ADC1 uses DMA2 Stream0 (or Stream4) Channel 0. Verify in CubeMX — DMA2 is required for ADC on F4 series.
