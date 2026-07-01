# Wiring Notes — Nucleo-F401RE BASIC/26: Sensor Fusion

## ⚠️ 3.3V Logic Warning
Both BMP280 and MPU6050 accept 3.3V power and signals.

## I2C Bus (shared by both sensors)
| Signal | Nucleo | Notes |
|--------|--------|-------|
| SCL | PB8 (D15) | 4.7 kΩ pull-up to 3.3V |
| SDA | PB9 (D14) | 4.7 kΩ pull-up to 3.3V |

## BMP280
| BMP280 | Nucleo |
|--------|--------|
| VCC | 3.3V |
| GND | GND |
| SCL | PB8 |
| SDA | PB9 |
| SDO | GND → I2C addr 0x76 |
| CSB | 3.3V → I2C mode |

## MPU6050
| MPU6050 | Nucleo |
|---------|--------|
| VCC | 3.3V |
| GND | GND |
| SCL | PB8 |
| SDA | PB9 |
| AD0 | GND → I2C addr 0x68 |
| INT | NC (not used) |

## Address Conflict
If both sensors happened to share the same address, you'd need two separate I2C buses or a TCA9548A multiplexer. BMP280 at 0x76 and MPU6050 at 0x68 never conflict.

## Parts List
| Component | Qty |
|-----------|-----|
| Nucleo-F401RE | 1 |
| BMP280 breakout | 1 |
| MPU6050 breakout | 1 |
| 4.7 kΩ resistors | 2 |
| Breadboard + jumpers | 1 set |
