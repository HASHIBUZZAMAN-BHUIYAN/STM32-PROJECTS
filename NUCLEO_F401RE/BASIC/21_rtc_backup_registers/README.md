# Nucleo-F401RE BASIC/21 — RTC Backup Registers

## Overview
Uses RTC backup registers (BKP_DR1..BKP_DR20 on F401) to persist a boot counter across resets without reinitializing the RTC clock. On first boot: initializes RTC to 12:00:00. On every subsequent reset (including Standby wakeup): reads and increments the counter stored in BKP_DR1 without touching the RTC time.

## CubeMX Configuration

| Peripheral | Setting |
|-----------|---------|
| RTC | Activate Clock Source: LSI; Calendar enabled |
| PWR | Enable Backup domain access in USER CODE |
| USART2 | 115200 8N1, PA2/PA3 |

**LSI vs LSE**: Nucleo-F401RE does not have an external 32.768 kHz crystal (LSE) by default. Use LSI (~32 kHz internal oscillator). LSI is less accurate but sufficient for demo purposes.

## Parts List
| Component | Qty |
|-----------|-----|
| Nucleo-F401RE | 1 |
| USB cable | 1 |

## Expected Behavior
Every 5 seconds via UART:
```
Boot count: 3 | Time: 12:00:15
Boot count: 3 | Time: 12:00:20
```
Press reset → boot count increments to 4, time continues from where it stopped.
```
Boot count: 4 | Time: 12:00:23
```
