# Project 25: NVIC Interrupt Priority & Preemption

**Board:** STM32F411CEU6 Black Pill  
**Concept:** Three concurrent IRQs demonstrating NVIC preemption hierarchy

---

## What This Project Demonstrates

The ARM Cortex-M4 NVIC (Nested Vectored Interrupt Controller) allows higher-priority interrupts to preempt lower-priority ones mid-execution. This project wires up three interrupt sources at three distinct preempt priorities so you can observe the nesting in action.

| Interrupt | Priority (Preempt) | Trigger | Action |
|-----------|-------------------|---------|--------|
| TIM2 | 0 (highest) | 1 kHz timer | Increment `g_timestamp` counter |
| USART1 RX | 1 | Incoming byte | Append byte to `rx_buf[]` |
| EXTI0 (PA0) | 2 (lowest) | Button press | Toggle PC13 LED |

**Key rule:** A lower preempt number beats a higher one.  
TIM2 (0) can preempt UART ISR (1) which can preempt EXTI (2).

Priority grouping: **NVIC_PRIORITYGROUP_4** — 4 bits for preempt, 0 bits for sub-priority. This gives 16 preempt levels and removes sub-priority ambiguity entirely.

---

## CubeMX Configuration

### System Core
- **RCC:** HSE Crystal, 100 MHz via PLL
- **SYS:** No debug (frees PA13/PA14 — or keep SWD if using ST-Link)
- **NVIC → Priority Group:** Group 4 (4 bits preempt / 0 sub)

### TIM2
- **Mode:** Internal Clock
- **Prescaler (PSC):** 99  
- **Counter Period (ARR):** 999  
- **Clock:** 100 MHz → TIM2 tick = 100 MHz / (99+1) = 1 MHz → overflow at 1000 = **1 kHz**
- **NVIC → TIM2 global interrupt:** Enabled, Preempt=0, Sub=0

### USART1
- **Mode:** Asynchronous
- **Baud:** 115200, 8N1
- **PA9:** TX, **PA10:** RX
- **NVIC → USART1 global interrupt:** Enabled, Preempt=1, Sub=0

### GPIO
- **PA0:** GPIO_Input, Pull-Up, EXTI0 (rising edge for button release, or falling for press)
- **PC13:** GPIO_Output, Push-Pull (Onboard LED, active-low)
- **NVIC → EXTI line 0 interrupt:** Enabled, Preempt=2, Sub=0

---

## How Preemption Works

```
Timeline (not to scale):

        EXTI0 ISR begins (priority 2)
        |
        |  TIM2 fires (priority 0) ──────────── preempts EXTI!
        |  |  g_timestamp++
        |  |  TIM2 ISR returns
        |  (EXTI0 ISR resumes)
        |
        |  USART1 fires (priority 1) ─────────── also preempts EXTI!
        |  |  rx_buf[head++] = DR
        |  |  USART1 ISR returns
        |  (EXTI0 ISR resumes)
        |
        EXTI0 ISR completes
```

The main loop polls `g_timestamp`, `rx_buf`, and LED state each second and prints a status line to UART1, showing all three are updating concurrently.

---

## DFU Flashing Instructions

1. Hold **BOOT0** button
2. While holding BOOT0, briefly press and release **NRST**
3. Wait ~0.5 s, then release BOOT0
4. Board enumerates as **"STM32 BOOTLOADER"** USB device
5. Open **STM32CubeProgrammer** → USB → Connect → Download .hex or .bin
6. Press **NRST** to exit DFU and run firmware

**Alternative:** External ST-Link V2 via SWD  
- PA13 = SWDIO, PA14 = SWCLK, GND, 3.3V

---

## Serial Monitor

Open at **115200 baud**. Every second you will see lines like:

```
[TICK:00001042] EXTI_toggles:3 RX_chars:7 buf="hello\r\n"
[TICK:00002089] EXTI_toggles:5 RX_chars:14 buf="hello\r\nhello\r"
```

Send any text from your terminal to exercise the UART ISR.

---

## Files

| File | Purpose |
|------|---------|
| `README.md` | This file |
| `main_usercode.c` | User code: ISR handlers, shared volatiles, main print loop |
| `wiring_notes.md` | Pin connections and button wiring |

---

## Expected Behavior

- LED (PC13) toggles each time PA0 button is pressed
- `g_timestamp` increments 1000× per second (TIM2 @ 1kHz)
- Received UART bytes appear in `rx_buf`
- UART output every 1 s proves all three ISRs fire without corruption
