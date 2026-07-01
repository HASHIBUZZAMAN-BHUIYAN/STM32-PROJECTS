// ============================================================
// Black Pill BASIC/20 — Hardware CRC32 Peripheral
// STM32F411CEU6  |  HAL + CubeMX 6.16  |  100 MHz
// CRC unit + software CRC32 comparison, USART1
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PV =====
static uint32_t test_data[] = {
    0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xABCDEF01,
    0x00000000, 0xFFFFFFFF, 0x01020304, 0xFEDCBA98
};
static char buf[80];
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static void uart_tx(const char *s) {
    HAL_UART_Transmit(&huart1, (const uint8_t*)s, strlen(s), 200);
}

// Software CRC32 (IEEE 802.3 polynomial) — for comparison
static uint32_t sw_crc32(const uint8_t *data, uint32_t len) {
    uint32_t crc = 0xFFFFFFFF;
    for (uint32_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 1) crc = (crc >> 1) ^ 0xEDB88320;
            else         crc >>= 1;
        }
    }
    return ~crc;
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    uart_tx("Black Pill Hardware CRC32 demo\r\n");

    // Hardware CRC32
    uint32_t hw_crc = HAL_CRC_Calculate(&hcrc, test_data,
                                         sizeof(test_data)/sizeof(uint32_t));

    // Software CRC32 (standard bit-reversed polynomial)
    uint32_t sw_crc = sw_crc32((uint8_t*)test_data, sizeof(test_data));

    int n = snprintf(buf, sizeof(buf), "HW CRC32: 0x%08lX\r\n", hw_crc);
    uart_tx(buf);
    n = snprintf(buf, sizeof(buf), "SW CRC32: 0x%08lX\r\n", sw_crc);
    uart_tx(buf);

    // Note: HW and SW may differ due to bit order / initial value differences
    // The STM32 CRC uses MSB-first, no bit reversal; standard CRC32 is LSB-first
    uart_tx("(Note: HW uses MSB-first; SW uses LSB-first — different results)\r\n");

    // Demonstrate accumulate: split same data into two halves
    __HAL_CRC_DR_RESET(&hcrc);
    uint32_t crc_a = HAL_CRC_Accumulate(&hcrc, test_data, 4);
    uint32_t crc_b = HAL_CRC_Accumulate(&hcrc, test_data + 4, 4);
    uint32_t crc_full = HAL_CRC_Calculate(&hcrc, test_data,
                                           sizeof(test_data)/sizeof(uint32_t));
    n = snprintf(buf, sizeof(buf),
                 "Acc(A)=0x%08lX Acc(B)=0x%08lX Full=0x%08lX\r\n",
                 crc_a, crc_b, crc_full);
    uart_tx(buf);
    uart_tx(crc_b == crc_full ? "Accumulate matches full: YES\r\n"
                               : "Accumulate matches full: NO\r\n");
    (void)n;
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        HAL_Delay(1000);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====
