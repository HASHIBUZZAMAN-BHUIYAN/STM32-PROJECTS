# Wiring Notes — Black Pill BASIC/28: Servo Motor PWM

## Pin Assignment
| Black Pill Pin | Signal | Servo Wire |
|---------------|--------|-----------|
| PA0 | TIM2_CH1 PWM | Signal (usually white/yellow/orange) |
| GND | GND | Ground (black/brown) |
| External 5V | Servo VCC | Power (red) |

## Critical: Separate Power Supply for Servo
Servos draw 200mA–1A during movement. Powering from the Black Pill 3.3V or USB 5V pin risks:
1. Voltage drop causing STM32 reset
2. USB port current limit trip (500mA)

Use a separate 4.8–6V power source (AA batteries or 5V wall adapter):

```
Black Pill PA0 ─── Servo SIGNAL (white)
Black Pill GND ─┬─ Servo GND (black)
                └─ External 5V GND (SHARED GROUND REQUIRED)
External 5V ──── Servo VCC (red)
```

## PA0 = Servo, Not Button
On WeAct Black Pill, PA0 is the user button AND TIM2_CH1. This project uses PA0 as PWM output, so the button cannot be used simultaneously. Control via USART1 instead.

## Servo Control Cable Colors
| Color | Signal |
|-------|--------|
| Brown/Black | GND |
| Red/Orange | VCC (4.8-6V) |
| White/Yellow/Orange | PWM Signal |

Colors vary by manufacturer — check your servo's datasheet.

## 3.3V Signal Compatibility
Most servos accept 3.3V control signals even when powered at 5V. However, for best reliability:
- Use a level shifter or voltage divider if your servo requires strict 5V signal
- Most modern hobby servos (SG90, MG996R) work fine with 3.3V signal

## Servo Types
| Servo | Torque | Speed | Power |
|-------|--------|-------|-------|
| SG90 (micro) | 1.8kg·cm | 0.1s/60° | 250mA |
| MG996R | 10kg·cm | 0.2s/60° | 1A peak |
| DS3218 | 20kg·cm | 0.17s/60° | 1.5A peak |

SG90 is good for testing — low current, compatible with USB 5V power.

## Parts List
| Component | Qty | Notes |
|-----------|-----|-------|
| Black Pill (STM32F411CEU6) | 1 | |
| Servo motor (SG90 or similar) | 1 | |
| External 5V power supply | 1 | For servo |
| USB-UART adapter | 1 | For USART1 control |
| USB-C cable | 1 | For Black Pill power |
| Jumper wires | 3 | |
