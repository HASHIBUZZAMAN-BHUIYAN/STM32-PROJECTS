# Wiring Notes — 06_adc_dma

## CRITICAL: 3.3 V Logic

**All STM32F103C8T6 GPIO and analog pins operate at 3.3 V.**
Never apply more than 3.3 V to any pin, including PA1.
Applying 5 V will permanently damage the MCU.

## Connections

| Blue Pill Pin | Connect To | Notes |
|---------------|------------|-------|
| PA1 | Potentiometer wiper | See diagram below |
| 3V3 | Pot high end | Board 3.3 V rail |
| GND | Pot low end | Common ground |
| PC13 | — | Onboard LED, no external wiring needed |

## Potentiometer Wiring Diagram

```
3V3 ──┬── [Pot Pin 1]
      │
      └── [Pot Wiper] ──── PA1
      
GND ──── [Pot Pin 3]
```

A 10 kΩ potentiometer is a good choice. The wiper sweeps 0 V to 3.3 V,
producing ADC readings 0 to 4095.

## Analog Input Notes

- PA1 is a 12-bit ADC input — input impedance should be low (pot < 10 kΩ recommended).
- Add a 100 nF decoupling capacitor from PA1 to GND near the pin to reduce noise.
- The VREF+ for ADC on this MCU is tied to VDD (3.3 V). There is no separate VREF pin on this package.
- If driving PA1 from a sensor with output > 3.3 V, use a resistor voltage divider to scale down.

## DMA Notes

- DMA1 Channel 1 is reserved for ADC1 on STM32F103. Do not assign anything else to it.
- The `adc_dma_buf` array must be a global (or static) variable — DMA cannot access stack memory reliably.

## Power

- Power the Blue Pill from USB (5 V to the USB connector) or from the 3V3 pin (regulated 3.3 V).
- Do NOT simultaneously power via USB and apply 5 V to the 5V pin without checking your board's LDO.
