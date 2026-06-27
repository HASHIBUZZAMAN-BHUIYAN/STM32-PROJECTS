# 25 — Multi-Peripheral Integration (BMP280 + UART + PWM LED)

## Overview
Integrates three peripherals simultaneously:
- **BMP280** temperature/pressure sensor over **I2C1** (PB6 SCL, PB7 SDA)
- **USART1** (PA9 TX) reporting temperature every 500 ms
- **TIM1 CH1 PWM** on PA8 driving an LED whose brightness scales with temperature
  (hotter → brighter)

The PWM duty cycle is mapped linearly from the temperature reading:
- ≤ 20 °C → 0 % duty (LED off)
- ≥ 40 °C → 100 % duty (LED fully on)
- Values in between are interpolated linearly.

## Hardware
- MCU: STM32F103C8T6 (Blue Pill), 72 MHz
- BMP280 module (I2C address 0x76 or 0x77 depending on SDO pin)
- External LED + 330 Ω resistor on PA8 (or use the onboard LED pathway with a separate GPIO)
- USB-TTL adapter on PA9 (TX)

## CubeMX Configuration

### I2C1
| Parameter | Value |
|-----------|-------|
| Mode | I2C |
| Speed Mode | Standard Mode (100 kHz) |
| Clock Speed | 100000 Hz |

### USART1
| Parameter | Value |
|-----------|-------|
| Baud Rate | 115200 |
| Word Length | 8 bits |
| Parity | None |
| Stop Bits | 1 |

### TIM1 Channel 1 — PWM
| Parameter | Value | Notes |
|-----------|-------|-------|
| Mode | PWM Generation CH1 | |
| Prescaler (PSC) | 71 | 1 µs tick (72 MHz / 72 = 1 MHz) |
| Counter Period (ARR) | 999 | 1 kHz PWM (1 MHz / 1000) |
| Pulse (CCR1) | 0 | Start at 0 % duty |
| CH Polarity | High | |

### GPIO
| Pin | Mode | Label |
|-----|------|-------|
| PB6 | I2C1_SCL | |
| PB7 | I2C1_SDA | |
| PA8 | TIM1_CH1 (AF Push-Pull) | PWM LED |
| PA9 | USART1_TX | |
| PA10 | USART1_RX | |
| PC13 | GPIO_Output | Onboard LED (active-low) |

## BMP280 Register Summary (used in code)
| Register | Address | Description |
|----------|---------|-------------|
| chip_id | 0xD0 | Should read 0x60 |
| ctrl_meas | 0xF4 | Oversampling + mode |
| config | 0xF5 | Standby time, filter |
| press_msb | 0xF7 | Pressure raw [19:12] |
| temp_msb | 0xFA | Temperature raw [19:12] |
| calib00 | 0x88 | Start of 24-byte trim parameters |

The compensation formula follows the BMP280 datasheet Appendix (integer version).
Trim parameters (dig_T1…dig_T3, dig_P1…dig_P9) are read from the sensor at startup.

## PWM Duty Mapping
```
temp_clamped = clamp(temp_celsius, 20.0f, 40.0f)
duty_percent = (temp_clamped - 20.0f) / 20.0f * 100.0f
CCR1 = (uint32_t)(duty_percent * (ARR + 1) / 100.0f)
```

## Notes
- This code has NOT been tested on hardware.
- BMP280 I2C address: 0x76 if SDO → GND, 0x77 if SDO → VCC. Adjust `BMP280_I2C_ADDR` define.
- BMP280 VCC: 3.3 V. Do NOT connect to 5 V.
- Pull-up resistors (4.7 kΩ) required on SDA and SCL lines.
- PA8 (TIM1 CH1) is a 3.3 V output. The external LED must tolerate 3.3 V drive.
- TIM1 is an Advanced-Control timer; call `HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1)` and also
  ensure the Main Output Enable (MOE) bit is set — `HAL_TIMEx_PWMN_Start` or setting
  `TIM1->BDTR |= TIM_BDTR_MOE` may be needed if CubeMX does not set it automatically.
