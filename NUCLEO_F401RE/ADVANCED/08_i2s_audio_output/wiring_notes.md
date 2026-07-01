# Wiring Notes — Nucleo-F401RE ADVANCED/08: I2S Audio

## Pin Assignment
| Nucleo Pin | I2S3 Signal | PCM5102A Pin |
|-----------|------------|-------------|
| PC10 | I2S3_CK (BCK) | BCK |
| PC12 | I2S3_SD (DATA) | DIN |
| PA4 | I2S3_WS (LRCK) | LRCK |
| PC7 | I2S3_MCK | SCK (if needed) |
| 3.3V | — | VIN, XSMT (unmute) |
| GND | — | GND, FMT, DEMP |

## PCM5102A Strapping Pins
| Pin | Connect To | Purpose |
|-----|-----------|---------|
| FMT | GND | I2S Philips format (not left-justified) |
| DEMP | GND | No de-emphasis filter |
| XSMT | 3.3V | Unmute DAC output |
| SCK | PC7 (MCK) or leave open | System clock |

Most PCM5102A breakout boards have these strapping pins pulled correctly via onboard resistors. Check your board's datasheet.

## Audio Output
PCM5102A has a 3.5mm stereo jack. Connect:
- 3.5mm jack → powered speakers or headphones

The PCM5102A output is line-level (~2V RMS). It can drive headphones directly but a headphone amplifier improves quality.

## PLLI2S Setup in CubeMX
1. Enable PLLI2S under Clock Configuration
2. Set I2S3 Sample Rate = 44100 Hz
3. CubeMX will suggest PLLI2SN and PLLI2SR values
4. The actual achieved rate may be 44118 Hz (±0.04%) — acceptable for audio

## I2S3 vs SPI3
I2S3 and SPI3 share the same hardware block. In CubeMX, enabling SPI3 in I2S mode automatically configures the I2S protocol. Do NOT also enable SPI3 in SPI mode.

## 3.3V Safety
All I2S signals are 3.3V. PCM5102A operates at 3.3V. No level shifting needed.

## Parts List
| Component | Qty | Notes |
|-----------|-----|-------|
| Nucleo-F401RE | 1 | |
| PCM5102A I2S DAC module | 1 | GY-PCM5102 breakout |
| Speakers or headphones | 1 | 3.5mm stereo |
| Jumper wires | 6 | |
| USB cable | 1 | |
