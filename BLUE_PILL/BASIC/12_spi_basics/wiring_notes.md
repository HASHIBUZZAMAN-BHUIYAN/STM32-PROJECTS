# Wiring Notes — 12_spi_basics

## Voltage Warning
> **ALL Blue Pill GPIO pins operate at 3.3 V logic.**
> SPI signals (SCK, MOSI, MISO, CS) are 3.3 V.
> Connecting a 5 V SPI device directly to these pins WILL damage the MCU.
> Use a level-shifter (e.g., TXS0104E) for any 5 V SPI peripheral.

---

## SPI1 Pin Connections

### Loopback Test (self-test — no external device)
| Blue Pill Pin | Signal | Connection |
|---------------|--------|------------|
| PA5 | SPI1 SCK | (no external connection needed) |
| PA6 | SPI1 MISO | Jumper wire → PA7 (MOSI) |
| PA7 | SPI1 MOSI | Jumper wire → PA6 (MISO) |
| PA4 | CS (GPIO out) | (not needed for loopback, but asserted by code) |

Install a single jumper wire between PA6 and PA7 before running the loopback
test.  Remove it before connecting a real SPI device.

### Generic 3.3 V SPI Device
| Blue Pill Pin | Signal | Device Pin | Notes |
|---------------|--------|------------|-------|
| PA5 | SCK | SCLK / CLK | Clock |
| PA6 | MISO | SDO / DOUT | Master-In Slave-Out |
| PA7 | MOSI | SDI / DIN | Master-Out Slave-In |
| PA4 | CS | CSN / SS | Active-low chip select |
| 3.3V | Power | VCC | If device runs at 3.3 V |
| GND | Ground | GND | Common ground |

### Onboard LED (PC13)
| Blue Pill Pin | Function | Notes |
|---------------|----------|-------|
| PC13 | Onboard LED (active-low) | Slow blink = PASS, fast blink = FAIL |

### UART1 (optional — for pass/fail output)
| Blue Pill Pin | Signal | USB-UART Adapter |
|---------------|--------|-----------------|
| PA9 | TX | RX |
| GND | GND | GND |

---

## SPI Mode / Polarity
Code default: **Mode 0** (CPOL = 0, CPHA = 0 — clock idle LOW, sample on rising edge).
Change `SPI_CPOL_LOW` / `SPI_PHASE_1EDGE` in CubeMX to match your device.

Common modes:
| Mode | CPOL | CPHA | Idle Clock |
|------|------|------|-----------|
| 0 | 0 | 0 | LOW |
| 1 | 0 | 1 | LOW |
| 2 | 1 | 0 | HIGH |
| 3 | 1 | 1 | HIGH |

---

## CS (NSS) Note
PA4 is driven as a plain **GPIO output** (software NSS).
The SPI1 hardware NSS signal (also on PA4) is **not** used — NSS is set to
"Software" in CubeMX.  This avoids the NSS pulse mode quirks of the STM32
hardware.

De-assert CS (PA4 HIGH) is the idle/default state.
Assert CS (PA4 LOW) before each transaction; de-assert after.

---

## Common Issues

| Symptom | Likely Cause | Fix |
|---------|-------------|-----|
| RX buffer all 0xFF | MISO not connected or no loopback jumper | Check wiring |
| RX buffer all 0x00 | MOSI stuck LOW | Check PA7 AF config in CubeMX |
| Loopback FAIL every byte | Incorrect SPI mode | Match CPOL/CPHA to device requirements |
| Device not responding | CS not asserted, or wrong polarity | Verify PA4 goes LOW during transfer |
| Data shifted by 1 bit | CPHA mismatch | Try switching CPHA in CubeMX |

---

## PC13 / BKP Quirk
The onboard LED on PC13 is active-low.
This project does not use RTC, so PC13 should behave normally.
If the LED stays on unexpectedly, see project 15 for the BKP/ASOE workaround.
