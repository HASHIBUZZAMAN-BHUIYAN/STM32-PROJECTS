# 13 — DHT22 Temperature & Humidity Sensor

## Overview
Reads temperature and humidity from a DHT22 sensor using a bit-banged
single-wire protocol on PB0.  TIM3 is run in free-running up-count mode with a
1 µs tick (PSC = 71 at 72 MHz, ARR = 0xFFFF) to provide precise microsecond
delays and pulse-width measurements without blocking `HAL_Delay`.

## Hardware
- Board  : STM32F103C8T6 (Blue Pill)
- Sensor : DHT22 (AM2302) on PB0
- Pull-up: 4.7 kΩ from PB0 to 3.3 V

## CubeMX Settings
| Peripheral | Setting |
|------------|---------|
| TIM3 | Up-counting, PSC = 71, ARR = 0xFFFF, no interrupts |
| PB0 | GPIO_Output initially; toggled to Input during read |
| USART1 | 115200 8N1, TX = PA9 |
| SYS | SysTick as timebase |

## Files
| File | Purpose |
|------|---------|
| `main_usercode.c` | DHT22 bit-bang driver + UART print |
| `wiring_notes.md` | Pin connections and hardware caveats |

## DHT22 Protocol (summary)
1. Host pulls data line LOW for ≥ 18 ms.
2. Host releases line (input, pulled HIGH by resistor).
3. Sensor pulls LOW 80 µs, then HIGH 80 µs (acknowledge).
4. 40 bits transmitted: each bit starts with 50 µs LOW, then:
   - "0" → ~26–28 µs HIGH
   - "1" → ~70 µs HIGH
5. 16-bit RH integral + 16-bit RH decimal + 16-bit T integral +
   16-bit T decimal + 8-bit checksum.

## Output
Results printed to UART1 every 2 seconds:
```
RH: 55.3 %  T: 24.1 C
```

## Note
Code is provided as a reference template. It has not been tested on hardware.
