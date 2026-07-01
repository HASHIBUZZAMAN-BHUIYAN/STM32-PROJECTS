// ============================================================
// Nucleo-F401RE ADVANCED/07 — Internal Flash EEPROM Emulation
// STM32F401RETx  |  HAL + CubeMX 6.16
// Uses Sector 7 (0x08060000) — 128 KB, last sector
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
#define FLASH_USER_SECTOR    FLASH_SECTOR_7
#define FLASH_USER_ADDR      0x08060000UL
#define FLASH_MAGIC          0xDEADBEEFUL

// Data layout in flash (word-aligned):
// [0] MAGIC
// [1] boot_count
// [2] user_value
// [3] checksum = MAGIC ^ boot_count ^ user_value
#define FLASH_OFFSET_MAGIC  0
#define FLASH_OFFSET_BOOTCT 4
#define FLASH_OFFSET_VALUE  8
#define FLASH_OFFSET_CRC    12
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
static char buf[80];
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static uint32_t flash_read_word(uint32_t offset) {
    return *(__IO uint32_t*)(FLASH_USER_ADDR + offset);
}

static uint8_t flash_data_valid(void) {
    uint32_t magic    = flash_read_word(FLASH_OFFSET_MAGIC);
    uint32_t boot_ct  = flash_read_word(FLASH_OFFSET_BOOTCT);
    uint32_t value    = flash_read_word(FLASH_OFFSET_VALUE);
    uint32_t checksum = flash_read_word(FLASH_OFFSET_CRC);
    if (magic != FLASH_MAGIC) return 0;
    return (checksum == (FLASH_MAGIC ^ boot_ct ^ value)) ? 1 : 0;
}

static HAL_StatusTypeDef flash_erase_user_sector(void) {
    FLASH_EraseInitTypeDef e = {
        .TypeErase    = FLASH_TYPEERASE_SECTORS,
        .Sector       = FLASH_USER_SECTOR,
        .NbSectors    = 1,
        .VoltageRange = FLASH_VOLTAGE_RANGE_3
    };
    uint32_t err = 0;
    return HAL_FLASHEx_Erase(&e, &err);
}

static HAL_StatusTypeDef flash_write_data(uint32_t boot_count, uint32_t user_val) {
    uint32_t checksum = FLASH_MAGIC ^ boot_count ^ user_val;

    HAL_FLASH_Unlock();
    HAL_StatusTypeDef r = flash_erase_user_sector();
    if (r == HAL_OK) r = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_USER_ADDR + FLASH_OFFSET_MAGIC,  FLASH_MAGIC);
    if (r == HAL_OK) r = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_USER_ADDR + FLASH_OFFSET_BOOTCT, boot_count);
    if (r == HAL_OK) r = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_USER_ADDR + FLASH_OFFSET_VALUE,  user_val);
    if (r == HAL_OK) r = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_USER_ADDR + FLASH_OFFSET_CRC,    checksum);
    HAL_FLASH_Lock();
    return r;
}

static void uart_tx(const char *s) {
    HAL_UART_Transmit(&huart2, (const uint8_t*)s, strlen(s), 200);
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    uart_tx("Flash EEPROM emulation demo\r\n");

    uint32_t boot_count = 1;
    uint32_t user_value = 42; // example config value

    if (flash_data_valid()) {
        boot_count = flash_read_word(FLASH_OFFSET_BOOTCT) + 1;
        user_value = flash_read_word(FLASH_OFFSET_VALUE);
        int n = snprintf(buf, sizeof(buf),
                         "Loaded: boot_count=%lu user_val=%lu\r\n",
                         boot_count, user_value);
        uart_tx(buf);
        (void)n;
    } else {
        uart_tx("No valid data in flash — first boot\r\n");
    }

    // Save updated boot count
    HAL_StatusTypeDef r = flash_write_data(boot_count, user_value);
    if (r == HAL_OK) {
        int n = snprintf(buf, sizeof(buf),
                         "Saved: boot_count=%lu user_val=%lu\r\n",
                         boot_count, user_value);
        uart_tx(buf);
        (void)n;
    } else {
        uart_tx("Flash write FAILED\r\n");
    }

    uart_tx("Reset board to increment boot count.\r\n");
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        HAL_Delay(1000);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====
