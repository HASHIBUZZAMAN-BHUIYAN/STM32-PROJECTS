# Black Pill ADVANCED/10 — Capstone: USB Sensor Device

## Overview
Full USB CDC virtual COM device exposing multi-sensor data. Host connects via USB (no UART adapter needed — USB-C directly on Black Pill), sends commands, receives JSON sensor streams. FreeRTOS manages four tasks. The Black Pill capstone exploits every unique feature of the board: 100 MHz FPU, direct USB, SPI1 free, 128KB RAM.

## Unique to Black Pill
- USB-C directly on PA11/PA12 — no extra hardware, one cable from PC to device
- PLLQ must divide to exactly 48 MHz for USB: HSE 25 MHz, PLLM=25, PLLN=192, PLLP=4 (100 MHz), PLLQ=4 (48 MHz)
- SPI1 free for W25Q32 flash logging
- FPU: MPU6050 pitch/roll computed every 50ms without float overhead

## System Architecture
```
USB CDC (PA11/PA12)
    ↕
CmdTask (Normal) ──→ SensorTask (High) → DataQueue → LogTask (Normal)
                                         ↓
                                    DisplayTask (Low) → SSD1306 OLED
```

## Task Roles
| Task | Priority | Function |
|------|----------|----------|
| SensorTask | High | Read BMP280+MPU6050 every 50ms, queue data |
| LogTask | Normal | Drain queue, write to W25Q32 flash, send USB JSON |
| DisplayTask | Low | Update SSD1306 OLED every 500ms |
| CmdTask | Normal | Process USB CDC commands |

## USB CDC Commands
| Command | Response |
|---------|----------|
| `stream on` | Start JSON stream at 1 Hz |
| `stream off` | Stop stream |
| `read` | Single sensor snapshot |
| `log dump` | Dump last 20 flash records |
| `log erase` | Erase flash log |
| `status` | Task stack HWM, heap free, uptime |
| `reboot` | NVIC_SystemReset() |

## JSON Format
```json
{"tick":5000,"bmp_t":24.1,"bmp_p":101325,"ax":-0.02,"ay":0.98,"az":0.12,"pitch":1.1,"roll":-2.3}
```

## CubeMX
- USB_OTG_FS Device Only + CDC class
- FreeRTOS CMSIS-V2, TOTAL_HEAP_SIZE=16384 (16KB — 128KB RAM allows it)
- TIM11 timebase
- I2C1 PB6/PB7, SPI1 PA5/PA6/PA7, USART1 PA9/PA10 (fallback debug)
