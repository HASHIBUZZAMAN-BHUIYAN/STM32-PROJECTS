# Wiring Notes — Nucleo-F401RE ADVANCED/01: FreeRTOS Basics

## No Extra Hardware Required
Uses only onboard resources: LD2 (PA5), USART2 via ST-Link VCP.

## CubeMX FreeRTOS Setup (Critical Steps)
1. **Middleware → FreeRTOS → CMSIS-V2** — Enable
2. **Tasks and Queues tab** — leave default task (will be deleted in user code)
3. **Config Parameters** → `TOTAL_HEAP_SIZE` = 8192
4. **Config Parameters** → `configCHECK_FOR_STACK_OVERFLOW` = 2
5. **Pinout & Configuration → System Core → SYS** → Timebase Source = TIM11 (not SysTick)
6. Generate code

## Why TIM11 for HAL Timebase?
FreeRTOS uses SysTick exclusively for the RTOS tick. If HAL also uses SysTick (default), there's a conflict. CubeMX automatically moves HAL's `HAL_IncTick()` to TIM11 interrupt when FreeRTOS is enabled.

## Stack Size Guidance
| Task complexity | Stack (words) |
|----------------|--------------|
| Simple toggle, no printf | 64–128 |
| With snprintf/sprintf | 256–512 |
| With FatFs/USB | 512–1024 |

Always err on the side of larger stacks during development. Enable stack overflow check = 2 to catch issues early.

## Parts List
| Component | Qty |
|-----------|-----|
| Nucleo-F401RE | 1 |
| USB cable | 1 |
