# Wiring Notes — Black Pill BASIC/06: ADC DMA

## Pin Assignment
| Black Pill Pin | Signal | Component |
|---------------|--------|----------|
| PA1 | ADC1_IN1 | Pot wiper (or short to GND/3.3V for fixed test) |
| 3.3V | VCC | Pot pin 1 |
| GND | GND | Pot pin 3 |
| PA9 | USART1_TX | USB-UART adapter RX |
| GND | GND | USB-UART GND |

## Internal Temperature Sensor
No external wiring needed — channel IN18 is internal. Enable via `ADC->CCR |= ADC_CCR_TSVREFE` before starting ADC.

The internal sensor has ±5°C accuracy. Not suitable for precision applications — use BMP280 or DS18B20 instead.

## DMA2 Requirement (F4 Series)
On STM32F4, ADC can only use DMA2 (not DMA1). CubeMX enforces this. If you see a DMA conflict, check that ADC1 is assigned to DMA2 Stream0 or Stream4, Channel 0.

## Parts List
| Component | Qty |
|-----------|-----|
| Black Pill (STM32F411CEU6) | 1 |
| 10kΩ potentiometer | 1 |
| USB-UART adapter | 1 |
| USB-C cable | 1 |
| Breadboard + jumpers | 1 set |

## 3.3V Safety
PA1 analog input: never exceed 3.3V.
