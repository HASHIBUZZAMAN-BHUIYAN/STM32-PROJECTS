# Wiring Notes — Black Pill ADVANCED/06: UART Framed Protocol

## USART1 (DMA RX)
| Black Pill | USB-UART |
|-----------|---------|
| PA9 (TX) | RX |
| PA10 (RX) | TX |
| GND | GND |

## BMP280 (I2C1)
| BMP280 | Black Pill |
|--------|-----------|
| VCC | 3.3V |
| GND | GND |
| SCL | PB6 |
| SDA | PB7 |
| SDO | GND (addr 0x76) |

## CubeMX DMA RX Setup
1. USART1 → DMA Settings → Add → DMA1 Stream2
2. Direction: Peripheral-to-Memory
3. Mode: Circular
4. Data Width: Byte / Byte
5. Memory Increment: Enabled

## Testing with Python
```python
import serial, struct, time
s = serial.Serial('COM3', 115200, timeout=1)

def send_cmd(cmd, payload=b''):
    pay = bytes([cmd]) + payload
    ln = len(pay)
    frame = bytes([0x55,0xAA,ln>>8,ln&0xFF]) + pay
    crc = 0xFFFF
    for b in frame[2:]:
        crc ^= b<<8
        for _ in range(8): crc = (crc<<1)^0x1021 if crc&0x8000 else crc<<1
    crc &= 0xFFFF
    s.write(frame + bytes([crc>>8, crc&0xFF]))
    return s.read(32)

send_cmd(0x01)         # PING
send_cmd(0x03)         # READ_ALL
send_cmd(0x04, b'\x01')  # LED ON
```

## Parts List
| Component | Qty |
|-----------|-----|
| Black Pill (STM32F411CEU6) | 1 |
| BMP280 module | 1 |
| USB-UART adapter | 1 |
| Jumper wires | 7 |
