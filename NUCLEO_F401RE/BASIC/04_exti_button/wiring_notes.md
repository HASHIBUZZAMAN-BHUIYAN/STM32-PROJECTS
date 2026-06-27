# Wiring Notes — BASIC/04 EXTI Button

## Pin Diagram
```
Nucleo-F401RE
+-------------------------------+
| PC13 ◄── B1 (onboard button)  |
| PA2  ──► ST-Link → USB → PC   |
| PA3  ◄── ST-Link ← USB ← PC   |
+-------------------------------+
```

## Parts List
| Item | Qty | Notes |
|------|-----|-------|
| Nucleo-F401RE | 1 | |
| USB cable | 1 | ST-Link VCP (no adapter needed) |
| PC with terminal emulator | 1 | 115200 baud, 8N1 |

## Safety Notes
⚠ ALL GPIO pins operate at 3.3V logic. Do NOT apply 5V to any GPIO pin.
- All connections are onboard — no external wiring risk.
- The ST-Link on the Nucleo automatically creates a virtual COM port when connected via USB.

## Terminal Setup
- Windows: Check Device Manager for "STMicroelectronics Virtual COM Port" COMx
- Linux: /dev/ttyACM0 or /dev/ttyUSB0
- Settings: 115200 baud, 8 data bits, No parity, 1 stop bit, No flow control
