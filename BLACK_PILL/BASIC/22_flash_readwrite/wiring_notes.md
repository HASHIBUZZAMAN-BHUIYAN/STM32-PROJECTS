# Wiring Notes — Black Pill BASIC/22: Flash Read/Write

## No Extra Hardware Required
Uses USART1 via USB-UART adapter for output.

## USART1 Connection
| Black Pill | USB-UART Adapter |
|-----------|----------------|
| PA9 (TX) | RX |
| GND | GND |

## Linker Script Modification (Critical)
Open the generated linker script (e.g., `STM32F411CEUX_FLASH.ld`):
```ld
MEMORY
{
  RAM    (xrw) : ORIGIN = 0x20000000, LENGTH = 128K
  FLASH  (rx)  : ORIGIN = 0x08000000, LENGTH = 384K  /* was 512K */
}
```
Reducing FLASH to 384K prevents the linker from placing code in Sector 7 (0x08060000).

Without this change, if your firmware is large enough, code could be placed in Sector 7 — which your firmware then erases, causing a crash or infinite reboot loop.

## Flash Write Endurance
~10,000 erase/program cycles before wear-out. This demo erases once per boot — safe for development. Production systems should implement wear leveling.

## FLASH_VOLTAGE_RANGE_3
This flag sets the programming parallelism. At VDD=3.3V, use `FLASH_VOLTAGE_RANGE_3` (32-bit parallel writes). Do NOT use VOLTAGE_RANGE_4 (64-bit) at 3.3V — requires VDD > 2.7V and explicit 64-bit mode support.

## Parts List
| Component | Qty |
|-----------|-----|
| Black Pill (STM32F411CEU6) | 1 |
| USB-UART adapter | 1 |
| USB-C cable | 1 |
| Jumper wires | 2 |
