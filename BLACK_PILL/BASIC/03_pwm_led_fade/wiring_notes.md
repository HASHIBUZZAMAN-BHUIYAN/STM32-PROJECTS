# Wiring Notes — Black Pill ADVANCED/03: PWM LED Fade

## Pin Assignment
| Black Pill Pin | Signal | Connected To |
|---------------|--------|-------------|
| PA8 | TIM1_CH1 PWM | 330Ω → LED Anode |
| GND | GND | LED Cathode |
| PC13 | LED (active-LOW) | Onboard status LED (optional) |

## Wiring Diagram
```
PA8 ─── 330Ω ─── LED(+) ─── LED(−) ─── GND
```

## LED Current Calculation
- VDD = 3.3V, V_LED ≈ 2.0V (red), R = 330Ω
- I = (3.3 - 2.0) / 330 ≈ 4mA — safe for STM32 GPIO (max 25mA per pin)
- For brighter LED: use 220Ω (~6mA), still safe

## PWM Details
- Frequency: 1 kHz (PSC=99, ARR=999 at 100 MHz)
- Resolution: 0–1000 (0.1% steps)
- PA8 = TIM1_CH1 = Alternate Function AF1

## TIM1 Note (Advanced Timer)
TIM1 is an advanced timer with complementary outputs. For simple PWM CH1, enable the channel and call `HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1)`. No need to configure the complementary output (CH1N on PB13).

## How to Flash (No ST-Link)
1. Hold BOOT0 button
2. Briefly press/release NRST
3. Release BOOT0
4. Board shows as "STM32 BOOTLOADER" USB device
5. Use STM32CubeProgrammer in USB mode to flash

## 3.3V Safety
- PA8 is 3.3V GPIO — do NOT connect directly to 5V LEDs without a resistor
- Never exceed 3.3V on GPIO pins

## Parts List
| Component | Qty | Notes |
|-----------|-----|-------|
| Black Pill (STM32F411CEU6) | 1 | WeAct or similar |
| LED (any color) | 1 | 3mm or 5mm |
| 330Ω resistor | 1 | Current limiting |
| Breadboard + jumpers | 1 set | |
| USB-C cable | 1 | |
