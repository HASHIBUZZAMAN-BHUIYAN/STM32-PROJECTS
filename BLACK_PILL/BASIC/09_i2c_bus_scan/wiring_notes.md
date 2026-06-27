# Wiring Notes — Project 09: I2C Bus Scanner

## Voltage Warning
**ALL GPIO pins on the STM32F411 Black Pill are 3.3 V logic.**
Never connect 5 V signals directly to any GPIO pin — this will damage the MCU.
If your I2C device runs at 5 V, use a bidirectional level shifter (e.g., TXS0102).

## No Onboard ST-Link
The Black Pill has no onboard ST-Link. You need either:
- USB DFU mode (BOOT0 + NRST dance — see README)
- An external ST-Link V2 dongle connected to PA13/PA14

## I2C1 Wiring

```
Black Pill          I2C Device(s)
----------          -------------
PB6 (SCL) ----+---- SCL
              |
             4.7kΩ
              |
             3.3V

PB7 (SDA) ----+---- SDA
              |
             4.7kΩ
              |
             3.3V

3V3 -------------- VCC  (for 3.3V devices only)
GND -------------- GND
```

### Pull-up Resistor Notes
- 4.7 kΩ pull-ups to 3.3 V are required on both SCL and SDA
- Many breakout boards (SSD1306, MPU-6050, BMP280) have onboard pull-ups — check your module
- If you stack multiple I2C devices, their pull-ups combine in parallel; use 10 kΩ per device to keep total resistance above 1 kΩ
- At 100 kHz Standard Mode, 4.7 kΩ is the safe standard value
- Do NOT use 5V as the pull-up rail; all pull-ups must go to 3.3 V

## UART1 Wiring (for serial monitor)

```
Black Pill          USB-UART Adapter
----------          ----------------
PA9  (TX) --------- RX
PA10 (RX) --------- TX
GND       --------- GND
```

- PA10 quirk: if UART RX does not work reliably, add a 10 kΩ pull-up from PA10 to 3.3 V
- Adapter must be set to 3.3 V logic level (not 5 V)
- Baud rate: 115200

## Example Devices You Can Scan

| Device         | Typical Address | Notes                               |
|----------------|-----------------|-------------------------------------|
| PCF8574 (LCD)  | 0x27 or 0x3F    | Depends on A0/A1/A2 jumpers         |
| SSD1306 OLED   | 0x3C or 0x3D    | Depends on SA0 pin                  |
| MPU-6050/6500  | 0x68 or 0x69    | Depends on AD0 pin                  |
| BMP280         | 0x76 or 0x77    | Depends on SDO pin                  |
| AHT10/AHT20   | 0x38            | Fixed address                       |
| DS3231 RTC     | 0x57 + 0x68     | Two addresses (EEPROM + clock)      |
| VL53L0X ToF    | 0x29            | Fixed address                       |

## Schematic Diagram (ASCII)

```
            +3.3V
              |
         4.7kΩ  4.7kΩ
              |       |
Black Pill  [PB6]--SCL---[I2C Device 1]---[I2C Device 2]
            [PB7]--SDA---[I2C Device 1]---[I2C Device 2]
            [GND]--GND---[I2C Device 1]---[I2C Device 2]
            [3V3]--VCC---[I2C Device 1]---[I2C Device 2]
```

## Checklist Before Powering On
- [ ] Pull-up resistors present on SCL and SDA to 3.3 V
- [ ] All device VCC pins connected to 3.3 V (not 5 V)
- [ ] Common GND between Black Pill and all I2C devices
- [ ] USB-UART adapter logic level set to 3.3 V
- [ ] Serial terminal open at 115200 8N1 before resetting board
- [ ] No bare wire shorts between SCL and SDA
