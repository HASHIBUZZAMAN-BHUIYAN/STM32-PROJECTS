# Wiring Notes — 09_i2c_bus_scan

## CRITICAL: 3.3 V Logic

**All STM32F103C8T6 GPIO pins operate at 3.3 V.**
PB6 (SCL) and PB7 (SDA) are 3.3 V open-drain signals.
Do not connect 5 V I2C devices directly. Many modules (OLED, LCD backpacks)
run on 5 V but have 3.3 V-tolerant I2C lines — check the datasheet.
If the device uses 5 V I2C logic, use a bidirectional I2C level shifter.

## I2C Pull-Up Resistors — REQUIRED

I2C is an open-drain bus. Both SCL and SDA need pull-up resistors to VCC.
Without pull-ups, I2C will not work.

| Line | Pull-up resistor | To |
|------|------------------|----|
| PB6 (SCL) | 4.7 kΩ (or 2.2 kΩ for 400 kHz) | 3.3 V |
| PB7 (SDA) | 4.7 kΩ (or 2.2 kΩ for 400 kHz) | 3.3 V |

Many breakout modules include onboard pull-ups. If your module has them,
external resistors may not be needed (check the module schematic).
Having two sets of pull-ups in parallel is usually fine but lowers effective
pull-up resistance — keep total pull-up resistance ≥ 1 kΩ.

## Connections

| Blue Pill Pin | Connect To | Notes |
|---------------|------------|-------|
| PB6 | Device SCL | + 4.7 kΩ pull-up to 3.3 V |
| PB7 | Device SDA | + 4.7 kΩ pull-up to 3.3 V |
| 3V3 | Device VCC | Only if device runs on 3.3 V |
| GND | Device GND | Common ground |
| PA9 | USB-TTL RX | 3.3 V UART TX for scan results |

## I2C Wiring Diagram

```
3V3 ──┬────[4.7kΩ]────┬── SCL ──┬── [Device SCL]
      │               │         │
      └────[4.7kΩ]────┴── SDA ──┴── [Device SDA]
                      
     PB6 ────────────────── SCL
     PB7 ────────────────── SDA
```

## Cable Length

Keep I2C wires short (< 30 cm at 100 kHz, < 10 cm at 400 kHz).
Long wires increase capacitance and cause signal edges to round off,
leading to communication errors.

## UART for Scan Output

Connect a 3.3 V USB-to-TTL adapter:

| Adapter | Blue Pill |
|---------|-----------|
| GND | GND |
| RX | PA9 (TX) |

Open terminal at **115200 8N1** to view scan results.

## Multiple Devices

Multiple I2C devices can share the same SCL/SDA lines.
Each device must have a unique 7-bit address.
Some devices (e.g., PCF8574) have address pins (A0/A1/A2) that can be
tied to GND or VCC to select from a range of addresses.
