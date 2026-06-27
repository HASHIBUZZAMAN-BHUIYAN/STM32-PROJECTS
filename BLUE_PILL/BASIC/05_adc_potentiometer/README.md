# 05 — ADC Potentiometer Read (ADC1 CH1, PA1)

## Overview
Reads an analog voltage from a potentiometer wiper on **PA1** (ADC1 Channel 1) in polling mode, scales the 12-bit result (0–4095) to a percentage (0–100 %), and transmits the value over **UART1** (PA9 TX, PA10 RX) at 115200 baud.

## Hardware
- Board: STM32F103C8T6 Blue Pill
- Potentiometer: 10 kΩ (wiper to PA1, ends to 3.3 V and GND)
- UART adapter: USB-to-Serial (3.3 V logic) on PA9/PA10

## CubeMX Configuration
| Peripheral | Setting |
|------------|---------|
| ADC1 IN1 | Enabled (PA1), Single Conversion Mode |
| ADC1 Resolution | 12-bit (default) |
| ADC1 Data Alignment | Right |
| ADC1 Continuous Conv. | Disabled (polling) |
| ADC1 Scan Mode | Disabled |
| USART1 | Asynchronous, 115200 baud, 8N1 |
| SYS | Debug: Serial Wire |

No DMA is required. The firmware calls `HAL_ADC_Start`, `HAL_ADC_PollForConversion`, and `HAL_ADC_GetValue` in the main loop.

## How It Works
1. `HAL_ADCEx_Calibration_Run` calibrates the ADC at startup (recommended for accuracy on STM32F1).
2. The main loop starts a conversion, waits for it (100 ms timeout), reads the 12-bit value, scales it, then formats and sends a string via UART.
3. A 500 ms delay between readings keeps the output readable.

### Scaling Formula
```
percent = (adc_value * 100U) / 4095U
```

## Build & Flash
1. Create a new STM32CubeIDE project for STM32F103C8T6.
2. Apply CubeMX settings above and generate code.
3. Copy sections from `main_usercode.c` into `Core/Src/main.c`.
4. Build and flash via ST-Link.
5. Open a serial terminal (115200 8N1) on the USB-to-Serial adapter's COM port.

## Notes
- Code is **not tested on hardware**. Review before flashing.
- PA0 is ADC1 CH0; PA1 is ADC1 CH1. These are different pins — ensure you use PA1.
- `hadc1` and `huart1` are declared by the CubeMX-generated code.
- The `sprintf` function requires that the project links with the C standard library (default in STM32CubeIDE).
