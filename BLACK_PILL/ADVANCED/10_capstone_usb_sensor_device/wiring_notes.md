# Wiring Notes — Black Pill ADVANCED/10: USB Sensor Device Capstone

## USB Connection
The Black Pill's USB-C connector connects DIRECTLY to PA11 (D-) and PA12 (D+).
One USB-C cable from PC to Black Pill is the only required connection.
This is the key advantage over Nucleo, which needs external USB hardware for USB CDC.

## CubeMX USB Clock Setup (critical)
HSE = 25 MHz external crystal (on-board):
| PLL param | Value | Result |
|-----------|-------|--------|
| PLLM | 25 | VCO input = 1 MHz |
| PLLN | 192 | VCO output = 192 MHz |
| PLLP | /4 | SYSCLK = 48 MHz × 2 = 96 MHz... |

Actually for 100 MHz SYSCLK + 48 MHz USB:
| PLL param | Value | Result |
|-----------|-------|--------|
| PLLM | 25 | 1 MHz |
| PLLN | 200 | 200 MHz VCO |
| PLLP | /2 | 100 MHz SYSCLK |
| PLLQ | 4 | 50 MHz... not exact |

USB requires EXACTLY 48 MHz. Use PLLM=25,PLLN=192,PLLP=4,PLLQ=4:
- SYSCLK = 192/4 = 48 MHz (reduced to avoid USB error)
- Or: add separate USB PLL config in CubeMX → RCC → PLLQ

Safest: let CubeMX auto-configure with USB enabled — it picks valid values.

## I2C1 Shared Bus (BMP280 + MPU6050 + SSD1306)
| Signal | Black Pill | Note |
|--------|-----------|------|
| SCL | PB6 | NOT PB8 — Black Pill specific |
| SDA | PB7 | NOT PB9 — Black Pill specific |

## BMP280
| Pin | Connection |
|-----|-----------|
| VCC | 3.3V |
| GND | GND |
| SCL | PB6 |
| SDA | PB7 |
| SDO | GND (0x76) |

## MPU6050
| Pin | Connection |
|-----|-----------|
| VCC | 3.3V |
| GND | GND |
| SCL | PB6 |
| SDA | PB7 |
| AD0 | GND (0x68) |

## SSD1306 OLED
| Pin | Connection |
|-----|-----------|
| VCC | 3.3V |
| GND | GND |
| SCL | PB6 |
| SDA | PB7 |

## W25Q32 SPI Flash (SPI1 — free on Black Pill)
| Pin | Black Pill |
|-----|-----------|
| VCC | 3.3V |
| GND | GND |
| CLK | PA5 |
| MOSI | PA7 |
| MISO | PA6 |
| /CS | PA4 |
| /WP, /HOLD | 3.3V |

## usbd_cdc_if.c Modification
In `CDC_Receive_FS()`:
```c
// Add at the end of the function:
extern void App_CDC_Receive(const uint8_t *Buf, uint32_t Len);
App_CDC_Receive(Buf, *Len);
```

## Parts List
| Component | Qty |
|-----------|-----|
| Black Pill (STM32F411CEU6) | 1 |
| BMP280 module | 1 |
| MPU6050 module | 1 |
| SSD1306 OLED (I2C, 128×64) | 1 |
| W25Q32 SPI flash module | 1 |
| USB-C cable | 1 |
| 4.7kΩ resistors (I2C pullups) | 2 |
| Breadboard + jumper wires | - |
