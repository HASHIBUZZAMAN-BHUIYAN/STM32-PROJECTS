# 21 — Timer Input Capture (HC-SR04 Ultrasonic Distance)

## Overview
Measures distance using an HC-SR04 ultrasonic sensor. A 10 µs trigger pulse is sent on PA1.
The echo pulse returns on PA0, which is configured as TIM2 Channel 1 in Input Capture mode.
The pulse width in microseconds is converted to centimeters and printed over UART1.

## Hardware
- MCU: STM32F103C8T6 (Blue Pill), 72 MHz
- Sensor: HC-SR04 ultrasonic module
- UART1: PA9 (TX) → USB-TTL adapter for PC terminal

## CubeMX Configuration

### TIM2
| Parameter | Value |
|-----------|-------|
| Channel 1 | Input Capture direct mode |
| Prescaler (PSC) | 71 (1 µs tick at 72 MHz: 72 MHz / (71+1) = 1 MHz) |
| Counter Period (ARR) | 65535 |
| IC1 Polarity | Rising Edge (first capture), toggled in code to falling |
| IC1 Filter | 0 |
| IC1 Prescaler | No divide |

### GPIO
| Pin | Mode | Label |
|-----|------|-------|
| PA0 | TIM2_CH1 (Alternate Function Input) | ECHO |
| PA1 | GPIO_Output, Push-Pull, No Pull | TRIG |
| PA9 | USART1_TX | |
| PA10 | USART1_RX | |

### USART1
- Baud: 115200, 8N1, no flow control

### NVIC
- TIM2 global interrupt: Enabled

## Formula
```
distance_cm = pulse_width_us / 58
```
Speed of sound ≈ 343 m/s. Round-trip 1 cm ≈ 58 µs.

## Algorithm
1. Pull TRIG high for 10 µs, then low.
2. Start Input Capture on rising edge → record `t1`.
3. Switch capture polarity to falling edge → record `t2`.
4. `width_us = t2 - t1` (handle timer overflow/wraparound).
5. `distance_cm = width_us / 58`.
6. Print result over UART1 every ~100 ms.

## Notes
- This code has NOT been tested on hardware.
- HC-SR04 operates at 5 V logic; the Echo pin outputs 5 V. Use a voltage divider (1 kΩ + 2 kΩ) or level shifter before PA0.
- Maximum reliable range: ~400 cm. Minimum: ~2 cm.
- Keep trigger pulse exactly 10 µs; shorter pulses may not fire the sensor.
