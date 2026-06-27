# Wiring Notes — 04_pid_controller

## Voltage Warning

**ALL GPIO pins on the Blue Pill are 3.3 V logic.**
PA1 (ADC input) must receive 0–3.3 V only. Do NOT exceed 3.3 V on analog pins.
PA8 (PWM output) is a 3.3 V push-pull output. Drive an LED through a current-
limiting resistor — do not connect the LED directly to 3.3 V without a resistor.

## Pin Usage

| Pin | Function      | Direction  | Notes                                      |
|-----|---------------|------------|--------------------------------------------|
| PA8 | TIM1 CH1 PWM  | Output     | LED drive signal (0–3.3 V, 100 kHz)        |
| PA1 | ADC1 Ch1      | Analog In  | LDR voltage divider output (0–3.3 V)       |
| PA9 | UART1 TX      | Output     | Serial debug / command output              |
| PA10| UART1 RX      | Input      | UART command input                         |

## LED Circuit (PA8 → LED → LDR Feedback)

```
PA8 ──── 220 Ω ──── LED(+)
                     LED(-) ──── GND
```

Use a standard 5 mm red/green/white LED. At 3.3 V and 220 Ω the current is
about 10 mA — within GPIO drive limits. Do not omit the resistor.

## LDR Voltage Divider (PA1 Feedback)

Mount the LDR so it faces the LED controlled by PA8.

```
3.3V ──── LDR ──┬──── PA1 (ADC input)
                 |
               10 kΩ (fixed resistor)
                 |
                GND
```

As the LED brightens, more light falls on the LDR, its resistance drops,
and the PA1 voltage rises. The PID controller reduces PWM duty cycle to
bring the reading back to the setpoint.

Calibration tip: in a dark room, open-circuit LDR resistance can be > 1 MΩ
(PA1 near 0 V). Under bright LED illumination, LDR resistance may drop to
1–5 kΩ (PA1 near 2.5–3.0 V with 10 kΩ fixed resistor).

## TIM4 Interrupt — 100 Hz

CubeMX settings for 100 Hz timer interrupt at 72 MHz SYSCLK:
- PSC = 7199  (divides 72 MHz → 10 kHz)
- ARR = 99    (counts 0–99 → overflows 100 times per second)

Verify in CubeMX: TIM4 → Counter Period = 99, Prescaler = 7199, trigger NVIC interrupt.

## TIM1 PWM — 100 kHz

CubeMX settings for 100 kHz PWM at 72 MHz:
- PSC = 0     (no prescaler — 72 MHz timer clock)
- ARR = 719   (period = 720 counts → 72 MHz / 720 = 100 kHz)

`__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, N)` sets duty to N/720.

## UART

```
Blue Pill PA9  (TX) ──► USB-TTL RX
Blue Pill PA10 (RX) ◄── USB-TTL TX
Blue Pill GND       ──► USB-TTL GND
```

115200 baud, 8N1. Send commands terminated with `\r\n`:
- `S2048\r\n` — set midscale setpoint
- `R\r\n`     — report current state

## Physical Layout Tip

Place the LED and LDR inside a small enclosure (cardboard tube, heat-shrink
sleeve) to isolate them from ambient light, which would otherwise add a DC
offset to the LDR reading and interfere with closed-loop control.
