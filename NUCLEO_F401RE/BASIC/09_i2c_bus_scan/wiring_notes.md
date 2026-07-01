# Wiring Notes — Nucleo-F401RE BASIC/09: I2C Bus Scan

## ⚠️ 3.3V Logic Warning
All Nucleo GPIO pins are 3.3V. Power I2C devices from the 3.3V pin on the Nucleo headers.

## I2C Connections
| Signal | Nucleo Pin | Arduino Header | Notes |
|--------|-----------|----------------|-------|
| SCL | PB8 | D15 | 4.7 kΩ pull-up to 3.3V |
| SDA | PB9 | D14 | 4.7 kΩ pull-up to 3.3V |

## Example: Adding a BMP280
| BMP280 | Nucleo |
|--------|--------|
| VCC | 3.3V (CN7 or CN10) |
| GND | GND |
| SCL | PB8 (D15) |
| SDA | PB9 (D14) |
| SDO | GND → address 0x76 |
| CSB | 3.3V → I2C mode |

## Serial Output
No extra wiring for serial. ST-Link VCP: plug USB into CN1, open terminal at 115200 baud.

## Notes
- Many breakout boards (BMP280, SSD1306, MPU6050) include onboard 4.7 kΩ pull-ups — no external resistors needed
- If no devices respond, check pull-ups and 3.3V power
- The scan sends a START + address + STOP to each address; devices ACK if present
