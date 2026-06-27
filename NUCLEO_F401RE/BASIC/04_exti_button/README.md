# BASIC/04 — External Interrupt Button (EXTI13)

Uses PC13 (B1) as an external interrupt source (EXTI line 13, falling edge). Each button press increments a counter. The main loop prints the counter value via UART2 (ST-Link VCP) every second.

## CubeMX Configuration
- Board: NUCLEO-F401RE
- PC13: GPIO_EXTI13
  - GPIO mode: External Interrupt Mode with Falling edge trigger detection
  - GPIO Pull-up/Pull-down: Pull-up
  - Label: B1_Pin
- System Core → NVIC:
  - EXTI line[15:10] interrupts: Enabled, Priority 2 / Sub-priority 0
- Connectivity → USART2:
  - Mode: Asynchronous
  - Baud Rate: 115200, 8N1
  - PA2: USART2_TX, PA3: USART2_RX
- System Core → SYS → Debug: Serial Wire
- Clock: 84 MHz

## Parts List
- Nucleo-F401RE board
- USB cable (provides VCP — no USB-serial adapter needed)
- Terminal emulator on PC (e.g. PuTTY, Tera Term) at 115200 baud

## Wiring
No external wiring required.

## Build and Flash
1. Configure CubeMX as above.
2. Generate code, paste main_usercode.c sections.
3. Build, flash.
4. Open terminal at 115200 baud on the ST-Link COM port.

## Expected Behavior
Terminal shows "Button count: N" every second. Press B1 to increment N. Interrupt fires on falling edge (press), not release.
