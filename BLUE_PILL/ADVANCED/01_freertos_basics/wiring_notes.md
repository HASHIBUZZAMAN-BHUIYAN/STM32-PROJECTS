# Wiring Notes — 01_freertos_basics

## Voltage Warning

**ALL GPIO pins on the Blue Pill are 3.3 V logic.**
Do NOT connect 5 V signals directly to any pin without a level shifter or voltage divider.
The STM32F103C8T6 is NOT 5 V tolerant on analog pins (PA0 in ADC mode).

## Pin Usage

| Pin  | Function       | Direction | Notes                                    |
|------|----------------|-----------|------------------------------------------|
| PC13 | Onboard LED    | Output    | Active-low. RESET = ON, SET = OFF        |
| PA9  | UART1 TX       | Output    | Connect to USB-TTL adapter RX            |
| PA10 | UART1 RX       | Input     | Connect to USB-TTL adapter TX            |
| PA0  | ADC1 Ch0       | Analog In | 0–3.3 V only. Connect pot or sensor here |

## Minimal Test Setup

### LED
No external wiring needed — PC13 LED is onboard.

### UART (serial monitor)
```
Blue Pill PA9  (TX) ──► USB-TTL RX
Blue Pill PA10 (RX) ◄── USB-TTL TX
Blue Pill GND       ──► USB-TTL GND
```
Open serial terminal at **115200 baud, 8N1**.

### ADC input (optional)
```
3.3V ──┬── 10 kΩ ──┬── PA0
        |           |
       GND        10 kΩ
                    |
                   GND
```
A simple voltage divider or 10 kΩ potentiometer wiper connected between GND and 3.3 V
gives a varying 0–3.3 V signal on PA0 (ADC reads 0–4095).

## Power

Power the Blue Pill via USB or the 5 V pin (onboard 3.3 V regulator supplies the MCU).
Do not exceed 3.3 V on 3.3 V pin.

## CubeMX Clock Settings

- Input: HSE 8 MHz (crystal on Blue Pill)
- PLL multiplier: x9 → 72 MHz SYSCLK
- APB1: 36 MHz, APB2: 72 MHz
- SysTick used by FreeRTOS tick (1 ms default)
