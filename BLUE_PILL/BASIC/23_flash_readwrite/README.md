# 23 — Internal Flash Read / Write (Boot Counter)

## Overview
Stores a persistent `uint32_t` boot counter in the last Flash page (page 63, address
`0x0800FC00`) of the STM32F103C8T6 (64 KB part). Each power cycle increments the counter by 1.
The value survives resets because it lives in Flash, not RAM. A page erase is performed before
each write because Flash bits can only be cleared (0) by erase — not set back to 1 individually.

## Hardware
- MCU: STM32F103C8T6 (Blue Pill), 72 MHz
- No external components needed beyond a USB-TTL adapter on UART1.

## CubeMX Configuration
No special peripheral configuration is required for Flash access. The HAL Flash driver
(`stm32f1xx_hal_flash.c`, `stm32f1xx_hal_flash_ex.c`) is included by default.

### USART1
| Parameter | Value |
|-----------|-------|
| Baud Rate | 115200 |
| Word Length | 8 bits |
| Parity | None |
| Stop Bits | 1 |

### GPIO
| Pin | Mode |
|-----|------|
| PA9 | USART1_TX |
| PA10 | USART1_RX |
| PC13 | GPIO_Output (onboard LED, active-low) |

## Flash Map (STM32F103C8T6, 64 KB)
| Page | Address Range | Size |
|------|---------------|------|
| 0 | 0x08000000 – 0x080003FF | 1 KB |
| … | … | … |
| 63 | 0x0800FC00 – 0x0800FFFF | 1 KB |

Page 63 is the last page and is used here for configuration storage to minimize interference
with the program code, which starts at page 0.

## Important Caveats
- STM32F103 Flash endurance: **10,000 erase cycles** typical. Do not erase on every loop iteration.
- If the stored value reads as `0xFFFFFFFF` (erased state), the counter initialises to 0.
- The linker script must NOT place any code or read-only data at `0x0800FC00`. Verify in `.map` file.
- `HAL_FLASH_Unlock()` must always be paired with `HAL_FLASH_Lock()`.

## Notes
- This code has NOT been tested on hardware.
