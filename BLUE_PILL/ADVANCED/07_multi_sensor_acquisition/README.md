# 07 — Multi-Sensor Acquisition

## Overview
Deterministic data acquisition system using TIM3 at 1 kHz to trigger simultaneous ADC DMA sampling and scheduled BMP280 I2C reads. Every 100 ms (100 samples), computes min/max/avg from the ADC buffer and prints a summary over UART1 using DMA.

## Architecture

```
TIM3 ISR (1 kHz)
  ├─ ADC1 DMA → fills 100-sample circular buffer (automatic)
  └─ Sets g_i2c_pending flag every 100th tick

Main Loop
  ├─ If g_i2c_pending: read BMP280 temp via I2C (blocking OK here)
  └─ If g_buf_ready: compute min/max/avg, format string, UART DMA TX
```

**Key principle:** I2C and UART operations are NEVER done inside the ISR. Only lightweight flag-setting occurs in interrupt context.

## Sampling Details
| Parameter         | Value                                  |
|-------------------|----------------------------------------|
| TIM3 period       | 1 ms (1 kHz)                           |
| ADC buffer depth  | 100 samples                            |
| Report interval   | 100 ms (every 100 TIM3 interrupts)     |
| ADC channel       | ADC1 Ch0 (PA0), 12-bit                 |
| ADC trigger       | Software start (DMA continuous)        |
| UART baud         | 115200                                 |
| UART TX           | DMA (non-blocking)                     |

## DMA Behaviour
- ADC1 uses DMA1 Channel 1 in circular mode.
- When the half-complete and full-complete callbacks fire, a snapshot of the 100-sample buffer is flagged for processing.
- UART DMA uses DMA1 Channel 4 (USART1 TX). A `g_uart_tx_busy` guard prevents overrun.

## Output Format (UART, every 100 ms)
```
ADC: min=412 max=3891 avg=2103 | BMP280: 25.14 C
```

## CubeMX Configuration
| Peripheral | Setting                                                   |
|------------|-----------------------------------------------------------|
| TIM3       | PSC=71, ARR=999 → 1 kHz; NVIC interrupt enabled          |
| ADC1       | Ch0 (PA0), DMA Continuous, DMA1 Ch1, half-word, circular |
| I2C1       | Standard 100 kHz, PB6=SCL, PB7=SDA                       |
| USART1     | 115200 8N1, DMA TX on DMA1 Ch4                            |
| NVIC       | TIM3 global interrupt priority 6; DMA priorities default  |
| SYS        | Timebase: SysTick or TIM4                                 |

## Files
| File              | Purpose                      |
|-------------------|------------------------------|
| `main_usercode.c` | All user code sections       |
| `wiring_notes.md` | Hardware connection guide    |

## Not Tested
This code has not been compiled or run on hardware. It is provided as a reference implementation for study and adaptation.
