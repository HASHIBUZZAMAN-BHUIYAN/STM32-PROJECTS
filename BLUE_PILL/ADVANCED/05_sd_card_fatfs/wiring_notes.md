# Wiring Notes — 05_sd_card_fatfs

## Voltage Warning

**ALL GPIO pins on the Blue Pill are 3.3 V logic.**
The STM32F103C8T6 SPI pins (PB12–PB15) are 3.3 V. SD cards also operate at
3.3 V. Most micro-SD breakout boards sold online include a 3.3 V LDO regulator
and level shifters for use with 5 V Arduinos. On the Blue Pill, connect the
breakout's 3.3 V supply pin directly (or 5 V if the board has its own LDO) and
wire SPI directly at 3.3 V logic — do NOT add extra level shifters on the data
lines if the breakout already has them (double-shifting will break signals).

Verify your specific breakout board's schematic before wiring.

## SPI2 Pinout

| Blue Pill Pin | Signal  | SD Breakout Pin | Notes                         |
|---------------|---------|-----------------|-------------------------------|
| PB13          | SCK     | CLK / SCK       | SPI clock                     |
| PB14          | MISO    | DO / MISO       | SD card data out              |
| PB15          | MOSI    | DI / MOSI       | SD card data in               |
| PB12          | CS      | CS / SS         | Active-low chip select        |
| 3.3V          | VCC     | 3.3V or VCC     | Check breakout requirements   |
| GND           | GND     | GND             |                               |

## Wiring Diagram

```
Blue Pill                   Micro-SD Breakout
---------                   -----------------
PB13 (SCK)  ──────────────► CLK
PB14 (MISO) ◄────────────── DO
PB15 (MOSI) ──────────────► DI
PB12 (CS)   ──────────────► CS          (active-low)
3.3V        ──────────────► 3.3V / VCC
GND         ──────────────── GND
```

## ADC Input (PA0)

| Pin | Function   | Notes                                          |
|-----|------------|------------------------------------------------|
| PA0 | ADC1 Ch0   | 0–3.3 V analog input. Connect to pot or sensor |

Simple potentiometer test:

```
3.3V ─── pot pin 1
          pot wiper ─── PA0
          pot pin 3 ─── GND
```

## UART Debug Output (Optional)

```
Blue Pill PA9 (TX) ──► USB-TTL RX
Blue Pill GND      ──► USB-TTL GND
```

115200 baud, 8N1. Shows "SD mount OK" and each log line as it is written.

## SD Card Preparation

1. Format the SD card as FAT32 (Windows: right-click → Format → FAT32).
   Cards > 32 GB formatted as exFAT will not work with default FatFs settings.
2. Recommended: 2 GB or 4 GB micro-SD for simplicity.
3. The code opens `LOG.TXT` in append mode — previous data is preserved on
   every power cycle.

## SPI Speed

SD cards require SPI clock <= 400 kHz during initialisation (CMD0 / CMD8 phase).
After the card responds, higher speeds are acceptable (typically up to 25 MHz
for standard SD, 50 MHz for SDHC in high-speed mode).

The CubeMX FatFs SPI driver template switches speed automatically. If writing
a custom driver, set prescaler to /256 (281 kHz at 72 MHz) during init, then
switch to /4 (18 MHz) for data transfers.

## PC13 Onboard LED (Error Indicator)

The error handler blinks PC13 rapidly on any FatFs failure.
PC13 is active-low: GPIO_PIN_RESET = LED ON, GPIO_PIN_SET = LED OFF.

## Common Issues

| Symptom                  | Likely Cause                                 |
|--------------------------|----------------------------------------------|
| f_mount returns FR_NODISK| CS not wired, or CS logic level wrong        |
| f_mount returns FR_DISK_ERR | SPI speed too high during init; wiring issue |
| f_write fails            | Card full, or filesystem not FAT32           |
| Corrupt LOG.TXT          | Power removed without f_sync / f_close       |
