# Wiring Notes — Nucleo-F401RE BASIC/14: Onboard User Button

## No Extra Wiring Needed
All components are onboard the Nucleo-F401RE.

## Onboard Resources
| Resource | Pin | Notes |
|----------|-----|-------|
| B1 User Button | PC13 | Active-LOW; internal pull-up via board resistor |
| LD2 LED | PA5 | Active-HIGH |
| ST-Link VCP | PA2 (TX) / PA3 (RX) | Serial output via USB CN1 |

## CubeMX Note
When selecting board "NUCLEO-F401RE" in CubeMX, PC13 is pre-configured as an external interrupt input. If using raw MCU (STM32F401RETx), configure manually:
- PC13 → GPIO_EXTI13
- Trigger: Falling edge (button press)
- Pull: Pull-up
- NVIC: EXTI15_10 enabled, priority 2

## Important: EXTI13 vs PC13 LED Confusion
On the Blue Pill, PC13 is an OUTPUT (LED). On the Nucleo-F401RE, PC13 is an INPUT (B1 user button). The LED is on PA5. These boards have opposite uses for PC13.
