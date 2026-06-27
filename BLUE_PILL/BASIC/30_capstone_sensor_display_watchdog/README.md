# 30 — Capstone: BMP280 + SSD1306 OLED + IWDG Watchdog

## Overview

This capstone project integrates three key subsystems:

1. **BMP280** — I2C temperature and pressure sensor (I2C1: PB6 SCL, PB7 SDA)
2. **SSD1306 128×64 OLED** — I2C display showing live readings (same I2C1 bus)
3. **IWDG** — Independent Watchdog that resets the board if the sensor fails 3 consecutive times

### Normal Operation (each 1-second cycle)

```
Read BMP280 → Format values → Update OLED → Refresh IWDG → Wait remainder of 1s
```

### Error Recovery Logic

```
If sensor read fails:
    consecutive_failures++
    Display "SENSOR ERROR" on OLED
    If consecutive_failures >= 3:
        Stop refreshing IWDG → IWDG resets board
    Else:
        Continue refreshing IWDG (board stays alive)
On successful read:
    consecutive_failures = 0
```

---

## MCU

STM32F103C8T6 (Blue Pill), 72 MHz

---

## Hardware Required

| Component | Description |
|---|---|
| BMP280 module | I2C variant (SDO to GND → address 0x76; SDO to VCC → 0x77) |
| SSD1306 OLED | 128×64, I2C variant (address 0x3C or 0x3D) |
| Blue Pill | STM32F103C8T6 |
| 4.7 kΩ resistors × 2 | I2C pull-ups (if not already on modules) |

---

## CubeMX Configuration

| Peripheral | Setting |
|---|---|
| I2C1 | PB6 = SCL, PB7 = SDA, Standard mode 100 kHz |
| IWDG | Prescaler: /32, Reload: 3124 → ~3.2 s timeout at 40 kHz LSI |
| GPIO PC13 | Output (onboard LED for status blink) |
| Clock | HSE 8 MHz → PLL → 72 MHz |
| SysTick | For HAL_Delay and HAL_GetTick |

### IWDG Timeout Calculation

```
IWDG clock source: LSI ~40 kHz (±30% on F103)
Timeout = (Prescaler × (Reload + 1)) / LSI_freq
        = (32 × 3125) / 40000
        = 2.5 s nominal
```

The main loop runs every 1 second. With a 2.5 s watchdog, one missed refresh = reset. Setting 3 consecutive failures means the 3rd failure does NOT refresh → reset occurs within the next 2.5 s after the 3rd failure.

Adjust Reload value in CubeMX to tune timeout. Use a value comfortably longer than 1 full main-loop cycle (1 s here).

---

## External Libraries Required

This project requires two driver libraries not included in CubeMX:

### BMP280

- **Bosch BMP2-Sensor-API** (official): https://github.com/boschsensortec/BMP2-Sensor-API
- Alternatively, a lightweight bare HAL version: implement `bmp280_read_temp_pressure()` using `HAL_I2C_Mem_Read` targeting registers 0xF7–0xFC (raw ADC burst read).
- The `main_usercode.c` shows the HAL I2C register-level approach for portability.

### SSD1306

- **stm32-ssd1306** by afiskon (MIT license): https://github.com/afiskon/stm32-ssd1306
- Copy `ssd1306.c`, `ssd1306.h`, `ssd1306_fonts.c`, `ssd1306_fonts.h` into your project's `Src/` and `Inc/` folders.
- This library uses `hi2c1` directly. Ensure `extern I2C_HandleTypeDef hi2c1;` is accessible.

---

## Expected OLED Output

```
+---------------------------+
| Temp:  24.5 C             |
| Press: 1013.2 hPa         |
+---------------------------+
```

On sensor error:
```
+---------------------------+
| SENSOR ERROR              |
| Fails: 2/3                |
+---------------------------+
```

---

## Watchdog Behavior

- Every successful sensor read calls `HAL_IWDG_Refresh(&hiwdg)`.
- On 3rd consecutive failure, the refresh is skipped. The IWDG times out and issues a system reset.
- After reset, the board reinitializes and attempts sensor communication again.
- This is intentional error recovery — no manual intervention needed for transient sensor faults (e.g., wiring disturbance, I2C glitch).

---

## Files

| File | Purpose |
|---|---|
| `main_usercode.c` | BMP280 raw I2C reads, SSD1306 display, IWDG logic |
| `wiring_notes.md` | I2C wiring, pull-up requirements, voltage notes |

## Note

This code has not been tested on hardware. The BMP280 compensation formulas shown are simplified integer approximations suitable for demonstration — use the official Bosch compensation code for calibrated, accurate readings.
