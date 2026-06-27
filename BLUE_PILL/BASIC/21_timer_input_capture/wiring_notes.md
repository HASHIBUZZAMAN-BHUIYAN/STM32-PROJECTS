# Wiring Notes — 21: HC-SR04 Ultrasonic Input Capture

## CRITICAL: 3.3 V Logic
All STM32F103 GPIO pins operate at **3.3 V**. The HC-SR04 Echo pin outputs **5 V**.
Connecting 5 V directly to PA0 will damage the MCU over time (even if it seems to work initially).
**Always use a voltage divider or a bidirectional level shifter on the Echo line.**

## Voltage Divider for Echo Pin (5 V → 3.3 V)
```
HC-SR04 ECHO ──┬── 1 kΩ ──── PA0 (TIM2 CH1)
               │
              2 kΩ
               │
              GND
```
Ratio: 2 kΩ / (1 kΩ + 2 kΩ) = 0.667 → 5 V × 0.667 ≈ 3.33 V. Safe for STM32.

Alternatively use a pre-made bidirectional logic level shifter module (BSS138-based).

## Pin Connections

| Blue Pill Pin | HC-SR04 Pin | Notes |
|---------------|-------------|-------|
| GND | GND | Common ground |
| 5V (VBUS from USB) | VCC | HC-SR04 requires 5 V supply |
| PA1 | TRIG | 3.3 V signal is accepted by HC-SR04 TRIG |
| PA0 | ECHO (via divider) | **Must level-shift from 5 V to 3.3 V** |
| PA9 (TX) | USB-TTL RX | UART1 output to PC |
| GND | USB-TTL GND | |

## HC-SR04 Operating Conditions
| Parameter | Value |
|-----------|-------|
| Supply voltage | 5 V DC |
| TRIG pulse width | ≥ 10 µs |
| ECHO output voltage | 5 V (HIGH) / 0 V (LOW) |
| Minimum range | ~2 cm |
| Maximum range | ~400 cm |
| Measuring angle | 15° |

## TIM2 CH1 (PA0) Notes
- PA0 must be configured as **Alternate Function Input** in CubeMX (no pull).
- PA0 is shared with ADC1 CH0. Do not enable ADC on PA0 simultaneously.
- TIM2 on STM32F103 is a **16-bit** counter. Handle overflow in software
  (see the subtraction wraparound code in main_usercode.c).

## UART1 (USB-TTL Adapter)
| Blue Pill | USB-TTL |
|-----------|---------|
| PA9 (TX) | RX |
| GND | GND |
| (PA10 RX optional) | TX |

Terminal settings: **115200 baud, 8N1, no flow control**.

## Power
- Use the Blue Pill's 5V pin (fed from USB) to power the HC-SR04.
- Ensure the USB host can supply enough current (HC-SR04 peak ~15 mA).
