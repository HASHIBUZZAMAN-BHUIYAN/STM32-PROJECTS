# Black Pill BASIC/27 — State Machine (Traffic Light Controller)

## Overview
Implements a finite state machine for a 3-phase traffic light using GPIO outputs and HAL_GetTick() timing. An emergency button (PA0) triggers an immediate EMERGENCY state that overrides the normal cycle. Demonstrates clean state machine design in embedded C.

## States
```
IDLE ──→ RED ──→ RED_AMBER ──→ GREEN ──→ AMBER ──→ RED (loop)
              ↑                                        |
              └──── EMERGENCY ◀── PA0 pressed at any time
```

## GPIO Outputs
| State | Red LED | Amber LED | Green LED |
|-------|---------|-----------|-----------|
| RED | ON | OFF | OFF |
| RED_AMBER | ON | ON | OFF |
| GREEN | OFF | OFF | ON |
| AMBER | OFF | ON | OFF |
| EMERGENCY | FLASH | OFF | OFF |

## CubeMX Configuration
| Pin | Signal | Notes |
|-----|--------|-------|
| PA5 | Red LED | External LED |
| PA6 | Amber LED | External LED |
| PA7 | Green LED | External LED |
| PA0 | EXTI0 emergency button | Rising edge, pull-down |
| PC13 | Status LED (onboard) | Active-LOW |
| USART1 | PA9/PA10, 115200 8N1 | State transitions |

## Timing
| State | Duration |
|-------|---------|
| RED | 5000ms |
| RED_AMBER | 1000ms |
| GREEN | 5000ms |
| AMBER | 2000ms |
| EMERGENCY | 3000ms then back to RED |
