# STM32-PROJECTS

A complete collection of 120 HAL-based STM32 projects across three boards, built using **native STM32 HAL + STM32CubeMX** — not Arduino-style STM32duino. Real embedded engineering: proper peripheral configuration via CubeMX, register-level awareness where it matters, and idiomatic C following current CubeMX `USER CODE BEGIN/END` conventions.

> **Verification note**: All code is written following STM32 HAL/CubeMX 6.16 conventions and manually reviewed. `arm-none-eabi-gcc` was not available in the authoring environment, so no automated compilation was run. Code is ready for you to generate the CubeMX project skeleton, paste in the USER CODE sections, build, and test on real hardware. See `STM32_SETUP.md` for the full workflow.

---

## Boards

### Capability Comparison

| Property | Blue Pill | Nucleo-F401RE | Black Pill |
|----------|-----------|---------------|------------|
| MCU | STM32F103C8T6 | STM32F401RETx | STM32F411CEU6 |
| Core | Cortex-M3 | Cortex-M4F | Cortex-M4F |
| Max Clock | 72 MHz | 84 MHz | 100 MHz |
| Flash | 64 KB | 512 KB | 512 KB |
| RAM | 20 KB | 96 KB | 128 KB |
| Onboard ST-Link | No | **Yes** (V2-1) | No |
| Native USB app | No (pull-up missing on clones) | Yes (USB OTG FS) | **Yes** (USB-C OTG FS) |
| CAN Bus | **Yes** (bxCAN) | No | No |
| I2S | No | Yes | Yes |
| FPU | No | Yes | Yes |
| Onboard LED | PC13 (active-LOW) | PA5 (active-HIGH, LD2) | PC13 (active-LOW) |
| Onboard Button | None | PC13 (B1, active-LOW) | BOOT0 btn + NRST |
| Flashing | External ST-Link or UART bootloader | Onboard ST-Link (plug & flash) | USB DFU or external ST-Link |
| Form Factor | Compact DIP | Arduino-shield headers | Compact DIP |
| Price (approx) | ~$2 clone | ~$12 official | ~$5 WeAct |

**Which board for what?**
- **Blue Pill**: Cheapest entry point; only board here with CAN bus; needs external programmer
- **Nucleo-F401RE**: Best for beginners — onboard ST-Link means plug-and-flash; Arduino shield headers; official ST quality
- **Black Pill**: Best everyday board — fastest clock, most RAM/Flash, native USB for CDC/DFU, compact; the sweet spot

---

## Why HAL + CubeMX (not Arduino-style STM32duino)

This repo deliberately uses **STM32 HAL + STM32CubeMX** rather than the Arduino-compatible STM32duino core:

- **Real peripheral configuration**: CubeMX generates pin-mux, clock, and DMA config correctly — no hidden abstractions
- **Production-representative code**: HAL is what you use in real embedded products; learning it transfers directly
- **Register-level transparency**: HAL is thin enough that you can see the register operations
- **CubeMX middleware**: FreeRTOS, FatFs, USB Device stack — all integrated correctly; Arduino-style makes this harder
- **Proper NVIC and DMA config**: CubeMX handles priority grouping, DMA channel/stream assignment per MCU

**Toolchain**: STM32Cube for VS Code (extension pack v3.0+, recommended) or STM32CubeIDE (Eclipse, more mature debugger). CubeMX v6.16+ is now **standalone** — install separately from the IDE. See `STM32_SETUP.md`.

---

## Repository Structure

```
STM32-PROJECTS\
├── README.md                        <- This file
├── STM32_SETUP.md                   <- Full toolchain setup + CubeMX workflow
│
├── BLUE_PILL\
│   ├── BASIC\
│   │   ├── 01_blink_led\
│   │   │   ├── README.md            <- What it does + exact CubeMX config
│   │   │   ├── main_usercode.c      <- HAL code for USER CODE BEGIN/END sections
│   │   │   └── wiring_notes.md      <- Pin diagram, parts list, 3.3V warnings
│   │   └── ... (02 through 30)
│   └── ADVANCED\
│       └── ... (01 through 10)
│
├── NUCLEO_F401RE\
│   ├── BASIC\     (01-30)
│   └── ADVANCED\  (01-10)
│
└── BLACK_PILL\
    ├── BASIC\     (01-30)
    └── ADVANCED\  (01-10)
```

Each project folder contains:
- `README.md` — description, exact CubeMX peripheral configuration, parts list, wiring, build/flash steps, expected behavior
- `main_usercode.c` — custom HAL C code organized by `USER CODE BEGIN/END` section label; paste each block into the matching section of CubeMX-generated `main.c`
- `wiring_notes.md` — physical pin connections, component ratings, **3.3V logic safety warnings**

---

## Blue Pill — Project List

### BASIC (30 projects)

| # | Project | Key Peripheral(s) |
|---|---------|-------------------|
| 01 | Blink LED (PC13 active-low) | GPIO Output |
| 02 | Push Button with debounce | GPIO Input (PA0) |
| 03 | PWM LED fade | TIM1 CH1 (PA8) |
| 04 | External interrupt (EXTI) | EXTI0 (PA0) |
| 05 | ADC potentiometer (polling) | ADC1 CH1 (PA1) |
| 06 | ADC with DMA | ADC1 + DMA1 CH1 |
| 07 | UART basics (TX/RX) | USART1 (PA9/PA10) |
| 08 | UART receive interrupt | USART1 RX IT |
| 09 | I2C bus scan | I2C1 (PB6/PB7) |
| 10 | 16x2 LCD via I2C backpack | I2C1 + PCF8574 |
| 11 | SSD1306 OLED via I2C | I2C1 (0x3C) |
| 12 | SPI loopback + device demo | SPI1 (PA5-7) |
| 13 | DHT22 temperature/humidity | Bit-bang + TIM3 µs timing |
| 14 | Independent Watchdog (IWDG) | IWDG |
| 15 | RTC basics (set/read time) | RTC + LSI |
| 16 | Sleep mode + EXTI wake | PWR + EXTI0 |
| 17 | Servo control (50Hz PWM) | TIM2 CH2 (PA1) |
| 18 | DC motor via H-bridge | TIM1 CH1 + GPIO direction |
| 19 | Relay module control | GPIO Output (PB5) |
| 20 | LED chase pattern (8 LEDs) | GPIO (PB8-PB15) |
| 21 | Timer input capture (HC-SR04) | TIM2 CH1 input capture |
| 22 | Hardware CRC32 unit | CRC peripheral |
| 23 | Flash read/write (config persist) | HAL Flash programming API |
| 24 | Window Watchdog (WWDG) | WWDG |
| 25 | Multi-peripheral integration | I2C1 + UART1 + TIM1 PWM |
| 26 | DMA UART TX | USART1 TX + DMA1 CH4 |
| 27 | State machine (button-driven) | EXTI0 + GPIO |
| 28 | Bootloader & BOOT0 guide | Reference/guide project |
| 29 | Power measurement guide | Reference/guide project |
| 30 | Capstone: sensor + OLED + watchdog | BMP280 + SSD1306 + IWDG |

### ADVANCED (10 projects)

| # | Project | Key Concept |
|---|---------|-------------|
| 01 | FreeRTOS basics (3 tasks) | RTOS task scheduling |
| 02 | FreeRTOS queues | Inter-task communication |
| 03 | FreeRTOS mutex (shared I2C bus) | Resource protection |
| 04 | PID controller (LDR feedback) | Closed-loop control |
| 05 | SD card + FatFs over SPI | Filesystem, data storage |
| 06 | Custom UART framed protocol | Reliable framed comms |
| 07 | Multi-sensor data acquisition | Precise 1kHz sampling + DMA |
| 08 | Bootloader concepts (dual-region) | Flash layout, application jump |
| 09 | Low-power sensor node | Stop mode + RTC wakeup |
| 10 | Capstone: FreeRTOS data logger | Full embedded product pattern |

---

## Nucleo-F401RE — Project List

### BASIC (30 projects)

| # | Project | Key Peripheral(s) |
|---|---------|-------------------|
| 01 | Blink LD2 (PA5 active-high) | GPIO Output |
| 02 | Push Button B1 (PC13) | GPIO Input Pull-up |
| 03 | PWM LED fade | TIM1 CH1 (PA8) |
| 04 | External interrupt (B1/EXTI13) | EXTI13 (PC13) |
| 05 | ADC potentiometer | ADC1 CH0 (PA0 / Arduino A0) |
| 06 | ADC with DMA | ADC1 + DMA2 Stream0 |
| 07 | UART via ST-Link VCP (no extra HW) | USART2 (PA2/PA3) |
| 08 | UART receive interrupt | USART2 RX IT |
| 09 | I2C bus scan | I2C1 (PB8/PB9 / Arduino D15/D14) |
| 10 | 16x2 LCD via I2C | I2C1 + PCF8574 |
| 11 | SSD1306 OLED via I2C | I2C1 (0x3C) |
| 12 | SPI loopback + device demo | SPI2 (PB13-15) |
| 13 | ST-Link VCP showcase | USART2, structured JSON output |
| 14 | Onboard B1 multi-mode (3 actions) | EXTI13 state machine |
| 15 | 84MHz timing demo | Calibrated NOP loop |
| 16 | 4 PWM channels simultaneously | TIM2 CH1-CH4 |
| 17 | Dual UART bridge (CAN substitute*) | USART1 + USART2 |
| 18 | I2S audio tone generation | SPI3/I2S3 + DMA |
| 19 | USB CDC application port | USB_OTG_FS + CDC middleware |
| 20 | Sleep/Stop/Standby comparison | PWR + RTC + EXTI |
| 21 | RTC backup registers | RTC + BKP_DRx |
| 22 | Hardware CRC32 | CRC peripheral |
| 23 | DMA SPI transfer | SPI2 + DMA1 Stream4 |
| 24 | Dual UART simultaneous | USART1 + USART2 RX IT |
| 25 | Servo + DC motor combined | TIM1 CH1 + TIM3 CH1 + L298N |
| 26 | Sensor fusion (BMP280+MPU6050) | I2C1 dual-sensor |
| 27 | State machine (traffic light) | GPIO outputs + EXTI |
| 28 | NVIC interrupt priorities demo | TIM2 + UART + EXTI preemption |
| 29 | Power measurement guide | IDD jumper method, reference |
| 30 | Capstone: connected sensor station | BMP280 + OLED + VCP + Stop mode |

*CAN bus is not available on STM32F401 — project 17 explains this and substitutes a dual-UART bridge.

### ADVANCED (10 projects)

| # | Project | Key Concept |
|---|---------|-------------|
| 01 | FreeRTOS basics (3 tasks) | RTOS scheduling |
| 02 | FreeRTOS queues (BMP280→display) | Inter-task communication |
| 03 | FreeRTOS mutex (shared I2C) | Resource protection |
| 04 | PID controller (encoder motor) | TIM3 encoder + TIM1 PWM |
| 05 | SD card + FatFs | SPI2 + FatFs middleware |
| 06 | Custom UART framed protocol | Framing + CRC8 + ACK |
| 07 | Multi-sensor data acquisition | 1kHz TIM + dual ADC + I2C |
| 08 | Bootloader concepts (dual-region) | Flash sectors, UART HEX receive |
| 09 | Low-power sensor node | Stop + RTC alarm, IDD measurement |
| 10 | Capstone: PC dashboard bridge | FreeRTOS + JSON + SD + commands |

---

## Black Pill — Project List

### BASIC (30 projects)

| # | Project | Key Peripheral(s) |
|---|---------|-------------------|
| 01 | Blink LED (PC13 active-low) | GPIO Output |
| 02 | Push Button with debounce | GPIO Input Pull-up (PA0) |
| 03 | PWM LED fade | TIM1 CH1 (PA8) |
| 04 | External interrupt | EXTI0 (PA0) |
| 05 | ADC potentiometer | ADC1 CH0 (PA0) |
| 06 | ADC with DMA | ADC1 + DMA2 Stream0 |
| 07 | UART basics | USART1 (PA9/PA10) |
| 08 | UART receive interrupt | USART1 RX IT |
| 09 | I2C bus scan | I2C1 (PB6/PB7) |
| 10 | 16x2 LCD via I2C | I2C1 + PCF8574 |
| 11 | SSD1306 OLED via I2C | I2C1 (0x3C) |
| 12 | SPI loopback + device demo | SPI1 (PA5-7) |
| 13 | USB DFU bootloader guide | Reference/guide project |
| 14 | 100MHz timing demo | Calibrated NOP loop |
| 15 | 4 PWM channels simultaneously | TIM2 CH1-CH4 |
| 16 | I2S audio tone generation | SPI3/I2S3 + DMA |
| 17 | USB CDC application port | USB_OTG_FS + CDC middleware |
| 18 | Sleep/Stop/Standby comparison | PWR + EXTI + RTC |
| 19 | RTC backup registers | RTC + BKP_DRx |
| 20 | Hardware CRC32 | CRC peripheral |
| 21 | DMA SPI transfer | SPI1 + DMA2 Stream3 |
| 22 | Flash read/write (sector erase) | HAL Flash sector erase API |
| 23 | Independent Watchdog (IWDG) | IWDG |
| 24 | Multi-peripheral integration | BMP280 + OLED + UART + PWM |
| 25 | NVIC interrupt priorities demo | TIM2 + UART + EXTI preemption |
| 26 | Sensor fusion (BMP280+MPU6050) | I2C1 dual-sensor |
| 27 | State machine (traffic light) | GPIO outputs + EXTI |
| 28 | Servo + DC motor combined | TIM1 CH1 + TIM3 CH1 + L298N |
| 29 | Power measurement guide | USB CDC complication notes |
| 30 | Capstone: portable sensor logger | BMP280 + OLED + USB CDC + Flash + Stop |

### ADVANCED (10 projects)

| # | Project | Key Concept |
|---|---------|-------------|
| 01 | FreeRTOS basics (3 tasks) | RTOS scheduling |
| 02 | FreeRTOS queues (BMP280→display) | Inter-task communication |
| 03 | FreeRTOS mutex (shared I2C) | Resource protection |
| 04 | PID controller (encoder motor) | TIM3 encoder + TIM1 PWM |
| 05 | SD card + FatFs | SPI2 + FatFs middleware |
| 06 | Custom UART framed protocol | Framing + CRC8 + ACK |
| 07 | Multi-sensor data acquisition | 1kHz sampling + USB CDC output |
| 08 | Bootloader concepts (dual-region) | Flash sectors, UART HEX receive |
| 09 | Low-power sensor node | Standby + RTC alarm |
| 10 | Capstone: USB sensor device | FreeRTOS + USB CDC + SD + commands |

---

## Safety — Read This First

### 3.3V Logic Levels

**All three boards operate at 3.3V logic.** This is the single most important safety fact in this repo.

| Situation | Result |
|-----------|--------|
| Apply 5V signal to any GPIO pin | Permanent MCU damage |
| Power a 5V-only sensor from 3.3V rail | Sensor may not work / wrong readings |
| Use a 3.3V-compatible module | Fine — most modern I2C/SPI sensors support both |
| Connect a 5V sensor | Use a BSS138-based bidirectional level shifter |

**Commonly safe at 3.3V**: BMP280, BME280, MPU6050, SSD1306 OLED, PCF8574 LCD backpacks, MAX7219, PCM5102A. Verify your specific module's datasheet.

**Needs level shifting**: HC-SR04 echo pin (if using 5V version), some older LCD modules.

### Common Mistakes

1. **Blue Pill LED polarity**: `HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET)` = LED **ON** (active-low)
2. **BOOT0 left at 1 on Blue Pill**: Board enters bootloader instead of running firmware after reset
3. **F4 boards have no CAN**: Use Blue Pill (F103) for CAN bus projects; F401/F411 do not have CAN
4. **CubeMX is now standalone**: As of November 2025, CubeMX is not bundled inside STM32CubeIDE — install both separately
5. **PA5 on Nucleo is both LD2 LED and SPI1 SCK**: Use SPI2 for SPI projects on Nucleo
6. **Black Pill PA10 floating**: WeAct board — can cause accidental UART ISP entry; add 10kΩ pull-up to 3.3V on PA10

---

## Parts List Summary

### Common to all boards

| Component | Qty | Notes |
|-----------|-----|-------|
| BMP280 breakout | 1 | I2C 0x76/0x77, 3.3V |
| MPU6050 breakout | 1 | I2C 0x68/0x69, 3.3V |
| SSD1306 OLED 128x64 | 1 | I2C 0x3C, 3.3V |
| 16x2 LCD + PCF8574 I2C backpack | 1 | I2C 0x27/0x3F |
| DHT22 sensor | 1 | Single-wire, 3.3V |
| HC-SR04 ultrasonic | 1 | 5V; echo needs level shift |
| L298N H-bridge module | 1 | 5-12V motor supply |
| SG90 servo | 1 | 5V supply, 3.3V signal |
| SPI SD card module | 1 | 3.3V compatible module |
| PCM5102A I2S DAC | 1 | For I2S audio projects |
| 10kΩ potentiometer | 2 | ADC input |
| Tactile pushbuttons | 5 | |
| 5mm LEDs (assorted) + 330Ω resistors | 10 each | |
| Jumper wires | 1 set | |
| Breadboard | 1 | |

### Blue Pill specific

| Component | Purpose |
|-----------|---------|
| ST-Link V2 clone | SWD programming/debugging (required) |
| CP2102 or CH340 USB-serial (3.3V) | Alternative: UART bootloader flashing |

### Nucleo-F401RE specific

| Component | Purpose |
|-----------|---------|
| Micro-B or Mini-B USB cable | ST-Link connection — nothing else needed |

### Black Pill specific

| Component | Purpose |
|-----------|---------|
| USB-C cable | DFU flashing + USB CDC application |
| 10kΩ resistor | PA10 pull-up (prevents accidental UART ISP) |

---

## Toolchain Setup

See **[STM32_SETUP.md](STM32_SETUP.md)** for complete instructions:
- IDE choice: STM32Cube for VS Code vs STM32CubeIDE (pros/cons, recommendation)
- CubeMX v6.16 workflow: create project → configure peripherals → generate code → paste USER CODE
- ST-Link driver installation (STSW-LINK009, STM32CubeProgrammer)
- Per-board flashing: Blue Pill UART/SWD, Nucleo ST-Link, Black Pill DFU
- FreeRTOS setup via CubeMX CMSIS-V2
- Safety notes and common mistakes

## How to Use a Project

1. Open **STM32CubeMX** (v6.16+, standalone)
2. Select MCU or board per the project's `README.md` CubeMX Configuration section
3. Configure each peripheral exactly as listed
4. Generate Code (STM32CubeIDE toolchain)
5. Open `Core/Src/main.c` in your IDE
6. Copy each labeled block from `main_usercode.c` into the matching `/* USER CODE BEGIN xxx */` section
7. Build (`Ctrl+B`)
8. Flash per the project README

---

*120 projects: 3 boards × (30 BASIC + 10 ADVANCED). All code follows STM32 HAL/CubeMX 6.16 conventions. Manually reviewed — not compiled or hardware-tested. Ready to generate, paste, build, and test.*
