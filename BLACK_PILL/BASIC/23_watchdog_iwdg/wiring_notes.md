# Wiring Notes — Black Pill BASIC/23: IWDG Watchdog

## No Extra Hardware Required
Uses USART1 via USB-UART adapter.

## USART1 Connection
| Black Pill | USB-UART Adapter |
|-----------|----------------|
| PA9 (TX) | RX |
| PA10 (RX) | TX |
| GND | GND |

## IWDG Timeout Calculation
At LSI ≈ 32 kHz:
- Prescaler 4 → 4/32000 = 0.125ms/tick
- Prescaler 128 → 128/32000 = 4ms/tick
- Reload 500 with Prescaler 128 → 500 × 4ms = 2000ms = 2s timeout

Exact LSI frequency varies ±15%, so actual timeout may be 1.7s–2.3s. For critical timing, use WWDG clocked from PCLK1.

## CubeMX IWDG Setup
1. Watchdog → IWDG → Activate
2. Prescaler = 128
3. Reload Value = 500
4. Counter Window Value = 0 (no window — any time kick is valid)

## Testing
1. Open terminal at 115200
2. See "WDG kick #N" every second
3. Type `hang` + Enter
4. After ~2 seconds, see "*** Woke from IWDG RESET ***"
5. System resumes normal operation

## IWDG Cannot Be Disabled
Once IWDG is started (HAL_IWDG_Init), it cannot be stopped or disabled except by MCU reset. Keep this in mind during debugging — the debugger pausing execution will cause IWDG to fire. In CubeIDE, enable "Freeze IWDG when core halted" in Debug Configuration → Debugger options.

## Parts List
| Component | Qty |
|-----------|-----|
| Black Pill (STM32F411CEU6) | 1 |
| USB-UART adapter | 1 |
| USB-C cable | 1 |
| Jumper wires | 3 |
