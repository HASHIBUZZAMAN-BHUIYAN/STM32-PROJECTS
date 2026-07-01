# Nucleo-F401RE ADVANCED/08 — I2S Audio Output (PCM5102A DAC)

## Overview
Streams a 440 Hz sine wave (A4 note) to a PCM5102A I2S DAC using I2S3 with DMA circular mode. The sine LUT is pre-computed at startup. A UART command interface changes the frequency (note) in real time by rebuilding the LUT.

## I2S on STM32F401
The F401 has one dedicated I2S: **I2S3** (or SPI3 in I2S mode). The PLLI2S PLL provides the precise audio clock independent of the main PLL running at 84 MHz.

I2S3 pinout:
- PC10 — I2S3_CK (bit clock)
- PC12 — I2S3_SD (data out)
- PA4  — I2S3_WS (word select / LR clock)
- PC7  — I2S3_MCK (master clock output, optional)

## CubeMX Configuration
| Item | Setting |
|------|---------|
| I2S3 | Master Transmit, Philips standard, 16-bit, 44100 Hz |
| PLLI2S | Enable, set N/R for exact 44.1 kHz |
| DMA1 Stream5 | SPI3_TX, Circular, Half-word→Half-word |
| USART2 | PA2/PA3, 115200 8N1 |

**PLLI2S for 44.1 kHz** (approximate, F401):
- PLLI2SN=271, PLLI2SR=6 → PLLI2S output = (HSE×N/R) ÷ various dividers

CubeMX auto-calculates these when you set the I2S sample rate. Accept its values.

## PCM5102A DAC Connections
| PCM5102A Pin | Connect To |
|-------------|-----------|
| VIN (VCC) | 3.3V |
| GND | GND |
| SCK (BCK) | PC10 |
| DIN (DATA) | PC12 |
| LRCK (WS) | PA4 |
| FMT | GND (I2S Philips format) |
| DEMP | GND (no de-emphasis) |
| XSMT | 3.3V (unmute) |

## Sine LUT Strategy
```c
// 100-sample LUT at 44100 Hz → fundamental = 441 Hz ≈ A4
// For exact frequency f: samples_per_cycle = 44100/f
// Use floating-point recalc or fixed-LUT with nearest integer
#define LUT_SIZE 100
int16_t lut[LUT_SIZE * 2]; // stereo: L,R,L,R,...
// Fill: lut[2i] = lut[2i+1] = (int16_t)(32767 * sin(2π*i/LUT_SIZE))
```

## Expected Behavior
Connect PCM5102A to 3.5mm jack → speaker/headphones → hear 440 Hz tone. UART command `freq 523` changes to C5 (523 Hz).

## Supported Commands
- `freq <hz>` — set sine frequency (20–20000 Hz)
- `vol <0-100>` — scale amplitude
- `stop` — mute output
