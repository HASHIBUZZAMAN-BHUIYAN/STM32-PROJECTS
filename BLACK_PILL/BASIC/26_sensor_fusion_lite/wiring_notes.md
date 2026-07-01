# Wiring Notes — Black Pill BASIC/26: Sensor Fusion

## Pin Assignment
| Black Pill Pin | Signal | BMP280 | MPU6050 |
|---------------|--------|--------|---------|
| PB6 | I2C1_SCL | SCL | SCL |
| PB7 | I2C1_SDA | SDA | SDA |
| 3.3V | VCC | VCC | VCC |
| GND | GND | GND | GND + AD0 |

## I2C Addresses
- BMP280: SDO→GND = **0x76**
- MPU6050: AD0→GND = **0x68**

All on same I2C1 bus.

## MPU6050 AD0 Pin
AD0 sets the lower bit of the I2C address:
- AD0 = GND → 0x68 (most common)
- AD0 = VCC → 0x69 (second device on same bus)

## FPU Note
The Black Pill F411 has hardware FPU (Cortex-M4F). Make sure compile flags include:
`-mfpu=fpv4-sp-d16 -mfloat-abi=hard`

CubeMX sets these for F4 targets automatically. This makes atan2f() run ~20× faster than on Blue Pill (M3 softfloat).

## Parts List
| Component | Qty |
|-----------|-----|
| Black Pill (STM32F411CEU6) | 1 |
| BMP280 I2C module | 1 |
| MPU6050 GY-521 module | 1 |
| USB-UART adapter | 1 |
| USB-C cable | 1 |
| Breadboard + jumpers | 1 set |

## 3.3V Safety
Both BMP280 and MPU6050 are 3.3V logic. The GY-521 MPU6050 module has an onboard 3.3V LDO — connect VCC to 3.3V (NOT 5V directly to the chip).
