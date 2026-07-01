# Wiring Notes — Black Pill ADVANCED/08: Bootloader Concepts

## USART1 Connection
| Black Pill | USB-UART |
|-----------|---------|
| PA9 (TX) | RX |
| PA10 (RX) | TX |
| GND | GND |

## Flash Sector Map (STM32F411)
| Sector | Start | Size | Use |
|--------|-------|------|-----|
| 0 | 0x08000000 | 16KB | Bootloader (this project) |
| 1 | 0x08004000 | 16KB | Reserved |
| 2 | 0x08008000 | 16KB | Reserved |
| 3 | 0x0800C000 | 16KB | Reserved |
| 4 | 0x08010000 | 64KB | Application slot |
| 5 | 0x08020000 | 128KB | Extended application |
| 6 | 0x08040000 | 128KB | Available |
| 7 | 0x08060000 | 128KB | Available |

## Application Linker Script Change
For the application to run from 0x08010000:
```
MEMORY {
  FLASH (rx) : ORIGIN = 0x08010000, LENGTH = 64K
  RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 128K
}
```

## XMODEM Transfer (minicom/tera term)
```
minicom: Ctrl+A, S, xmodem, select .bin file
TeraTerm: File > Transfer > XMODEM > Send
```

## Parts List
| Component | Qty |
|-----------|-----|
| Black Pill (STM32F411CEU6) | 1 |
| USB-UART adapter | 1 |
| Jumper wires | 3 |
