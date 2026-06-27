# Wiring Notes — 17 Servo PWM

## Voltage Warning
**All GPIO pins on the Blue Pill operate at 3.3 V logic.**
Most hobby servos accept a 3.3 V PWM signal even though they are powered at 5 V. Verify your specific servo's datasheet — some low-cost servos require 5 V logic on the signal line. If needed, use a 3.3 V → 5 V level shifter on the signal wire.

## Pin Connections

| Signal | Blue Pill Pin | Servo Wire Color (typical) | Notes |
|--------|--------------|---------------------------|-------|
| PWM Signal | PA1 (TIM2 CH2) | Orange / Yellow / White | 50 Hz, 1000–2000 µs pulse |
| Servo VCC | External 5 V | Red | Do NOT power from the Blue Pill 3.3 V pin — servos can draw 500 mA+ under load |
| Servo GND | GND (shared) | Brown / Black | Common ground between Blue Pill and servo supply |

## Power Supply
- Power the servo from a dedicated 5 V supply (e.g., external USB power bank, 4× AA batteries, or a DC-DC buck converter).
- Share the GND between the Blue Pill and the servo supply.
- Do NOT draw servo power from the Blue Pill's onboard 3.3 V regulator — it is rated for only ~150–300 mA and servo stall current can exceed this.

## Servo Connector (standard JR/Futaba 3-pin)
```
Signal  (PA1) ──────── [Orange/Yellow]
+5V (external) ─────── [Red]
GND (shared)  ──────── [Brown/Black]
```

## Pulse Width Reference
| Pulse Width | Angle |
|------------|-------|
| 1000 µs | 0° |
| 1500 µs | 90° (centre) |
| 2000 µs | 180° |

Note: Exact angles vary by servo model. Some servos accept 500–2500 µs for a wider range; test carefully to avoid mechanical hard-stops.

## CubeMX Timer Settings Reminder
- TIM2, CH2 (PA1)
- Prescaler: 71
- Counter Period (ARR): 19999
- This yields exactly 50 Hz with 1 µs resolution per timer count.
