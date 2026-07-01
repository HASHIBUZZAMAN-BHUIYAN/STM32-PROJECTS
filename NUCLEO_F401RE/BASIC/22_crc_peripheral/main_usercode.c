// ============================================================
// Nucleo-F401RE BASIC/22 — Hardware CRC32 Peripheral
// STM32F401RETx  |  HAL + CubeMX 6.16
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PV =====
static uint32_t test_data[] = {
    0x12345678, 0xABCDEF01, 0xDEADBEEF,
    0x01234567, 0x89ABCDEF, 0xFEDCBA98,
    0x11223344, 0x55667788
};
static char buf[80];
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 2 =====
    HAL_UART_Transmit(&huart2, (uint8_t*)"CRC32 Hardware Test:\r\n", 21, 100);

    // Compute CRC over the test buffer
    uint32_t crc1 = HAL_CRC_Calculate(&hcrc, test_data,
                                        sizeof(test_data) / sizeof(uint32_t));
    int n = snprintf(buf, sizeof(buf), "  Buffer CRC: 0x%08lX\r\n", crc1);
    HAL_UART_Transmit(&huart2, (uint8_t*)buf, n, 100);

    // Accumulate (incremental): same result if computed in two halves
    HAL_CRC_Calculate(&hcrc, test_data, 4);               // first half resets
    uint32_t crc2 = HAL_CRC_Accumulate(&hcrc, &test_data[4], 4); // second half
    n = snprintf(buf, sizeof(buf), "  Incremental CRC: 0x%08lX %s\r\n",
                 crc2, crc1 == crc2 ? "MATCH" : "MISMATCH");
    HAL_UART_Transmit(&huart2, (uint8_t*)buf, n, 100);

    // Flip one bit — CRC should differ
    uint32_t corrupted[sizeof(test_data)/sizeof(uint32_t)];
    memcpy(corrupted, test_data, sizeof(test_data));
    corrupted[3] ^= 0x00000001; // flip LSB of word 3
    uint32_t crc3 = HAL_CRC_Calculate(&hcrc, corrupted,
                                        sizeof(corrupted) / sizeof(uint32_t));
    n = snprintf(buf, sizeof(buf), "  Corrupted CRC: 0x%08lX -> %s\r\n",
                 crc3, crc3 != crc1 ? "CORRUPTION DETECTED" : "MISS (false negative)");
    HAL_UART_Transmit(&huart2, (uint8_t*)buf, n, 100);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        HAL_Delay(5000);
        // Re-run CRC test every 5 seconds to show it's deterministic
        uint32_t crc_check = HAL_CRC_Calculate(&hcrc, test_data,
                                                 sizeof(test_data)/sizeof(uint32_t));
        n = snprintf(buf, sizeof(buf), "Periodic CRC: 0x%08lX %s\r\n",
                     crc_check, crc_check == crc1 ? "OK" : "ERROR");
        HAL_UART_Transmit(&huart2, (uint8_t*)buf, n, 100);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====
