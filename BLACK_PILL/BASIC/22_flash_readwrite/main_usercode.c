// ============================================================
// Black Pill BASIC/22 — Internal Flash Read/Write
// STM32F411CEU6  |  HAL + CubeMX 6.16  |  100 MHz
// Uses Sector 7 @ 0x08060000 for user data
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
#define USER_FLASH_ADDR  0x08060000UL
#define USER_FLASH_SECTOR FLASH_SECTOR_7
#define TEST_WORDS       16
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
static char buf[64];
static uint8_t all_ok = 1;
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static void uart_tx(const char *s) {
    HAL_UART_Transmit(&huart1, (const uint8_t*)s, strlen(s), 200);
}

static HAL_StatusTypeDef flash_erase_sector7(void) {
    FLASH_EraseInitTypeDef e = {
        .TypeErase    = FLASH_TYPEERASE_SECTORS,
        .Sector       = USER_FLASH_SECTOR,
        .NbSectors    = 1,
        .VoltageRange = FLASH_VOLTAGE_RANGE_3
    };
    uint32_t err = 0;
    return HAL_FLASHEx_Erase(&e, &err);
}

static HAL_StatusTypeDef flash_write_words(uint32_t addr, uint32_t *data, uint16_t count) {
    for (uint16_t i = 0; i < count; i++) {
        HAL_StatusTypeDef r = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
                                                 addr + i*4, data[i]);
        if (r != HAL_OK) return r;
    }
    return HAL_OK;
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    uart_tx("Black Pill Flash Read/Write demo\r\n");

    // Prepare test data
    uint32_t write_data[TEST_WORDS];
    for (int i = 0; i < TEST_WORDS; i++) write_data[i] = (uint32_t)(i + 1);

    // Erase
    uart_tx("Erasing Sector 7...\r\n");
    HAL_FLASH_Unlock();
    HAL_StatusTypeDef r = flash_erase_sector7();
    if (r != HAL_OK) { uart_tx("Erase FAILED\r\n"); goto done; }
    uart_tx("Erase OK\r\n");

    // Write
    uart_tx("Writing...\r\n");
    r = flash_write_words(USER_FLASH_ADDR, write_data, TEST_WORDS);
    HAL_FLASH_Lock();
    if (r != HAL_OK) { uart_tx("Write FAILED\r\n"); goto done; }

    // Verify
    for (int i = 0; i < TEST_WORDS; i++) {
        uint32_t read_val = *(__IO uint32_t*)(USER_FLASH_ADDR + i*4);
        if (read_val != write_data[i]) {
            int n = snprintf(buf, sizeof(buf),
                             "FAIL[%d]: wrote 0x%08lX read 0x%08lX\r\n",
                             i, write_data[i], read_val);
            uart_tx(buf); all_ok = 0;
            (void)n;
        }
    }
    uart_tx(all_ok ? "Verify: ALL OK\r\n" : "Verify: SOME FAILED\r\n");

done:;
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        HAL_Delay(all_ok ? 500 : 100); // fast blink on failure
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====
