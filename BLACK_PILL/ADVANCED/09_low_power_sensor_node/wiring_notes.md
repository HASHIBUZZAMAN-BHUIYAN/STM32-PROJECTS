# Wiring Notes — Black Pill ADVANCED/09: Low-Power Sensor Node

## Power Measurement
Black Pill has no IDD jumper. Use a USB power meter or bench PSU method:
- Disconnect USB
- Power from 3.3V bench PSU with 10Ω shunt inline
- Measure shunt voltage → mA = mV/10

## BMP280 (I2C1)
| BMP280 | Black Pill |
|--------|-----------|
| VCC | 3.3V |
| GND | GND |
| SCL | PB6 |
| SDA | PB7 |
| SDO | GND (addr 0x76) |

## W25Q32 Flash (SPI1 — free on Black Pill)
| W25Q32 | Black Pill |
|--------|-----------|
| VCC | 3.3V |
| GND | GND |
| CLK | PA5 (SPI1_SCK) |
| MOSI | PA7 (SPI1_MOSI) |
| MISO | PA6 (SPI1_MISO) |
| /CS | PA4 (GPIO Output) |
| /WP | 3.3V (write protect off) |
| /HOLD | 3.3V (hold off) |

## USART1 (for debug/wakeup confirmation)
| Black Pill | USB-UART |
|-----------|---------|
| PA9 (TX) | RX |
| GND | GND |

## RTC Crystal (Optional but Recommended)
For accurate 60s intervals, solder a 32.768 kHz crystal between PC14 and PC15
with 12pF load capacitors to GND. Without it, the LSI (~32kHz±5%) will drift.

## Battery Options
| Option | Life (estimated) |
|--------|----------------|
| CR2032 (200mAh) | ~2000 hours |
| AA × 2 (3000mAh) | ~30,000 hours |
| LiPo 500mAh | ~5000 hours |

Use a 3.3V LDO regulator between battery and 3.3V pin if using >3.3V battery.

## Parts List
| Component | Qty |
|-----------|-----|
| Black Pill (STM32F411CEU6) | 1 |
| BMP280 module | 1 |
| W25Q32 SPI NOR flash (or module) | 1 |
| USB-UART adapter | 1 |
| CR2032 + holder (for VBAT) | 1 |
| 32.768 kHz crystal | 1 |
| 12pF capacitors | 2 |
| 10Ω shunt resistor (for measurement) | 1 |
| Jumper wires | 15 |
