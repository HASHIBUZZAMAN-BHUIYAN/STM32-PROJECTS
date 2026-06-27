# Wiring Notes — 05 ADC Potentiometer

## Voltage Warning
**All GPIO pins on the STM32F103C8T6 Blue Pill operate at 3.3 V logic.**
The ADC reference is VDDA = 3.3 V. Do NOT apply more than 3.3 V to PA1 — exceeding this will damage the ADC input and may destroy the MCU.

## Connections

### Potentiometer
| Pot Leg | Blue Pill Pin | Notes |
|---------|--------------|-------|
| End leg A | 3.3V | Supplies full-scale voltage |
| Wiper (middle) | PA1 | ADC1 Channel 1 — analog input |
| End leg B | GND | Sets zero-scale |

With a 10 kΩ linear potentiometer:
- Wiper at end A → PA1 ≈ 3.3 V → ADC = 4095 → 100 %
- Wiper at center → PA1 ≈ 1.65 V → ADC ≈ 2048 → 50 %
- Wiper at end B → PA1 ≈ 0 V → ADC = 0 → 0 %

### UART (USB-to-Serial adapter, 3.3 V logic required)
| Signal | Blue Pill Pin | Adapter Pin | Notes |
|--------|--------------|-------------|-------|
| TX | PA9 | RX | Blue Pill transmits; adapter receives |
| RX | PA10 | TX | Blue Pill receives; adapter transmits |
| GND | GND | GND | Common ground — REQUIRED |

**Do NOT connect a 5 V USB-serial adapter directly.** Use an adapter with a 3.3 V logic level, or add a 3.3 V ↔ 5 V level-shifter. Some adapters (e.g., FTDI FT232RL) have a 3.3 V mode selectable via a jumper.

### Programming
| Signal | Blue Pill Pin | Notes |
|--------|--------------|-------|
| ST-Link SWDIO | PA13 | — |
| ST-Link SWCLK | PA14 | — |
| ST-Link GND | GND | — |

## Schematic

```
3.3V ──────────────[Pot end A]
                   [Pot wiper]── PA1 (ADC1 IN1)
                   [Pot end B]
GND ───────────────────────────────────────────

PA9  (TX) ──────────────────────── RX  (USB-serial adapter)
PA10 (RX) ──────────────────────── TX  (USB-serial adapter)
GND       ──────────────────────── GND (USB-serial adapter)
```

## Serial Terminal Settings
| Parameter | Value |
|-----------|-------|
| Baud rate | 115200 |
| Data bits | 8 |
| Parity | None |
| Stop bits | 1 |
| Flow control | None |

## ADC Input Impedance
For accurate readings the source impedance driving PA1 should be below ~10 kΩ (STM32F1 datasheet recommendation). A 10 kΩ pot is at the boundary; a 1 kΩ or 4.7 kΩ pot gives better accuracy. Alternatively, add a unity-gain op-amp buffer if using a high-impedance source.

## PA0 vs PA1
- PA0 = ADC1 Channel 0 (also TIM2 CH1 / button in other projects)
- PA1 = ADC1 Channel 1 (this project)
Verify the CubeMX pin assignment before building.

## Boot Jumpers
BOOT0 = 0, BOOT1 = 0 to boot from Flash.
