# Black Pill BASIC/05 — ADC Potentiometer Reading

## Overview
Reads a 10kΩ potentiometer on PA1 (ADC1_IN1) using polling mode. Converts the 12-bit ADC value to millivolts and prints both to USART1 every 500ms. The onboard PC13 LED brightness is simulated in the serial output (no PWM in this project).

## MCU Facts
| Item | Value |
|------|-------|
| MCU | STM32F411CEU6 |
| ADC | ADC1, 12-bit, 0–4095 |
| Reference | VDDA = 3.3V |
| Sample rate | Every 500ms (polling) |

## CubeMX Configuration
| Item | Setting |
|------|---------|
| ADC1 IN1 | PA1, Single Conversion, Polling |
| Sample Time | 84 cycles (good for high-impedance sources) |
| Resolution | 12-bit |
| USART1 | PA9/PA10, 115200 8N1 |

## Conversion Formula
```
voltage_mV = (adc_raw * 3300) / 4096
```

## Expected Output
```
ADC: 2048 = 1650mV (50%)
ADC: 4090 = 3298mV (100%)
ADC: 102  = 82mV (2%)
```

## Why PA1 Not PA0?
PA0 is used by the onboard button on WeAct Black Pill. PA1 is the next available analog-capable pin.
