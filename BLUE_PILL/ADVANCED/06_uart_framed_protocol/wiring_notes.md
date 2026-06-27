# Wiring Notes — 06_uart_framed_protocol

## CRITICAL: 3.3V Logic
**ALL GPIO pins on the STM32F103C8T6 Blue Pill are 3.3V logic.**
Do NOT connect 5V signals directly to any STM32 pin without a level shifter.
Some pins are 5V-tolerant (marked FT in the datasheet) but output is always 3.3V.

---

## UART1 — Host Communication
| Signal | Blue Pill Pin | Host / USB-Serial Adapter |
|--------|---------------|--------------------------|
| TX     | PA9           | RX of adapter            |
| RX     | PA10          | TX of adapter            |
| GND    | GND           | GND of adapter           |

- Use a 3.3V USB-to-UART adapter (e.g., CP2102, CH340 in 3.3V mode).
- Do NOT connect a 5V TTL adapter directly — use a voltage divider or level shifter on the adapter TX → PA10 line if adapter is 5V TTL.
- Baud rate: 115200, 8N1.

---

## ADC1 Channel 0 — PA0
| Signal | Blue Pill Pin | Notes                                   |
|--------|---------------|-----------------------------------------|
| AIN    | PA0           | Analog input 0–3.3V                     |
| GND    | GND           | Reference ground                        |

- Do NOT apply signals above 3.3V to PA0 when used as ADC input.
- For testing: connect a 10 kΩ potentiometer between 3.3V and GND, wiper to PA0.

---

## I2C1 — BMP280 Sensor
| Signal | Blue Pill Pin | BMP280 Pin |
|--------|---------------|------------|
| SCL    | PB6           | SCK / SCL  |
| SDA    | PB7           | SDI / SDA  |
| VCC    | 3.3V          | VCC / VDDIO|
| GND    | GND           | GND        |
| CSB    | —             | Tie to VCC (selects I2C mode) |
| SDO    | —             | Tie to GND → I2C address 0x76 |
|        |               | Tie to VCC → I2C address 0x77 |

- Add 4.7 kΩ pull-up resistors from SDA and SCL to 3.3V if the BMP280 module does not include them.
- Most breakout boards (e.g., GY-BMP280) already include pull-ups and decoupling caps.
- I2C speed: 100 kHz (standard mode).

---

## Onboard LED — PC13
- Active-LOW: driving PC13 LOW turns the LED ON.
- No external wiring needed.
- The LED is controlled by CMD 0x02 (SET_LED).

---

## Power
| Source  | Blue Pill Pin | Notes                           |
|---------|---------------|---------------------------------|
| 5V USB  | 5V            | Input from USB or regulator     |
| 3.3V    | 3V3           | Regulated 3.3V output (~150 mA) |
| GND     | GND           | Common ground                   |

- The Blue Pill onboard 3.3V regulator (RT9193) is rated for ~300 mA but runs warm; keep peripheral current below 100 mA from this rail.

---

## Quick-Test Checklist
- [ ] USB-serial adapter GND connected to Blue Pill GND
- [ ] Adapter TX → PA10, Adapter RX ← PA9 (3.3V levels)
- [ ] BMP280 powered from 3.3V rail
- [ ] BMP280 SCL → PB6, SDA → PB7 with pull-ups present
- [ ] Potentiometer wiper to PA0 for ADC test
- [ ] Serial terminal: 115200 8N1
- [ ] Send test frame from Python/pyserial: `0xAA 0x01 0x01 <crc>` for GET_ADC
