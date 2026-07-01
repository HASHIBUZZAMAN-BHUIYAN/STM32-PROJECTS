# Wiring Notes — Black Pill BASIC/05: ADC Potentiometer

## Pin Assignment
| Black Pill Pin | Signal | Component |
|---------------|--------|----------|
| PA1 | ADC1_IN1 | Pot wiper |
| 3.3V | VCC | Pot pin 1 |
| GND | GND | Pot pin 3 |
| PA9 | USART1_TX | USB-UART RX |
| GND | GND | USB-UART GND |

## Wiring Diagram
```
3.3V ─── pot pin 1
         pot wiper ─── PA1
GND  ─── pot pin 3
```

## Potentiometer Selection
- 10kΩ is ideal — ADC input impedance recommendations from ST: ≤50kΩ source
- Any value 1kΩ–50kΩ works
- 3-pin trimpot or full rotary pot both work

## PA1 vs PA0
PA0 is the onboard user button (active when pressed = HIGH). To avoid conflict, use PA1 for the ADC. PA1 is a regular ADC channel (ADC1_IN1) with no board-level conflict.

## ADC Resolution
- 12-bit: 0–4095 steps
- 3.3V reference: 3300/4096 ≈ 0.806mV per step
- 10kΩ pot divides 0–3.3V linearly

## Parts List
| Component | Qty |
|-----------|-----|
| Black Pill (STM32F411CEU6) | 1 |
| 10kΩ potentiometer | 1 |
| USB-UART adapter | 1 |
| USB-C cable | 1 |
| Breadboard + jumpers | 1 set |

## 3.3V Safety
- PA1 is 3.3V analog input — NEVER exceed 3.3V (VDDA + 0.3V abs max)
- Pot between 3.3V and GND → always safe
