# Nucleo-F401RE BASIC/15 — 84 MHz Clock Timing Demo

## Overview
Demonstrates the F401RE's 84 MHz clock speed by bit-banging a 1 MHz square wave on PB0 using NOP loops, then printing the measured tick rate via UART2. Contrasting with Blue Pill (72 MHz) using the same loop produces a measurably different output frequency.

## CubeMX Configuration

| Peripheral | Setting |
|-----------|---------|
| Clock | SYSCLK = 84 MHz (HSI × PLL or HSE 8 MHz × 21/2) |
| GPIO PB0 | Output Push-Pull, Very High speed |
| USART2 | Async, 115200 8N1, PA2/PA3 |

**Clock tree**: In CubeMX Clock Configuration tab, set SYSCLK to 84 MHz via PLL. Set AHB = 84 MHz, APB1 = 42 MHz, APB2 = 84 MHz.

## Parts List
| Component | Qty | Notes |
|-----------|-----|-------|
| Nucleo-F401RE | 1 | |
| Logic analyzer or oscilloscope | 1 | To verify 1 MHz output on PB0 |

## Expected Behavior
- PB0 toggles at ~1 MHz (500 kHz per half-cycle, verified with scope)
- UART2 prints: `SYSCLK=84MHz | NOP loop 1MHz on PB0 | Measure PB0 with scope`
- Blue Pill at same loop count produces ~857 kHz (72/84 ratio)
