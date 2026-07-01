# Black Pill ADVANCED/03 — FreeRTOS Mutex & Priority Inheritance

## Overview
Demonstrates priority inheritance: a low-priority task holds a shared resource mutex, a high-priority task blocks waiting for it, FreeRTOS temporarily boosts the low task's priority to the high task's level to prevent priority inversion.

## Priority Inversion Demo
```
High (P7) blocks on mutex held by Low (P3)
  → FreeRTOS boosts Low to P7 (priority inheritance)
  → Low finishes critical section, releases mutex
  → High resumes at full P7; Low drops back to P3
  → Med (P5) was starved during this; resumes after
```

## Tasks
| Task | Priority | Period | Action |
|------|----------|--------|--------|
| High | AboveNormal+2 | 1s | Acquire sharedMtx, compute, release |
| Med | AboveNormal | 500ms | Loop counting (shows starvation) |
| Low | Normal | 2s | Hold sharedMtx for 300ms (simulates slow work) |
| Log | Low | 2s | Print priority snapshot |

## CubeMX
FreeRTOS CMSIS-V2, TIM11 timebase, USART1 115200, PC13 LED, 100 MHz.
