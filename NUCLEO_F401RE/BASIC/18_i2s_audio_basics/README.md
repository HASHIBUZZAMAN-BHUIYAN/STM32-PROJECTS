# Nucleo-F401RE BASIC/18 — I2S Audio Tone Generation

## Overview
Generates a 440 Hz sine wave tone using I2S3 (SPI3 configured as I2S master transmitter) and outputs it to a PCM5102A I2S DAC module. A 16-sample sine LUT at 44100 Hz sample rate produces a clean 440 Hz tone (44100/100 ≈ 441 Hz). DMA provides seamless, CPU-free audio streaming.

## CubeMX Configuration

| Peripheral | Setting | Pins |
|-----------|---------|------|
| SPI3 (as I2S3) | I2S Master Transmit, 16-bit, 44100 Hz, Philips standard | PC10 (SCK), PC12 (SD/MOSI), PA4 (WS/LRCK) |
| I2S3 MCLK | Enable MCLK output | PC7 |
| DMA | SPI3_TX, DMA1 Stream 5, Channel 0, Circular, Half-word | |
| USART2 | 115200 8N1 | PA2/PA3 |

**Clock note**: I2S requires a specific PLL (PLLI2S) separate from the main PLL. In CubeMX Clock Configuration, enable PLLI2S and set it so I2S3 achieves 44100 Hz. CubeMX calculates PLLI2SN/PLLI2SR automatically — just enter 44100 Hz in the I2S config.

## Parts List
| Component | Qty | Notes |
|-----------|-----|-------|
| Nucleo-F401RE | 1 | |
| PCM5102A I2S DAC module | 1 | 3.3V compatible, line-level output |
| Headphones or powered speaker | 1 | |
| Jumper wires | 5 | |

## Wiring
| PCM5102A | Nucleo |
|----------|--------|
| VCC | 3.3V |
| GND | GND |
| BCK (bit clock) | PC10 |
| DIN (data) | PC12 |
| LCK (word select) | PA4 |
| SCK (MCLK, optional) | PC7 |
| FLT | GND |
| DMP | GND |
| FMT | GND |
| XMT | 3.3V (unmute) |

## Expected Behavior
440 Hz sine tone (concert A) plays continuously through headphones/speaker.
