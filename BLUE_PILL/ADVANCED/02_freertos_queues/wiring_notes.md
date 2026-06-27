# Wiring Notes — 02_freertos_queues

## Voltage Warning

**ALL GPIO pins on the Blue Pill are 3.3 V logic.**
Do NOT apply voltages above 3.3 V to any pin, especially PA1 in ADC mode.
The STM32F103C8T6 analog inputs are NOT 5 V tolerant.

## Pin Usage

| Pin  | Function       | Direction | Notes                                      |
|------|----------------|-----------|--------------------------------------------|
| PA1  | ADC1 Ch1       | Analog In | 0–3.3 V only. Connect pot, LDR, or sensor  |
| PA9  | UART1 TX       | Output    | Connect to USB-TTL adapter RX              |
| PA10 | UART1 RX       | Input     | Connect to USB-TTL adapter TX (optional)   |
| PC13 | Onboard LED    | Output    | Active-low. Not used in this project       |

## ADC Input (PA1)

Connect a 10 kΩ potentiometer:

```
3.3V ──── pin1 (pot)
          pin2 (wiper) ──── PA1
          pin3 (pot) ──── GND
```

Or connect an LDR in a voltage divider:

```
3.3V ──── LDR ──┬──── PA1
                 |
               10 kΩ
                 |
                GND
```

## UART Serial Monitor

```
Blue Pill PA9  (TX) ──► USB-TTL RX
Blue Pill PA10 (RX) ◄── USB-TTL TX  (optional for this project)
Blue Pill GND       ──► USB-TTL GND
```

Open a terminal at **115200 baud, 8N1**. You should see ADC values printed
every ~200 ms.

## Queue Behavior Notes

- Queue depth is 8. If DisplayTask cannot keep up with SensorTask (unlikely at
  these rates), samples are dropped and `g_queueOverflows` increments.
- You can observe this variable in a debugger (ST-Link / OpenOCD).

## Power

Power via USB or the Blue Pill 5 V pin. Do not exceed 3.3 V on the 3.3 V pin.
