# Wiring Notes — 11_ssd1306_oled

## Voltage Warning
> **ALL Blue Pill GPIO pins operate at 3.3 V logic.**
> The SSD1306 OLED module must have a 3.3 V-compatible I2C interface.
> Many cheap modules include an on-board 3.3 V regulator and level shifter,
> allowing 5 V VCC supply; always check your module's datasheet.
> Do NOT connect 5 V signals directly to PB6 or PB7.

---

## Pin Connections

### I2C1 Bus
| Blue Pill Pin | Signal | SSD1306 Pin | Notes |
|---------------|--------|-------------|-------|
| PB6 | I2C1 SCL | SCL | Open-drain; requires pull-up |
| PB7 | I2C1 SDA | SDA | Open-drain; requires pull-up |
| 3.3V | Power | VCC | If module accepts 3.3 V directly |
| GND | Ground | GND | Common ground |

### I2C Pull-up Resistors
I2C is open-drain — external pull-up resistors to 3.3 V are **mandatory**.

| Signal | Resistor | To |
|--------|----------|----|
| SDA (PB7) | 4.7 kΩ | 3.3 V |
| SCL (PB6) | 4.7 kΩ | 3.3 V |

Many SSD1306 breakout modules include 4.7 kΩ or 10 kΩ pull-ups on the PCB.
If your module already has them, additional external resistors are not needed —
check the schematic of your specific module.

### Onboard LED (PC13)
| Blue Pill Pin | Function | Notes |
|---------------|----------|-------|
| PC13 | Onboard LED (active-low) | GPIO_PIN_RESET = LED ON |

---

## I2C Address
Default address: **0x3C** (SA0 / ADDR pin tied to GND).
If your module has SA0 pulled HIGH, the address is **0x3D** — update
`SSD1306_I2C_ADDR` in `main_usercode.c` accordingly (shift by 1 for HAL):
```c
#define SSD1306_I2C_ADDR  (0x3D << 1)
```

---

## CubeMX I2C1 Settings
- Mode: I2C (Fast Mode)
- Speed: 400 000 Hz
- Rise Time: 300 ns (default)
- Fall Time: 300 ns (default)
- Duty Cycle: Tlow/Thigh = 2 (default)
- GPIO: PB6 and PB7 → Alternate Function Open Drain, No internal pull-up
  (rely on external resistors)

---

## Common Issues

| Symptom | Likely Cause | Fix |
|---------|-------------|-----|
| No display output | Wrong I2C address | Scan bus with I2C address scanner |
| Garbled display | SDA/SCL swapped | Check connections |
| I2C bus stuck LOW | Missing pull-up resistors | Add 4.7 kΩ to 3.3 V |
| HAL_ERROR on init | Module not powered | Verify VCC / GND |
| Display on but dim | Contrast byte too low | Increase 0x81 value in init sequence |

---

## PC13 / BKP Quirk
If the onboard LED (PC13) glows unexpectedly with RTC enabled, the RTC may be
driving a calibration signal on PC13 via the Backup domain (ASOE bit).
Clear it with:
```c
HAL_PWR_EnableBkUpAccess();
BKP->RTCCR &= ~BKP_RTCCR_ASOE;
HAL_PWR_DisableBkUpAccess();
```
This project does not use the RTC, so this is unlikely to be an issue unless
another project has written to BKP registers previously.
