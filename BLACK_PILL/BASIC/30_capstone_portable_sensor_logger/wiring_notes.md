# Wiring Notes — Black Pill BASIC/30: Capstone Portable Sensor Logger

## Full Pin Table
| Black Pill Pin | Signal | Component |
|---------------|--------|----------|
| PB6 | I2C1_SCL | BMP280 SCL + SSD1306 SCL |
| PB7 | I2C1_SDA | BMP280 SDA + SSD1306 SDA |
| PA5 | SPI1_SCK | W25Q32 CLK |
| PA6 | SPI1_MISO | W25Q32 DO |
| PA7 | SPI1_MOSI | W25Q32 DI |
| PA4 | SPI1_CS | W25Q32 CS |
| PA8 | GPIO (DHT11) | DHT11 DATA |
| PA11 | USB_OTG_DM | USB-C D- (internal) |
| PA12 | USB_OTG_DP | USB-C D+ (internal) |
| PC13 | LED (active-LOW) | Heartbeat |
| 3.3V | VCC | BMP280, SSD1306, W25Q32 |
| GND | GND | All components |

## Wiring Diagram
```
Black Pill PB6 ──┬── BMP280 SCL
                 └── SSD1306 SCL

Black Pill PB7 ──┬── BMP280 SDA
                 └── SSD1306 SDA

Black Pill PA5 ── W25Q32 CLK
Black Pill PA6 ── W25Q32 DO
Black Pill PA7 ── W25Q32 DI
Black Pill PA4 ── W25Q32 CS
Black Pill 3.3V ─┬── W25Q32 VCC + HOLD + WP (tie HOLD,WP to 3.3V)
                 ├── BMP280 VCC
                 └── SSD1306 VCC

Black Pill PA8 ── DHT11 DATA ─── 10kΩ ─── 3.3V
Black Pill GND ─┬── W25Q32 GND
                ├── BMP280 GND
                ├── SSD1306 GND
                └── DHT11 GND
```

## USB CDC (Native)
Just plug USB-C cable into the Black Pill. The STM32 USB_OTG_FS is directly connected to PA11/PA12 → USB-C connector on WeAct boards.

## SPI1 on Black Pill — No LED Conflict
PA5 (SPI1_SCK) is free on Black Pill (LED is PC13). This is a major advantage over Nucleo where SPI1 conflicts with the LED on PA5.

## W25Q32 HOLD and WP
Tie both HOLD and WP pins to 3.3V to enable normal operation.

## Parts List
| Component | Qty | Notes |
|-----------|-----|-------|
| Black Pill (STM32F411CEU6) | 1 | |
| BMP280 I2C module | 1 | |
| SSD1306 OLED 128×64 | 1 | |
| DHT11 sensor | 1 | |
| W25Q32 SPI flash module | 1 | 32Mbit = 4MB |
| 10kΩ resistor | 1 | DHT11 pull-up |
| USB-C cable | 1 | |
| Breadboard + jumpers | 1 set | |

## 3.3V Safety
All components are 3.3V logic. Never connect 5V signals to Black Pill GPIO pins.
