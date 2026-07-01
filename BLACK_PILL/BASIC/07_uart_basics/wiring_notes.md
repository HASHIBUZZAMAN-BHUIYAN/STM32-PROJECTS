# Wiring Notes — Black Pill BASIC/07: UART Basics

## Pin Assignment
| Black Pill Pin | Signal | USB-UART Adapter |
|---------------|--------|-----------------|
| PA9 | USART1_TX | RX (adapter receives) |
| PA10 | USART1_RX | TX (adapter sends) |
| GND | GND | GND |

**Cross-connection**: STM32 TX → adapter RX, STM32 RX → adapter TX.

## Common USB-UART Adapters
| Adapter | Chip | Max voltage |
|---------|------|------------|
| CP2102 module | SiLabs CP2102 | 3.3V/5V selectable |
| CH340G module | WCH CH340G | 3.3V/5V selectable |
| FTDI FT232RL | FTDI | 3.3V/5V selectable |

**Always select 3.3V mode** when connecting to Black Pill GPIO. 5V UART signals will damage the STM32.

## Terminal Settings
- Baud: 115200
- Data: 8 bits
- Parity: None
- Stop: 1 bit
- Flow control: None

## PA10 Floating Quirk
If you see garbage characters at startup before pressing any key, add a 10kΩ pull-up resistor from PA10 to 3.3V.

## Power Note
Power the Black Pill via its USB-C connector. Do NOT also connect the USB-UART adapter's VCC to the Black Pill 3.3V pin — this creates a power conflict.

## Parts List
| Component | Qty |
|-----------|-----|
| Black Pill (STM32F411CEU6) | 1 |
| USB-UART adapter (3.3V logic) | 1 |
| USB-C cable (for Black Pill) | 1 |
| USB-A cable (for adapter) | 1 |
| Jumper wires | 3 |
