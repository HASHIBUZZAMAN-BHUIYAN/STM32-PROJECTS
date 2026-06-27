# 08 — Bootloader Concepts

## Overview
Demonstrates dual-region flash on the STM32F103C8T6. A minimal bootloader occupies the first 4 KB of flash (pages 0–3, base address `0x08000000`). The user application is linked to start at `0x08001000`. At boot, the bootloader checks a button on PB0: if held, it enters a UART command-waiting loop; otherwise it jumps to the application.

## Memory Map
```
Flash (64 KB total — 0x08000000 to 0x0800FFFF)
┌──────────────────────────────────────────┐
│  0x08000000 – 0x08000FFF  (4 KB, pages 0–3) │  Bootloader
│  0x08001000 – 0x0800FFFF  (60 KB, pages 4–63)│  Application
└──────────────────────────────────────────┘

STM32F103C8T6 flash page size: 1 KB
```

## Boot Flow
```
Power on / Reset
      │
      ▼
 Bootloader starts (0x08000000)
      │
      ▼
 Read PB0 (pulled up)
      │
      ├── PB0 LOW (button pressed) ──► Stay in bootloader
      │                                Wait for UART command
      │                                (placeholder — extend with XMODEM etc.)
      │
      └── PB0 HIGH (released) ──► Validate app vector table
                                       │
                                       ├── Stack pointer check OK ──► Jump to app
                                       └── Invalid SP ──► Stay in bootloader
```

## Application Relocation
The application **must** be built with a modified linker script so `.isr_vector` and all code are placed starting at `0x08001000`.

### STM32F103C8Tx_FLASH.ld changes for the application project:
```ld
MEMORY
{
  RAM    (xrw) : ORIGIN = 0x20000000, LENGTH = 20K
  FLASH  (rx)  : ORIGIN = 0x08001000, LENGTH = 60K   /* <-- changed */
}
```

### application main.c — relocate vector table:
```c
/* Add before MX_xxx_Init() calls, in USER CODE BEGIN 2 */
SCB->VTOR = 0x08001000;
```

## Jump Mechanism
The bootloader reads the application's stack pointer from `0x08001000` and reset handler address from `0x08001004`, then performs the jump:
```c
uint32_t app_sp  = *(volatile uint32_t *)0x08001000;
uint32_t app_pc  = *(volatile uint32_t *)0x08001004;
// Set stack pointer
__set_MSP(app_sp);
// Cast and call reset handler
void (*app_reset)(void) = (void (*)(void))app_pc;
app_reset();
```

## CubeMX Configuration

### Bootloader Project
| Peripheral | Setting                                              |
|------------|------------------------------------------------------|
| SYS        | No RTOS, SW debug (SWD), no RTOS timer              |
| USART1     | 115200 8N1, polling TX only                          |
| GPIO       | PB0 as Input, Pull-up                               |
| FLASH      | Origin 0x08000000, Length 4K (set in linker script) |

### Application Project
| Peripheral | Setting                                       |
|------------|-----------------------------------------------|
| SYS        | SWD, no RTOS                                  |
| GPIO       | PC13 as Output (LED blink)                    |
| FLASH      | Origin 0x08001000, Length 60K (linker script) |

## Flashing Both Images
1. Flash bootloader at `0x08000000` (standard CubeIDE / OpenOCD default).
2. Flash application at `0x08001000`:
   - STM32CubeProgrammer: set "Start address" to `0x08001000` before flashing.
   - OpenOCD: `flash write_image erase app.bin 0x08001000`

## Files
| File              | Purpose                                        |
|-------------------|------------------------------------------------|
| `main_usercode.c` | Bootloader code + application blink code       |
| `wiring_notes.md` | Hardware connection guide                      |

## Not Tested
This code has not been compiled or run on hardware. It is provided as a reference implementation for study and adaptation.
