# Wiring Notes — 03_freertos_mutex

## Voltage Warning

**ALL GPIO pins on the Blue Pill are 3.3 V logic.**
I2C lines (PB6, PB7) are open-drain with pull-up resistors to 3.3 V.
Do NOT connect 5 V I2C devices without a level shifter.
Many SSD1306 and BMP280 breakout boards include onboard 3.3 V regulators and
logic-level conversion — verify your specific module before connecting.

## I2C1 Bus

| Pin | Signal | Notes                                      |
|-----|--------|--------------------------------------------|
| PB6 | SCL    | Open-drain; requires pull-up to 3.3 V      |
| PB7 | SDA    | Open-drain; requires pull-up to 3.3 V      |

### Pull-up Resistors

If your breakout boards do not include pull-ups, add externally:

```
3.3V ──┬─── 4.7 kΩ ─── PB6 (SCL)
        └─── 4.7 kΩ ─── PB7 (SDA)
```

At 100 kHz standard mode, 4.7 kΩ pull-ups work reliably.

## BMP280 (Temperature Sensor)

| BMP280 Pin | Connect To       | Notes                            |
|------------|------------------|----------------------------------|
| VCC / VDD  | 3.3 V            |                                  |
| GND        | GND              |                                  |
| SCL / SCK  | PB6              |                                  |
| SDA / SDI  | PB7              |                                  |
| SDO        | GND              | Sets I2C address to 0x76         |
| CSB        | 3.3 V            | Selects I2C mode (not SPI)       |

If SDO is tied to 3.3 V, the I2C address becomes 0x77 — update `BMP280_ADDR`
in main_usercode.c accordingly.

## SSD1306 OLED (128x64, I2C)

| SSD1306 Pin | Connect To | Notes                              |
|-------------|------------|------------------------------------|
| VCC         | 3.3 V      | Some modules tolerate 5 V only — check |
| GND         | GND        |                                    |
| SCL         | PB6        |                                    |
| SDA         | PB7        |                                    |

Default I2C address is 0x3C (SA0/D/C# pin to GND). If SA0 is HIGH, address is
0x3D — update `SSD1306_ADDR` in main_usercode.c.

## Both Devices on the Same Bus

```
PB6 (SCL) ──────┬────────── BMP280 SCL
                 └────────── SSD1306 SCL

PB7 (SDA) ──────┬────────── BMP280 SDA
                 └────────── SSD1306 SDA

3.3V ─── 4.7 kΩ ─── PB6   (if no onboard pull-ups)
3.3V ─── 4.7 kΩ ─── PB7
```

## UART Debug

```
Blue Pill PA9 (TX) ──► USB-TTL RX
Blue Pill GND      ──► USB-TTL GND
```

115200 baud, 8N1.

## Mutex — What to Observe

To see the bug WITHOUT a mutex, comment out the `osMutexAcquire` / `osMutexRelease`
calls. Watch UART output for garbled lines or I2C errors. Re-enable the mutex to
restore correct behavior.
