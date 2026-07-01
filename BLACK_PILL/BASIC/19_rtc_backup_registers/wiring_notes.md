# Wiring Notes — Black Pill BASIC/19: RTC + Backup Registers

## No Extra Hardware Required
Uses onboard resources only. Optional: 3V coin cell on VBAT pin for RTC persistence after power-off.

## USART1 Connection
| Black Pill | USB-UART Adapter |
|-----------|----------------|
| PA9 (TX) | RX |
| GND | GND |

## VBAT for Real-Time Persistence
Without a battery, backup registers survive resets but not power-off.
To make the RTC and backup registers survive power-off:
1. Connect a 3V CR2032 coin cell to the VBAT pin
2. Add a Schottky diode between VDD and VBAT (anode to VDD, cathode to VBAT) to prevent VBAT from powering the MCU
3. The WeAct Black Pill has a VBAT pin broken out on the header

## RTC Clock Source (LSI vs LSE)
| Clock | Accuracy | Hardware needed |
|-------|----------|----------------|
| LSI (internal) | ±5% | None |
| LSE (external) | ±20ppm | 32.768kHz crystal |

Most WeAct Black Pill boards include the 32.768 kHz crystal (X2) for LSE. Use LSE for accurate timekeeping.

## Testing
1. Flash firmware
2. Open terminal at 115200
3. See "First boot: setting RTC..."
4. Press NRST
5. See "Reset #1 time=2025-01-01 12:00:xx"
6. Press NRST again → "Reset #2..."

## Parts List
| Component | Qty | Notes |
|-----------|-----|-------|
| Black Pill (STM32F411CEU6) | 1 | |
| USB-UART adapter | 1 | |
| CR2032 coin cell (optional) | 1 | For VBAT persistence |
| USB-C cable | 1 | |
