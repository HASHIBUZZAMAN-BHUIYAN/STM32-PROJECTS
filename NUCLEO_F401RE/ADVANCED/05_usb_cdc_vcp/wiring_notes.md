# Wiring Notes — Nucleo-F401RE ADVANCED/05: USB CDC VCP

## USB Connection (Critical)
The Nucleo has TWO USB paths:
1. **CN1 (mini-USB)** — goes to ST-Link only. Cannot be used for STM32 USB CDC.
2. **STM32 PA11/PA12** — direct STM32 USB pins. Use these for CDC.

You need to wire PA11/PA12 to an external USB connector:

```
PA11 ── 22Ω ── USB_D- (pin 2 of Type-A or Micro-B)
PA12 ── 22Ω ── USB_D+ (pin 3 of Type-A or Micro-B)
GND  ──────── USB_GND (pin 4 of Type-A, pin 5 of Micro-B)
```

22Ω series resistors are recommended for USB signal integrity (optional but good practice).

## Easy Alternative: USB Micro-B Breakout
Buy a Micro-USB breakout board (~$1 on AliExpress):
- VBUS → (don't connect to 3.3V; board is powered via CN1 ST-Link)
- D- → PA11 (via 22Ω optional)
- D+ → PA12 (via 22Ω optional)
- GND → GND

## PA11/PA12 Availability Check
On the Nucleo-F401RE morpho connector (CN7/CN10):
- PA11 is on CN10 pin 14
- PA12 is on CN10 pin 12

## ADC Test Input
| Pin | Signal |
|-----|--------|
| PA0 | ADC1_IN0 (connect pot or leave floating for noise) |

## Windows CDC Driver
Windows 10/11: automatic (built-in CDC ACM driver)  
Windows 7: install ST VCP driver from `st.com/stm32`

After plugging in, check Device Manager → Ports (COM & LPT) for "STMicroelectronics Virtual COM Port"

## 3.3V Safety
PA11/PA12 are 3.3V signals — USB data lines run at 3.3V. Do not connect them to 5V USB VBUS.

## Parts List
| Component | Qty | Notes |
|-----------|-----|-------|
| Nucleo-F401RE | 1 | |
| Micro-USB breakout board | 1 | For STM32 USB D+/D- |
| 22Ω resistors | 2 | Series termination (optional) |
| USB micro-B cable | 1 | Second cable beyond ST-Link |
| Breadboard + jumpers | 1 set | |
