# Wiring Notes — BASIC/06 ADC DMA

## Pin Diagram
Same as BASIC/05:
```
    3.3V (CN8 pin 4)
      │
     [10kΩ Pot]── Wiper ──► PA0 / A0 (CN8 pin 1)
      │
    GND (CN8 pin 6)
```

## Parts List
| Item | Qty | Notes |
|------|-----|-------|
| Nucleo-F401RE | 1 | |
| 10kΩ potentiometer | 1 | |
| Breadboard + jumpers | 1 set | |

## Safety Notes
⚠ ALL GPIO pins operate at 3.3V logic. NEVER apply more than 3.3V to PA0.
- DMA circular mode fills the buffer continuously without CPU intervention.
- Ensure DMA2 Stream 0 Channel 0 is correctly assigned in CubeMX (ADC1 DMA request).
- The buffer is read by the main loop; no mutex needed for single-core MCU if reads are atomic (uint16_t aligned).

## DMA Configuration Notes
- DMA2 Stream 0 Channel 0 is the correct DMA stream for ADC1 on STM32F401.
- Set Memory Data Width to Half Word (16-bit) to match uint16_t buffer.
- Set Peripheral Data Width to Half Word.
- Circular mode ensures automatic wrap-around after 32 samples.
