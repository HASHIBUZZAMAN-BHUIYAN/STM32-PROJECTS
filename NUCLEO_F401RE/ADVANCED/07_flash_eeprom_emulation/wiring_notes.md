# Wiring Notes — Nucleo-F401RE ADVANCED/07: Flash EEPROM Emulation

## No Extra Hardware Required
Uses onboard Nucleo only: LD2 (PA5), USART2 via ST-Link VCP.

## Linker Script Warning
By default, the STM32 linker places all code and data starting at 0x08000000. The firmware must not accidentally use Sector 7 (0x08060000). Check your `.ld` file:

```ld
FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 384K  /* leave last 128K for user data */
```

The STM32F401RE has 512KB flash total. Reserving the last 128KB (Sector 7) for data leaves 384KB for code.

**To change the linker script in CubeIDE:**
- Project → Properties → C/C++ Build → Settings → Linker → Linker Script
- Edit `STM32F401RETx_FLASH.ld`: change `LENGTH = 512K` to `LENGTH = 384K`

## FLASH_VOLTAGE_RANGE_3
`FLASH_VOLTAGE_RANGE_3` means VDD = 2.7–3.6V, which enables 32-bit parallelism during programming (fastest). Nucleo operates at 3.3V so this is correct.

## Important Constraints
1. **Cannot write to flash while executing from flash** (unless using Flash Cache) — this is generally fine on Cortex-M4 since the flash controller handles this
2. **Must erase whole sector before writing** — cannot flip bits from 0 → 1 without erasing; erasing sets all bits to 1
3. **Word-aligned writes** — `HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD)` requires 4-byte aligned address
4. **Do not write from interrupt** — flash programming disables all interrupts internally

## Testing
1. Flash firmware
2. Open terminal at 115200
3. "First boot" message, then "Saved: boot_count=1"
4. Press NRST (reset button)
5. "Loaded: boot_count=1" then "Saved: boot_count=2"
6. Repeat to verify persistence

## Parts List
| Component | Qty |
|-----------|-----|
| Nucleo-F401RE | 1 |
| USB cable | 1 |
