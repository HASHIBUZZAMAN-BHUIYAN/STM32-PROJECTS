# Wiring Notes — 13_dht22_sensor

## Voltage Warning
> **ALL Blue Pill GPIO pins operate at 3.3 V logic.**
> The DHT22 data line must not exceed 3.3 V when connected to PB0.
> The DHT22 can be powered from 3.3 V or 5 V.  When powered at 5 V its
> data line may swing to ~5 V — use a 3.3 V supply or add a level-shifter /
> voltage divider on the data line if powering at 5 V.
> Recommended: power the DHT22 from the Blue Pill's **3.3 V** pin.

---

## Pin Connections

### DHT22 (AM2302)
| Blue Pill Pin | Signal | DHT22 Pin | Notes |
|---------------|--------|-----------|-------|
| PB0 | Data | DATA (pin 2) | Single-wire protocol |
| 3.3V | Power | VCC (pin 1) | Preferred supply |
| GND | Ground | GND (pin 4) | Common ground |
| — | NC | NC (pin 3) | Leave pin 3 unconnected |

### Pull-up Resistor (mandatory)
| Signal | Resistor | To |
|--------|----------|----|
| PB0 (DATA) | 4.7 kΩ | 3.3 V |

The pull-up resistor is **required** by the DHT22 single-wire protocol.
Without it, the bus will not return HIGH and all reads will time out.

### UART1 (output)
| Blue Pill Pin | Signal | USB-UART Adapter |
|---------------|--------|-----------------|
| PA9 | TX | RX |
| GND | GND | GND |

### Onboard LED (PC13)
| Blue Pill Pin | Function | Notes |
|---------------|----------|-------|
| PC13 | Onboard LED (active-low) | Toggles every successful read |

---

## DHT22 vs DHT11
This code targets the **DHT22 (AM2302)** only.
- DHT22: 16-bit humidity, 16-bit temperature (0.1 resolution), sign bit
- DHT11: 8-bit values, no negative temperature, different timing

Do not substitute a DHT11 without modifying the parsing logic.

---

## Timing Sensitivity
The bit-bang protocol depends on accurate microsecond timing via TIM3.
Critical requirements:
- TIM3 PSC = 71, ARR = 0xFFFF at 72 MHz → 1 µs per tick
- No interrupt-driven code must preempt the read sequence (no heavy ISRs
  during the 5 ms read window)
- HAL SysTick ISR (1 ms) is acceptable — it introduces only ~1 µs jitter
  which is within the DHT22's tolerance

If reads are consistently failing, try disabling the SysTick interrupt during
the read with `__disable_irq()` / `__enable_irq()` as a diagnostic step.

---

## Minimum Read Interval
The DHT22 requires a minimum **2-second** interval between consecutive reads.
Reading more frequently may return stale data or cause the sensor to stop
responding.  The main loop uses `HAL_Delay(2000)`.

---

## Common Issues

| Symptom | Likely Cause | Fix |
|---------|-------------|-----|
| Always "read error" | Missing 4.7 kΩ pull-up | Add resistor from PB0 to 3.3 V |
| Checksum failure | Timing too fast / slow | Verify TIM3 PSC=71 at 72 MHz |
| Sensor heats up / smokes | 5 V connected to 3.3 V MCU pin | Use 3.3 V supply; add level shifter |
| First read always fails | Sensor warm-up | Wait 1–2 s after power-on before first read |
| Temperature negative unexpectedly | Sign bit misparse | Check `data[2] & 0x80` logic |

---

## PC13 / BKP Quirk
This project does not use the RTC.  PC13 should behave normally as an
active-low LED output.  See project 15 for BKP/ASOE details if needed.
