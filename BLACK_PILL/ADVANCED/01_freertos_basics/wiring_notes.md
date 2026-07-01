# Wiring Notes — Black Pill ADVANCED/01: FreeRTOS Basics

## No Extra Hardware Required
USART1 via USB-UART adapter, PC13 LED.

## USART1 Connection
| Black Pill | USB-UART Adapter |
|-----------|----------------|
| PA9 (TX) | RX |
| GND | GND |

## CubeMX FreeRTOS Setup
1. Middleware → FreeRTOS → CMSIS-V2 → Enable
2. TOTAL_HEAP_SIZE = 12288 (12KB — larger than F401 which had 8KB)
3. configCHECK_FOR_STACK_OVERFLOW = 2
4. SYS Timebase = TIM11 (frees SysTick for FreeRTOS)

## 128KB RAM Advantage
Black Pill F411 has 128KB RAM vs 96KB on F401. This allows:
- Larger heap (12KB vs 8KB)
- More and bigger task stacks
- Larger queues

## Parts List
| Component | Qty |
|-----------|-----|
| Black Pill (STM32F411CEU6) | 1 |
| USB-UART adapter | 1 |
| USB-C cable | 1 |
| Jumper wires | 2 |
