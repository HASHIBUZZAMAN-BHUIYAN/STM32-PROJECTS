# Wiring Notes — 25: Multi-Peripheral Integration (BMP280 + UART + PWM LED)

## CRITICAL: 3.3 V Logic
All STM32F103 GPIO pins operate at **3.3 V**. Do not connect any 5 V signal directly to any GPIO pin.
- BMP280 VCC: **3.3 V only**. Many BMP280 breakout boards include an onboard 3.3 V regulator
  and level shifters, accepting 3.3 V–5 V on VCC. Check your specific module datasheet.
  When in doubt, connect VCC to the Blue Pill's 3.3 V pin.
- The external LED on PA8 is driven at 3.3 V. Choose a resistor accordingly.

## Schematic

### BMP280 (I2C1)
```
Blue Pill 3.3V ──── BMP280 VCC
Blue Pill GND  ──── BMP280 GND
PB6 (SCL) ──┬──── BMP280 SCL
            │
           4.7 kΩ
            │
           3.3V

PB7 (SDA) ──┬──── BMP280 SDA
            │
           4.7 kΩ
            │
           3.3V
```
Pull-up resistors (4.7 kΩ) on both SDA and SCL are **required** for I2C Standard Mode.
Some BMP280 breakout boards include onboard pull-ups — check before adding external ones.

### BMP280 I2C Address
| SDO pin connection | I2C address (7-bit) |
|--------------------|---------------------|
| SDO → GND | 0x76 |
| SDO → VCC | 0x77 |

Update `BMP280_I2C_ADDR` in `main_usercode.c` to match (`(0x76U << 1)` or `(0x77U << 1)`).

### External LED on PA8 (TIM1 CH1 PWM)
```
PA8 ──── 330 Ω ──── LED anode
                    LED cathode ──── GND
```
LED forward voltage ~2.0–2.2 V (red/green).
R = (3.3 V − 2.1 V) / 10 mA ≈ 120 Ω minimum; 330 Ω gives ~3.6 mA (dim but visible).

### UART1 (USB-TTL Adapter)
```
PA9 (TX) ──── USB-TTL RX
PA10 (RX) ─── USB-TTL TX  (optional)
GND      ──── USB-TTL GND
```
Terminal settings: **115200 baud, 8N1, no flow control**.
USB-TTL adapter must be set to **3.3 V logic**.

## Full Pin Table

| Blue Pill Pin | Connected To | Direction | Notes |
|---------------|--------------|-----------|-------|
| 3.3V | BMP280 VCC | Power | |
| GND | BMP280 GND, LED GND, USB-TTL GND | Power | Common ground |
| PB6 | BMP280 SCL | Output | 4.7 kΩ pull-up to 3.3 V |
| PB7 | BMP280 SDA | Bidirectional | 4.7 kΩ pull-up to 3.3 V |
| PA8 | LED via 330 Ω | Output | TIM1 CH1 PWM, 1 kHz |
| PA9 | USB-TTL RX | Output | UART1 TX |
| PA10 | USB-TTL TX | Input | UART1 RX (optional) |
| PC13 | — | Output | Onboard LED (active-low), blinks per loop |

## CubeMX Checklist
- [ ] I2C1 enabled, Standard Mode, 100 kHz, PB6/PB7
- [ ] USART1 enabled, 115200 baud, PA9/PA10
- [ ] TIM1 Channel 1, PWM Generation, PSC=71, ARR=999, PA8 AF Push-Pull
- [ ] PC13 GPIO_Output, Push-Pull

## TIM1 Advanced-Control Timer Note
TIM1 is an **advanced-control timer** and has a **Main Output Enable (MOE)** bit in the
Break and Dead-Time Register (BDTR). PWM output on PA8 will not appear unless MOE is set.
`HAL_TIM_PWM_Start()` sets MOE automatically; the additional
`__HAL_TIM_MOE_ENABLE(&htim1)` call in the init code is a safeguard.

## Power
- Power the Blue Pill from USB (Micro-USB connector).
- Total current draw: MCU (~30 mA) + BMP280 (<1 mA) + LED (~4 mA) ≈ <50 mA. USB can supply this.
