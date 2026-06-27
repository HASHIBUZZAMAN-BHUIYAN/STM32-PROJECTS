# 06 — UART Framed Protocol

## Overview
Implements a custom binary framing protocol over UART1. The host sends structured frames; the STM32 parses them via interrupt-driven RX, executes commands (ADC read, LED control, BMP280 temperature), and transmits framed responses.

## Frame Format
```
[0xAA] [LEN] [CMD] [PAYLOAD...] [CRC8]
```
| Field   | Size    | Description                                  |
|---------|---------|----------------------------------------------|
| SOF     | 1 byte  | Start-of-frame marker: `0xAA`                |
| LEN     | 1 byte  | Number of bytes that follow: `CMD + PAYLOAD` |
| CMD     | 1 byte  | Command byte (see below)                     |
| PAYLOAD | 0–N     | Command-specific data                        |
| CRC8    | 1 byte  | CRC8 over `LEN + CMD + PAYLOAD` (poly 0x07)  |

## Commands
| CMD  | Name       | Payload (TX) | Response Payload           |
|------|------------|--------------|----------------------------|
| 0x01 | GET_ADC    | none         | 2 bytes: ADC value (LE)    |
| 0x02 | SET_LED    | 1 byte: 0/1  | 1 byte: echo of LED state  |
| 0x03 | GET_TEMP   | none         | 4 bytes: temp × 100 (int32)|

## CRC8 Algorithm
- Polynomial: `0x07` (standard CRC-8 / SMBUS)
- Initial value: `0x00`
- Computed over `LEN + CMD + PAYLOAD` bytes (excludes SOF and CRC itself)

## Error Handling
- Framing resync on SOF mismatch — discard bytes until `0xAA` is found
- CRC mismatch — send NACK frame (`CMD = 0xFF`, payload = bad CRC byte)
- LEN > MAX_PAYLOAD — discard and resync
- Timeout on partial frame — reset state machine after 100 ms idle

## CubeMX Configuration
| Peripheral | Setting                                      |
|------------|----------------------------------------------|
| USART1     | 115200 baud, 8N1, RX Interrupt enabled       |
| ADC1       | Channel 0 (PA0), single conversion, SW start |
| I2C1       | Standard mode 100 kHz, PB6=SCL, PB7=SDA     |
| NVIC       | USART1 global interrupt, priority 5          |
| SYS        | Timebase: TIM4 (free TIM1/2 for user use)    |

## BMP280 Notes
- I2C address: `0x76` (SDO to GND) or `0x77` (SDO to VCC)
- Temperature register: `0xFA–0xFC` (raw), requires compensation formula
- Project uses a simplified integer compensation for demonstration

## Files
| File              | Purpose                          |
|-------------------|----------------------------------|
| `main_usercode.c` | All user code sections           |
| `wiring_notes.md` | Hardware connection guide        |

## Not Tested
This code has not been compiled or run on hardware. It is provided as a reference implementation for study and adaptation.
