# Black Pill BASIC/24 — Multi-Peripheral Integration

## Overview
Integrates BMP280 (I2C), SSD1306 OLED (I2C shared bus), ADC (PA1), and USART1 simultaneously. Demonstrates managing multiple peripherals from a single main loop with non-blocking timing. Serves as a stepping stone toward the FreeRTOS capstone projects.

## Active Peripherals
| Peripheral | Pins | Rate |
|-----------|------|------|
| BMP280 (I2C1) | PB6/PB7 | Every 2s |
| SSD1306 (I2C1) | PB6/PB7 (shared) | Every 200ms |
| ADC1 IN1 | PA1 | Every 500ms |
| USART1 | PA9/PA10 | On-demand |
| PC13 LED | PC13 | Every 1s |

## I2C Bus Sharing
Both BMP280 (0x76) and SSD1306 (0x3C) share I2C1 (PB6/PB7). HAL_I2C functions are not interrupt-safe to call concurrently — use only one at a time from the main loop (no ISR I2C access in this project).

## Non-Blocking Timing Pattern
```c
uint32_t last_bmp = 0, last_oled = 0, last_adc = 0;
// In main loop:
if (HAL_GetTick() - last_bmp >= 2000) {
    last_bmp = HAL_GetTick();
    bmp280_read(); // blocking I2C read is OK in main loop
}
if (HAL_GetTick() - last_oled >= 200) {
    last_oled = HAL_GetTick();
    oled_update(); // brief I2C write
}
// No HAL_Delay() in main loop!
```

## Expected Behavior
- OLED shows BMP280 temperature/pressure + ADC value, updated every 200ms
- USART1 prints full readings every 5 seconds
- PC13 heartbeat blinks every 1 second

## CubeMX Configuration
| Peripheral | Setting |
|-----------|---------|
| I2C1 | PB6/PB7, Fast Mode 400 kHz |
| ADC1 | PA1 IN1, Single, Polling |
| USART1 | PA9/PA10, 115200 8N1 |
