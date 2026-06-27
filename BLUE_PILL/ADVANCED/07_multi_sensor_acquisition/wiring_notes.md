# Wiring Notes — 07_multi_sensor_acquisition

## CRITICAL: 3.3V Logic
**ALL GPIO pins on the STM32F103C8T6 Blue Pill are 3.3V logic.**
Do NOT connect 5V signals directly to any STM32 pin without a level shifter.
Input signals above 3.3V will damage the MCU (unless the specific pin is 5V-tolerant and marked FT in the datasheet).

---

## ADC1 Channel 0 — PA0
| Signal | Blue Pill Pin | Notes                                        |
|--------|---------------|----------------------------------------------|
| AIN    | PA0           | Analog input; 0–3.3V range                  |
| GND    | GND           | Reference ground                             |

- PA0 is also TIM2 Ch1 — ensure CubeMX sets PA0 as analog input for ADC, not alternate function.
- For a test signal: wire a 10 kΩ potentiometer between 3.3V and GND with the wiper to PA0.
- Do NOT exceed 3.3V on this pin.

---

## I2C1 — BMP280 Temperature/Pressure Sensor
| Signal | Blue Pill Pin | BMP280 Module Pin |
|--------|---------------|-------------------|
| SCL    | PB6           | SCK / SCL         |
| SDA    | PB7           | SDI / SDA         |
| VCC    | 3.3V          | VCC / VDDIO       |
| GND    | GND           | GND               |
| CSB    | —             | Tie HIGH (to VCC) — selects I2C mode |
| SDO    | —             | GND → addr 0x76, VCC → addr 0x77    |

- External 4.7 kΩ pull-up resistors required on SDA and SCL to 3.3V unless your breakout board includes them.
- Most GY-BMP280 modules include 3.3V pull-ups on-board.
- I2C speed configured for 100 kHz (standard mode). Do not exceed 400 kHz without re-configuring CubeMX.

---

## UART1 — Debug Output
| Signal | Blue Pill Pin | USB-Serial Adapter |
|--------|---------------|--------------------|
| TX     | PA9           | RX of adapter      |
| RX     | PA10          | TX of adapter      |
| GND    | GND           | GND of adapter     |

- Use a 3.3V USB-to-UART adapter.
- Baud rate: 115200, 8N1, no flow control.
- Output arrives every ~100 ms: `ADC: min=... max=... avg=... | BMP280: XX.XX C`

---

## DMA Resource Map (for CubeMX reference)
| Peripheral     | DMA Controller | Channel | Direction | Mode     |
|----------------|----------------|---------|-----------|----------|
| ADC1           | DMA1           | Ch1     | P → M     | Circular |
| USART1 TX      | DMA1           | Ch4     | M → P     | Normal   |

- DMA1 Ch1 and Ch4 must not conflict with other peripherals in your CubeMX project.
- TIM3 uses no DMA — it fires a CPU interrupt at 1 kHz only.

---

## Power
| Source | Blue Pill Pin | Notes                                    |
|--------|---------------|------------------------------------------|
| 5V     | 5V            | From USB or bench supply                 |
| 3.3V   | 3V3           | Regulated output for sensors             |
| GND    | GND           | Common ground                            |

- Keep total 3.3V rail current under ~150 mA (onboard LDO limit).

---

## Quick-Test Checklist
- [ ] BMP280 VCC to 3.3V, GND to GND, SCL to PB6, SDA to PB7
- [ ] Pull-up resistors present on SCL and SDA (4.7 kΩ to 3.3V)
- [ ] Potentiometer wiper to PA0 for variable ADC input
- [ ] UART adapter connected (PA9→RX, PA10←TX, GND common)
- [ ] Serial terminal open at 115200 8N1
- [ ] Observe ADC report lines appear every ~100 ms
