# 09 — I2C Bus Scanner

## Overview

Scans I2C1 for all 7-bit device addresses (0x01–0x7F) using
`HAL_I2C_IsDeviceReady`. Found addresses are printed over UART1.

This is the classic embedded "who is on my I2C bus?" diagnostic — useful
before writing a driver to confirm the device is wired and powered correctly.

## Hardware

| Signal | Pin |
|--------|-----|
| I2C1 SCL | PB6 |
| I2C1 SDA | PB7 |
| USART1 TX | PA9 (→ USB-TTL adapter RX) |
| USART1 RX | PA10 (optional for this project) |
| Onboard LED | PC13 |

## CubeMX Configuration

### I2C1
- Mode: I2C
- Speed Mode: Standard Mode
- Clock Speed: 100000 Hz (100 kHz)
- Duty Cycle: 2 (standard)
- General Call / No-Stretch: disabled

### USART1
- Mode: Asynchronous, 115200 8N1
- NVIC: disabled (polled TX only)

### GPIO
- PB6: I2C1_SCL (Alternate Function Open-Drain) — CubeMX sets this
- PB7: I2C1_SDA (Alternate Function Open-Drain) — CubeMX sets this

### Clock
- 72 MHz system clock

## Build & Flash

1. Configure CubeMX as above and generate code.
2. Copy user code sections from `main_usercode.c` into generated `main.c`.
3. Connect I2C devices to PB6/PB7.
4. Connect USB-TTL adapter to PA9 (TX).
5. Build, flash, open terminal at 115200.

## Expected Output

```
=== I2C Bus Scan ===
Scanning 0x01 - 0x7F ...
Found device at 0x27
Found device at 0x3C
Scan complete. 2 device(s) found.
```

(Addresses will vary depending on connected devices.)

If no devices are found, the terminal prints:
```
Scan complete. 0 device(s) found.
```

Then the scan repeats every 5 seconds.

## Common I2C Device Addresses

| Address | Device |
|---------|--------|
| 0x27 or 0x3F | PCF8574 (LCD backpack) |
| 0x3C or 0x3D | SSD1306 OLED |
| 0x68 or 0x69 | MPU-6050 IMU |
| 0x76 or 0x77 | BMP280 / BME280 |
| 0x48–0x4B | ADS1115 ADC |

## Notes

- Code is **not tested on hardware**.
- I2C requires pull-up resistors on SCL and SDA — see `wiring_notes.md`.
- 3.3 V logic — see `wiring_notes.md`.
