# Wiring Notes — Black Pill BASIC/16: I2S Audio Basics

## Pin Assignment
| Black Pill Pin | I2S3 Signal | PCM5102A |
|---------------|------------|---------|
| PC10 | I2S3_CK (BCK) | BCK |
| PC12 | I2S3_SD (DATA) | DIN |
| PA4 | I2S3_WS (LRCK) | LRCK |
| PC7 | I2S3_MCK | SCK (optional) |
| 3.3V | — | VIN, XSMT |
| GND | — | GND, FMT, DEMP |

## PCM5102A Strapping
| Pin | Connect |
|-----|---------|
| FMT | GND (I2S Philips format) |
| DEMP | GND (no de-emphasis) |
| XSMT | 3.3V (unmute) |

## Audio Output
PCM5102A → 3.5mm jack → headphones or powered speakers.

## PLLI2S Setup
In CubeMX Clock Configuration:
1. Enable PLLI2S
2. Set I2S3 sample rate = 44100 Hz
3. CubeMX auto-calculates PLLI2SN/PLLI2SR
4. Achieved rate typically 44117 Hz (0.04% error — fine for audio)

## Parts List
| Component | Qty |
|-----------|-----|
| Black Pill (STM32F411CEU6) | 1 |
| PCM5102A I2S DAC module | 1 |
| Headphones or speakers | 1 |
| USB-C cable | 1 |
| Jumper wires | 5 |

## 3.3V Safety
All I2S signals are 3.3V. PCM5102A is 3.3V compatible.
