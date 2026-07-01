# Black Pill ADVANCED/08 — Bootloader Concepts

## Overview
Explores the Black Pill's ROM bootloader, a custom in-application flash update mechanism via UART XMODEM, and a minimal application-jump trampoline. Teaches the three-layer boot flow that real products use.

## Black Pill Boot Modes
| BOOT0 | BOOT1 | Mode |
|-------|-------|------|
| 0 | X | Boot from Flash (normal) |
| 1 | 0 | System ROM bootloader (DFU / UART) |
| 1 | 1 | Boot from SRAM |

## Three Project Concepts
### A) ROM DFU (covered in BASIC/13)
Hold BOOT0, reset → STM32CubeProgrammer USB mode.

### B) UART Bootloader via USART1
System ROM bootloader also speaks USART1 at 115200 when in boot mode.
`stm32flash -w firmware.bin -v -R /dev/ttyUSB0` triggers it.

### C) In-Application Programmer (this firmware)
- Application receives new firmware over USART1 (XMODEM-CRC)
- Writes it to an alternate flash sector (Sector 4 @ 0x08010000)
- Validates CRC32 over received image
- Jumps to new application

## Flash Map
```
Sector 0  0x08000000  16KB  — Bootloader (this firmware)
Sector 1  0x08004000  16KB  — (reserved)
Sector 2  0x08008000  16KB  — (reserved)
Sector 3  0x0800C000  16KB  — (reserved)
Sector 4  0x08010000  64KB  — Application slot A
Sector 5  0x08020000  128KB — Application slot B (large app)
```

## Jump Mechanism
```c
void jump_to_app(uint32_t addr) {
    __disable_irq();
    // Relocate vector table
    SCB->VTOR = addr;
    // Load stack pointer and reset handler from app vectors
    uint32_t sp  = *(volatile uint32_t*)(addr + 0);
    uint32_t pc  = *(volatile uint32_t*)(addr + 4);
    __set_MSP(sp);
    ((void(*)(void))pc)();
}
```

## UART Commands
| Command | Action |
|---------|--------|
| `boot` | Print boot info and reset cause |
| `flash start` | Begin XMODEM receive, write to Sector 4 |
| `jump` | Validate and jump to app in Sector 4 |
| `verify` | CRC32 check of Sector 4 contents |
| `erase` | Erase Sector 4 |
