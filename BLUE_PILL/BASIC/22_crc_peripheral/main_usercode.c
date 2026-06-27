/*
 * main_usercode.c — 22: STM32 Hardware CRC Peripheral
 *
 * Board  : Blue Pill (STM32F103C8T6), 72 MHz
 * UART1  : PA9 TX / PA10 RX — 115200 baud
 * CRC    : Hardware CRC unit (CRC-32, polynomial 0x04C11DB7)
 *
 * Demonstrates:
 *   1. CRC-32 over a 4-word buffer → compare to expected.
 *   2. Single-bit corruption injected → CRC mismatch detected.
 *
 * NOTE: This code has NOT been tested on hardware.
 *       Paste each section into the matching USER CODE block in main.c.
 *       CubeMX must have the CRC peripheral enabled.
 */

/* ===== USER CODE BEGIN Includes ===== */
#include <stdio.h>
#include <string.h>
/* ===== USER CODE END Includes ===== */


/* ===== USER CODE BEGIN PD (Private Defines) ===== */
/*
 * Pre-computed expected CRC for the test buffer {0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xABCDEF01}.
 * This value was computed using the STM32 hardware CRC algorithm (no input/output bit-reversal).
 * Re-verify on actual hardware if the buffer changes.
 */
#define EXPECTED_CRC    0x89A1897FUL

#define BUFFER_WORDS    4U
/* ===== USER CODE END PD ===== */


/* ===== USER CODE BEGIN PV (Private Variables) ===== */
static uint32_t test_buffer[BUFFER_WORDS] = {
    0xDEADBEEFUL,
    0xCAFEBABEUL,
    0x12345678UL,
    0xABCDEF01UL
};

static char uart_buf[128];
/* ===== USER CODE END PV ===== */


/* ===== USER CODE BEGIN 0 (Helper Functions) ===== */

/**
 * @brief  Print a formatted string over UART1.
 */
static void UART_Print(UART_HandleTypeDef *huart, const char *str)
{
    HAL_UART_Transmit(huart, (const uint8_t *)str, (uint16_t)strlen(str), 200);
}

/* ===== USER CODE END 0 ===== */


/* ===== USER CODE BEGIN 2 (Init, after MX_xxx_Init calls) ===== */
/*
    // ----------------------------------------------------------------
    // Banner
    // ----------------------------------------------------------------
    UART_Print(&huart1, "\r\n=== STM32 Hardware CRC Demo ===\r\n");

    int len = snprintf(uart_buf, sizeof(uart_buf),
                       "Buffer: %u words\r\n", (unsigned)BUFFER_WORDS);
    HAL_UART_Transmit(&huart1, (uint8_t *)uart_buf, (uint16_t)len, 100);

    // ----------------------------------------------------------------
    // Step 1: Compute CRC over unmodified buffer
    // HAL_CRC_Calculate resets the CRC accumulator first, then feeds
    // each 32-bit word to the CRC->DR register.
    // ----------------------------------------------------------------
    uint32_t crc_result = HAL_CRC_Calculate(&hcrc, test_buffer, BUFFER_WORDS);

    len = snprintf(uart_buf, sizeof(uart_buf),
                   "CRC Result : 0x%08lX\r\n", (unsigned long)crc_result);
    HAL_UART_Transmit(&huart1, (uint8_t *)uart_buf, (uint16_t)len, 100);

    len = snprintf(uart_buf, sizeof(uart_buf),
                   "Expected   : 0x%08lX\r\n", (unsigned long)EXPECTED_CRC);
    HAL_UART_Transmit(&huart1, (uint8_t *)uart_buf, (uint16_t)len, 100);

    if (crc_result == EXPECTED_CRC)
    {
        UART_Print(&huart1, "PASS: CRC matches expected value\r\n");
        // LED ON (active-low: set PC13 LOW)
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
    }
    else
    {
        UART_Print(&huart1, "FAIL: CRC mismatch (unexpected result)\r\n");
        // LED OFF
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
    }

    // ----------------------------------------------------------------
    // Step 2: Corruption test — flip bit 0 of word[0]
    // ----------------------------------------------------------------
    UART_Print(&huart1, "\r\n--- Corruption Test ---\r\n");
    UART_Print(&huart1, "Flipping bit 0 of word[0]\r\n");

    uint32_t corrupted_buffer[BUFFER_WORDS];
    memcpy(corrupted_buffer, test_buffer, sizeof(test_buffer));
    corrupted_buffer[0] ^= 0x00000001UL;   // flip LSB

    uint32_t crc_corrupted = HAL_CRC_Calculate(&hcrc, corrupted_buffer, BUFFER_WORDS);

    len = snprintf(uart_buf, sizeof(uart_buf),
                   "Corrupted CRC: 0x%08lX\r\n", (unsigned long)crc_corrupted);
    HAL_UART_Transmit(&huart1, (uint8_t *)uart_buf, (uint16_t)len, 100);

    if (crc_corrupted != EXPECTED_CRC)
    {
        UART_Print(&huart1, "MISMATCH: Corruption detected!\r\n");
    }
    else
    {
        // Extremely unlikely for CRC-32 but worth handling
        UART_Print(&huart1, "COLLISION: CRC matched despite corruption (unlikely)\r\n");
    }

    UART_Print(&huart1, "\r\nDemo complete. Entering idle loop.\r\n");
*/
/* ===== USER CODE END 2 ===== */


/* ===== USER CODE BEGIN WHILE (Main loop — idle after one-shot demo) ===== */
/*
    while (1)
    {
        // Toggle LED slowly to show the MCU is alive
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        HAL_Delay(1000);
    }
*/
/* ===== USER CODE END WHILE ===== */
