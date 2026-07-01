# Wiring Notes — Nucleo-F401RE BASIC/18: I2S Audio

## ⚠️ 3.3V Logic Warning
All I2S signals are 3.3V. PCM5102A accepts 3.3V signals. Do not use 5V I2C/SPI devices on these pins.

## PCM5102A DAC Wiring
| PCM5102A Pin | Nucleo Pin | Notes |
|-------------|-----------|-------|
| VCC | 3.3V | |
| GND | GND | |
| BCK | PC10 | I2S3 clock |
| DIN | PC12 | I2S3 data |
| LCK (LRCK/WS) | PA4 | Word select / frame sync |
| SCK (MCLK) | PC7 | Master clock (optional) |
| FLT | GND | Normal roll-off filter |
| DMP | GND | Normal de-emphasis off |
| FMT | GND | I2S format |
| XMT | 3.3V | Un-mute (mute if GND) |

## Audio Output
Connect headphones or powered speakers to the PCM5102A LOUT/ROUT pins. Line-level output — use powered speakers or a headphone amplifier.

## Parts List
| Component | Qty |
|-----------|-----|
| Nucleo-F401RE | 1 |
| PCM5102A I2S DAC module | 1 |
| Headphones / powered speaker | 1 |
| Jumper wires | 5-6 |

## CubeMX I2S Clock Note
I2S requires PLLI2S to be enabled separately from the main PLL. In CubeMX → Clock Configuration:
1. Enable I2S PLL (PLLI2S)
2. Set I2S clock source to PLLI2S
3. CubeMX auto-calculates dividers for 44100 Hz

Without correct PLLI2S config, audio will be off-pitch.
