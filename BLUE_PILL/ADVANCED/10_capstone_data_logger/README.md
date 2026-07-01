# Blue Pill ADVANCED/10 — Capstone: FreeRTOS Data Logger

## Overview
A complete embedded data logger demonstrating a realistic small product architecture on the Blue Pill. Four FreeRTOS tasks (sensor, SD card logger, OLED display, UART command interface) run concurrently, protected by a mutex on the shared I2C bus. An IWDG watchdog ensures recovery from hangs.

## What It Demonstrates
- FreeRTOS multi-task architecture (synthesis of ADVANCED/01-03)
- SPI SD card with FatFs (ADVANCED/05)
- UART command parser (ADVANCED/06)
- Shared resource protection with mutex (ADVANCED/03)
- IWDG watchdog in an RTOS context

## CubeMX Configuration

### Part: STM32F103C8T6

### Peripherals
| Peripheral | Config | Pins |
|-----------|--------|------|
| I2C1 | Standard mode, 100kHz | PB6 (SCL), PB7 (SDA) |
| SPI2 | Full-Duplex Master, 8-bit, CPOL=0 CPHA=0, 1MHz | PB13 (SCK), PB14 (MISO), PB15 (MOSI) |
| USART1 | Async, 115200 8N1 | PA9 (TX), PA10 (RX) |
| TIM4 | Up counter, no output | Internal timebase for FreeRTOS |
| IWDG | PSC=/32, Reload=3124 → ~3s timeout | |
| GPIO | PB12 Output (SD CS), PC13 Output (LED) | |

### Middleware
| Middleware | Config |
|-----------|--------|
| FreeRTOS | CMSIS_V2, Heap=8192 |
| FATFS | SPI user driver |

### Clock
SYSCLK = 72 MHz via PLL (HSE 8MHz × 9)

### NVIC
- USART1 global interrupt: enabled, priority 5
- TIM4 global: enabled (FreeRTOS tick)

## Parts List
| Component | Qty |
|-----------|-----|
| Blue Pill (STM32F103C8T6) | 1 |
| BMP280 breakout (I2C) | 1 |
| SSD1306 OLED 128x64 (I2C) | 1 |
| SPI SD card module | 1 |
| 4.7kΩ resistors | 2 (I2C pull-ups) |
| USB-serial adapter (3.3V) | 1 |
| ST-Link V2 | 1 |

## Wiring Summary
- BMP280 / SSD1306: PB6 (SCL), PB7 (SDA), 4.7kΩ pull-ups to 3.3V
- SD card: PB13/PB14/PB15 (SPI2), PB12 (CS)
- UART: PA9 (TX→RX of adapter), PA10 (RX←TX of adapter)

## Build & Flash
1. Generate CubeMX project with above config
2. Paste main_usercode.c blocks into main.c USER CODE sections
3. Build (Ctrl+B)
4. Flash via ST-Link SWD

## UART Commands (115200 baud)
| Command | Action |
|---------|--------|
| `start` | Begin logging to SD "LOG.TXT" |
| `stop` | Pause logging |
| `read` | Print last 5 log entries |
| `erase` | Delete LOG.TXT |
| `status` | Print task states, log count |

## Expected Behavior
- OLED shows: current temp/pressure (top), log count (bottom), "REC" when logging active
- UART streams task heartbeats; accepts commands
- SD card LOG.TXT grows with CSV entries: `tick_ms,temp_cdeg,pressure_pa`
- IWDG resets the board if any task hangs >3s (each task refreshes watchdog flag; a watchdog task consolidates)
- PC13 LED blinks every 1s (LedTask heartbeat)
