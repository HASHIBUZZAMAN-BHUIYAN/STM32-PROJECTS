# Black Pill BASIC/16 — I2S Audio Basics (PCM5102A)

## Overview
Generates a 440 Hz sine wave over I2S3 to a PCM5102A DAC. The Black Pill's PLLI2S provides the precise audio clock. This is the same project as Nucleo ADVANCED/08 but demonstrates that the Black Pill (F411) has the same I2S3 peripheral.

## I2S3 Pinout (Same as F401)
| Pin | Signal |
|-----|--------|
| PC10 | I2S3_CK (bit clock) |
| PC12 | I2S3_SD (data) |
| PA4 | I2S3_WS (word select) |
| PC7 | I2S3_MCK (master clock, optional) |

## CubeMX Configuration
| Item | Setting |
|------|---------|
| SPI3 in I2S mode | Master Transmit, Philips, 16-bit, 44100 Hz |
| PLLI2S | Auto-calculated by CubeMX for 44.1 kHz |
| DMA1 Stream5 | SPI3_TX, Circular, Half-word |

## PCM5102A Connections
| PCM5102A | Black Pill |
|---------|-----------|
| BCK | PC10 |
| DIN | PC12 |
| LRCK | PA4 |
| VIN | 3.3V |
| GND | GND |
| XSMT | 3.3V (unmute) |
| FMT | GND (I2S format) |

## Sine Wave Generation
```c
#define LUT_SIZE 100  // 100 samples/cycle → 441 Hz at 44100 Hz
int16_t lut[LUT_SIZE * 2]; // stereo L+R
// fill: lut[2i] = lut[2i+1] = (int16_t)(32767 * sinf(2π*i/LUT_SIZE))
HAL_I2S_Transmit_DMA(&hi2s3, (uint16_t*)lut, LUT_SIZE * 2);
```

## F411 vs F401 I2S Difference
None significant — both use the same I2S3 peripheral. The F411 runs at 100 MHz max vs 84 MHz for F401, which can affect PLLI2S accuracy slightly. CubeMX handles this automatically.
