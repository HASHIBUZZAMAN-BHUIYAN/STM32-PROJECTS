# Wiring Notes — Project 26: DMA UART TX

## Voltage Warning

**ALL Blue Pill GPIO pins operate at 3.3 V logic.**
Do NOT connect 5 V signals directly to any STM32 pin. Use a level shifter or voltage divider if interfacing with 5 V devices. The USB-to-serial adapter's TX/RX lines must be 3.3 V compatible (e.g., CP2102, CH340 running at 3.3 V).

---

## Connections

### USART1 (PA9 TX / PA10 RX)

| Blue Pill Pin | USB-Serial Adapter Pin | Notes |
|---|---|---|
| PA9 (TX) | RX | Crossover: Blue Pill TX → Adapter RX |
| PA10 (RX) | TX | Crossover: Blue Pill RX → Adapter TX |
| GND | GND | Common ground is mandatory |
| 3.3V | — | Do NOT power Blue Pill from adapter 5V rail via this pin |

Only PA9 (TX) is strictly needed for this project since it only transmits.

### ADC Input (PA0)

| Blue Pill Pin | Signal | Notes |
|---|---|---|
| PA0 | Analog input 0–3.3 V | 3.3 V max — never exceed |
| GND | GND reference | |

A 10 kΩ potentiometer between 3.3 V and GND with the wiper to PA0 is the simplest test input.
- Wiper at 3.3 V → raw ≈ 4095
- Wiper at GND → raw ≈ 0

### Power

| Blue Pill Pin | Source |
|---|---|
| 5V pin | USB-C or 5 V supply (has onboard 3.3 V LDO) |
| GND | Common GND with all peripherals |

---

## DMA Note

DMA1 Channel 4 is the hardware-assigned channel for USART1_TX on the STM32F103.
This is fixed in silicon — you cannot change it in CubeMX. Ensure the DMA request in CubeMX is set to **USART1_TX → DMA1 Channel 4, Direction: Memory to Peripheral, Mode: Normal**.

---

## Serial Terminal Settings

| Parameter | Value |
|---|---|
| Baud rate | 115200 |
| Data bits | 8 |
| Parity | None |
| Stop bits | 1 |
| Flow control | None |

Recommended terminals: PuTTY, Tera Term, minicom, or the Arduino IDE Serial Monitor.
