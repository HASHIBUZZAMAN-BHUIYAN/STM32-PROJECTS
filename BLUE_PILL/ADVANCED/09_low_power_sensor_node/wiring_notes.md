# Wiring Notes — Blue Pill ADVANCED/09: Low-Power Sensor Node

## ⚠️ 3.3V Logic Warning
All Blue Pill GPIO pins are 3.3V. Never connect 5V signals to any pin. Power the BMP280 from the 3.3V pin.

## Pin Connections

### BMP280 (I2C)
| BMP280 Pin | Blue Pill Pin | Notes |
|-----------|--------------|-------|
| VCC | 3.3V | 3.3V only |
| GND | GND | |
| SCL | PB6 (I2C1 SCL) | 4.7kΩ pull-up to 3.3V |
| SDA | PB7 (I2C1 SDA) | 4.7kΩ pull-up to 3.3V |
| SDO | GND | Sets I2C address to 0x76 |
| CSB | 3.3V | Forces I2C mode |

### UART (debug output)
| Signal | Blue Pill Pin | PC (USB-serial adapter) |
|--------|--------------|------------------------|
| TX | PA9 | RX of adapter |
| RX | PA10 | TX of adapter |
| GND | GND | GND of adapter |

Use a 3.3V-level USB-serial adapter (CP2102, CH340G set to 3.3V). Do NOT use a 5V FTDI without level shifting.

### Power Supply
For low-power testing, power from 3.3V directly (bypass the on-board 5V USB regulator):
- Cut the USB 5V supply to the board
- Power the 3.3V pin directly from a regulated 3.3V source
- Place multimeter in series to measure average current

## Parts List
| Component | Qty | Notes |
|-----------|-----|-------|
| Blue Pill (STM32F103C8T6) | 1 | |
| BMP280 breakout module | 1 | 3.3V compatible |
| 4.7kΩ resistors | 2 | I2C pull-ups |
| USB-serial adapter (3.3V) | 1 | For UART output |
| External ST-Link V2 | 1 | For flashing |
| Multimeter | 1 | For current measurement |

## Power Budget Estimate
| Mode | Current | Duration |
|------|---------|---------|
| Active (BMP280 read + UART TX) | ~30mA | ~50ms per wake |
| Standby | ~5µA | ~29.95s between wakes |
| Average (30s cycle) | ~0.10mA | |

## How to Flash
Use external ST-Link V2: SWDIO→PA13, SWCLK→PA14, GND→GND, 3.3V→3.3V.
BOOT0 jumper stays at 0.

## Expected Behavior
On each wake (every 30 seconds):
1. MCU resets from Standby (RTC alarm woke it)
2. Reads BMP280 temperature
3. Transmits "Wake#N | Temp=XX.XX C" via UART1 at 115200
4. Sets next RTC alarm
5. Enters Standby mode again

LED (PC13) does not blink — no unnecessary GPIO activity to keep power low.
