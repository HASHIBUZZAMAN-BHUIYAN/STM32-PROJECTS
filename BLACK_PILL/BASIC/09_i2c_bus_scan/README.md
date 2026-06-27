# Project 09 — I2C Bus Scanner

## Board
**STM32F411CEU6 Black Pill** (WeAct Studio)
- 100 MHz Cortex-M4F, 512 KB Flash, 128 KB RAM
- Onboard LED: PC13 (active-low)
- No onboard ST-Link — DFU or external ST-Link required

## Project Description
Scans the I2C1 bus for all 7-bit device addresses (0x01 to 0x7F).
For each address, calls `HAL_I2C_IsDeviceReady()` with 3 trials and a 10 ms timeout.
Detected addresses are printed over UART1 as hex strings.
After the full scan a "Scan complete" message is printed.
The scan repeats automatically every 5 seconds.

Useful for quickly identifying which I2C peripherals are connected and confirming their addresses before writing a dedicated driver.

## Hardware Required
- STM32F411 Black Pill
- One or more I2C devices (e.g., PCF8574 LCD backpack, SSD1306 OLED, MPU-6050, BMP280)
- 4.7 kΩ pull-up resistors on SDA and SCL (if not already on breakout board)
- USB-to-UART adapter (e.g., CP2102, CH340) for UART1 monitoring

## Pin Assignments
| Signal | Black Pill Pin | Notes                        |
|--------|---------------|------------------------------|
| SCL    | PB6           | I2C1 SCL — add 4.7kΩ to 3.3V|
| SDA    | PB7           | I2C1 SDA — add 4.7kΩ to 3.3V|
| TX     | PA9           | USART1 TX → adapter RX       |
| RX     | PA10          | USART1 RX → adapter TX       |
| GND    | GND           | Common ground                |
| 3.3V   | 3V3           | Power for 3.3V devices only  |

## CubeMX Configuration

### Clock
- Input: HSE 25 MHz crystal
- PLL: PLLM=25, PLLN=200, PLLP=2 → SYSCLK = 100 MHz
- APB1 Prescaler: /2 → 50 MHz (I2C clock source)
- APB2 Prescaler: /1 → 100 MHz (USART1 source)

### I2C1
- Mode: I2C
- Speed Mode: Standard Mode
- Clock Speed: 100000 Hz (100 kHz)
- Pins: PB6 = I2C1_SCL, PB7 = I2C1_SDA
- Leave all other parameters at default

### USART1
- Mode: Asynchronous
- Baud Rate: 115200
- Word Length: 8 bits
- Parity: None
- Stop Bits: 1
- Pins: PA9 = USART1_TX, PA10 = USART1_RX

### GPIO
- PC13: GPIO Output (onboard LED, optional use)

### NVIC
- No interrupts required for this project (polling mode)

## How to Flash (USB DFU)
1. Hold the **BOOT0** button on the board
2. While holding BOOT0, briefly press and release **NRST**
3. Wait approximately 0.5 seconds, then release BOOT0
4. The board enumerates as **"STM32 BOOTLOADER"** on your PC's USB
5. Open **STM32CubeProgrammer**, select **USB** connection, click **Connect**
6. Go to **Erasing & Programming**, browse to your `.elf` or `.hex` file
7. Click **Start Programming**
8. After programming completes, press **NRST** to exit DFU and run firmware

**Alternative:** External ST-Link V2 via SWD
- PA13 = SWDIO, PA14 = SWCLK, GND, 3.3V

## Serial Monitor
Open a serial terminal at **115200 8N1** on the COM port of your USB-UART adapter.

Expected output (with a PCF8574 at 0x27 and SSD1306 at 0x3C connected):
```
I2C Bus Scanner — STM32F411 Black Pill
Scanning addresses 0x01 to 0x7F...
  Found device at address: 0x27
  Found device at address: 0x3C
Scan complete. 2 device(s) found.
--- Next scan in 5 seconds ---
```

## Notes
- I2C addresses 0x00 (general call) and 0x78-0x7F (reserved) are skipped
- Pull-up resistors are mandatory; without them SDA/SCL will float and all probes fail
- Some devices take a moment to power up; wait before scanning after power-on
- If no devices found, check pull-ups, power supply, and wiring continuity
