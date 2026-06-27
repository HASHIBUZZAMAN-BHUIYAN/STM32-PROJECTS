# BASIC/05 — ADC Potentiometer (Poll Mode)

Reads a potentiometer (or any 0–3.3V analog signal) on ADC1 Channel 0 (PA0 = Arduino A0) in polling mode. Prints the raw 12-bit ADC value (0–4095) and the calculated voltage via UART2 every 500 ms.

## CubeMX Configuration
- Board: NUCLEO-F401RE
- Analog → ADC1:
  - IN0: IN0 Single-Ended
  - Resolution: 12 bits
  - Scan Conversion Mode: Disabled
  - Continuous Conversion: Disabled
  - Discontinuous Conversion: Disabled
  - End Of Conversion Selection: EOC flag at end of single channel conversion
  - Data Alignment: Right alignment
  - Sampling Time: 84 Cycles (gives stable reading)
- Connectivity → USART2: Asynchronous, 115200 baud, 8N1
- PA0: ADC1_IN0 (auto-assigned)
- Clock: 84 MHz (ADC clock will be 84/4 = 21 MHz, within spec)

## Parts List
| Item | Qty | Notes |
|------|-----|-------|
| Nucleo-F401RE | 1 | |
| Potentiometer 10kΩ | 1 | Or any 0–3.3V source |
| Breadboard + jumpers | 1 set | |

## Wiring
- Pot wiper → PA0 (Arduino A0, CN8 pin 1)
- Pot end 1 → 3.3V (CN6 pin 4 or CN8 pin 4)
- Pot end 2 → GND (CN6 pin 6 or CN8 pin 6)

## Expected Behavior
Terminal shows:
  ADC raw: 2048  Voltage: 1.650 V
Updated every 500 ms as pot is turned.
