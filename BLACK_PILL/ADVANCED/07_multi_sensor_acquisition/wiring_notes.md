# Wiring Notes — Black Pill ADVANCED/07: Multi-Sensor Acquisition

## I2C1 Shared Bus (BMP280 + MPU6050)
| Signal | Black Pill | Note |
|--------|-----------|------|
| SCL | PB6 | I2C1_SCL (Black Pill specific — NOT PB8) |
| SDA | PB7 | I2C1_SDA (Black Pill specific — NOT PB9) |

## BMP280 Wiring
| BMP280 | Black Pill |
|--------|-----------|
| VCC | 3.3V |
| GND | GND |
| SCL | PB6 |
| SDA | PB7 |
| SDO | GND (addr 0x76) |

## MPU6050 Wiring
| MPU6050 | Black Pill |
|---------|-----------|
| VCC | 3.3V |
| GND | GND |
| SCL | PB6 (shared) |
| SDA | PB7 (shared) |
| AD0 | GND (addr 0x68) |
| INT | NC |

## DHT11 Wiring
| DHT11 | Black Pill |
|-------|-----------|
| VCC | 3.3V |
| GND | GND |
| DATA | PA8 |

## ADC Potentiometer
| Part | Black Pill |
|------|-----------|
| Pot wiper | PA1 (ADC1_IN1) |
| Pot +end | 3.3V |
| Pot -end | GND |

## USART1
| Black Pill | USB-UART |
|-----------|---------|
| PA9 (TX) | RX |
| GND | GND |

## 3.3V Caution
All modules must be 3.3V compatible. DHT11 can run on 3.3V but output high
is typically 3.0V — this is fine for 3.3V MCU inputs.

## Parts List
| Component | Qty |
|-----------|-----|
| Black Pill (STM32F411CEU6) | 1 |
| BMP280 module | 1 |
| MPU6050 module | 1 |
| DHT11 sensor | 1 |
| Potentiometer (10kΩ) | 1 |
| USB-UART adapter | 1 |
| 4.7kΩ resistors (I2C pull-up) | 2 |
| Jumper wires | 20 |
