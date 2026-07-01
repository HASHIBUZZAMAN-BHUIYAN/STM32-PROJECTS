// ============================================================
// Nucleo-F401RE BASIC/15 — 84 MHz Clock Timing Demo
// STM32F401RETx  |  HAL + CubeMX 6.16
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
// At 84 MHz, one NOP ≈ 11.9 ns.  For a 500 kHz half-period (1 µs = 84 cycles):
// GPIO write takes ~3 cycles, so 84-3 = 81 NOPs per half-cycle → tune empirically.
#define HALF_CYCLE_NOPS  40   // approximate; tune with scope
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN 0 =====
static inline void delay_nops(uint32_t n)
{
    while (n--) __NOP();
}

/* Bit-bang 1MHz square wave on PB0 for 'ms' milliseconds */
static void toggle_1mhz(uint32_t ms)
{
    uint32_t end = HAL_GetTick() + ms;
    while (HAL_GetTick() < end) {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
        delay_nops(HALF_CYCLE_NOPS);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
        delay_nops(HALF_CYCLE_NOPS);
    }
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    HAL_UART_Transmit(&huart2,
        (uint8_t*)"SYSCLK=84MHz | bit-bang 1MHz on PB0 | measure with scope\r\n", 60, 100);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    uint32_t cycles = 0;
    char buf[64];
    while (1)
    {
        // Generate ~1 MHz square wave for 1 second
        toggle_1mhz(1000);
        cycles++;
        int n = snprintf(buf, sizeof(buf), "Cycle %lu done (84MHz Nucleo)\r\n", cycles);
        HAL_UART_Transmit(&huart2, (uint8_t*)buf, n, 100);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====
