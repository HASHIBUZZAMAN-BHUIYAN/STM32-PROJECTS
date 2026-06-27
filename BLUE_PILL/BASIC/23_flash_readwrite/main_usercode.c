/*
 * main_usercode.c — 23: Internal Flash Read / Write (Boot Counter)
 *
 * Board  : Blue Pill (STM32F103C8T6), 72 MHz, 64 KB Flash
 * UART1  : PA9 TX / PA10 RX — 115200 baud
 *
 * Stores a uint32_t boot counter at Flash page 63 (0x0800FC00).
 * Each reset increments the counter by 1 and prints it over UART1.
 *
 * Sequence every boot:
 *   1. Read current value from Flash (0xFFFFFFFF means erased / first boot).
 *   2. Unlock Flash.
 *   3. Erase page 63.
 *   4. Write incremented counter.
 *   5. Lock Flash.
 *   6. Read back and verify.
 *   7. Print result over UART1.
 *
 * NOTE: This code has NOT been tested on hardware.
 *       Paste each section into the matching USER CODE block in main.c.
 *       No special CubeMX peripheral config is required beyond USART1.
 */

/* ===== USER CODE BEGIN Includes ===== */
#include <stdio.h>
#include <string.h>
/* ===== USER CODE END Includes ===== */


/* ===== USER CODE BEGIN PD (Private Defines) ===== */
/* STM32F103C8T6: 64 KB Flash, 1 KB pages (pages 0–63).
   Page 63 starts at 0x08000000 + 63 * 0x400 = 0x0800FC00. */
#define FLASH_PAGE63_ADDR   0x0800FC00UL
#define FLASH_PAGE63_NUMBER 63U

/* Magic: erased Flash reads as all 1s */
#define FLASH_ERASED_WORD   0xFFFFFFFFUL
/* ===== USER CODE END PD ===== */


/* ===== USER CODE BEGIN PV (Private Variables) ===== */
static char uart_buf[96];
/* ===== USER CODE END PV ===== */


/* ===== USER CODE BEGIN 0 (Helper Functions) ===== */

/**
 * @brief  Read a uint32_t from Flash using direct pointer dereference.
 *         Flash memory is memory-mapped and readable without any HAL calls.
 */
static uint32_t Flash_ReadWord(uint32_t address)
{
    return *((volatile uint32_t *)address);
}

/**
 * @brief  Erase page 63 then write a single 32-bit word.
 * @retval HAL_OK on success, HAL_ERROR on any failure.
 */
static HAL_StatusTypeDef Flash_WriteCounter(uint32_t value)
{
    HAL_StatusTypeDef status;

    /* --- Unlock --- */
    status = HAL_FLASH_Unlock();
    if (status != HAL_OK)
    {
        return status;
    }

    /* --- Erase page 63 --- */
    FLASH_EraseInitTypeDef erase_cfg;
    uint32_t page_error = 0U;

    erase_cfg.TypeErase   = FLASH_TYPEERASE_PAGES;
    erase_cfg.PageAddress = FLASH_PAGE63_ADDR;
    erase_cfg.NbPages     = 1U;

    status = HAL_FLASHEx_Erase(&erase_cfg, &page_error);
    if (status != HAL_OK)
    {
        HAL_FLASH_Lock();
        return status;
    }

    if (page_error != 0xFFFFFFFFUL)
    {
        /* page_error holds the address of the first page that failed to erase */
        HAL_FLASH_Lock();
        return HAL_ERROR;
    }

    /* --- Write 32-bit word --- */
    status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_PAGE63_ADDR, (uint64_t)value);

    /* --- Lock regardless of write result --- */
    HAL_FLASH_Lock();

    return status;
}

/* ===== USER CODE END 0 ===== */


/* ===== USER CODE BEGIN 2 (Init, after MX_xxx_Init calls) ===== */
/*
    const char *banner = "\r\n=== Flash Boot Counter Demo ===\r\n";
    HAL_UART_Transmit(&huart1, (uint8_t *)banner, strlen(banner), 100);

    // --- Step 1: Read current counter from Flash ---
    uint32_t stored_value = Flash_ReadWord(FLASH_PAGE63_ADDR);

    uint32_t new_counter;
    if (stored_value == FLASH_ERASED_WORD)
    {
        // First boot or manually erased
        new_counter = 1U;
        int len = snprintf(uart_buf, sizeof(uart_buf),
                           "Flash was erased. Initialising counter to %lu.\r\n",
                           (unsigned long)new_counter);
        HAL_UART_Transmit(&huart1, (uint8_t *)uart_buf, (uint16_t)len, 100);
    }
    else
    {
        new_counter = stored_value + 1U;
        int len = snprintf(uart_buf, sizeof(uart_buf),
                           "Previous boot count: %lu  ->  New: %lu\r\n",
                           (unsigned long)stored_value,
                           (unsigned long)new_counter);
        HAL_UART_Transmit(&huart1, (uint8_t *)uart_buf, (uint16_t)len, 100);
    }

    // --- Step 2: Write incremented counter ---
    HAL_StatusTypeDef write_status = Flash_WriteCounter(new_counter);

    if (write_status != HAL_OK)
    {
        const char *err = "ERROR: Flash write failed!\r\n";
        HAL_UART_Transmit(&huart1, (uint8_t *)err, strlen(err), 100);
        // Turn LED ON to signal error (active-low: PC13 LOW)
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
    }
    else
    {
        // --- Step 3: Verify readback ---
        uint32_t readback = Flash_ReadWord(FLASH_PAGE63_ADDR);

        if (readback == new_counter)
        {
            int len = snprintf(uart_buf, sizeof(uart_buf),
                               "Write OK. Readback: %lu\r\n",
                               (unsigned long)readback);
            HAL_UART_Transmit(&huart1, (uint8_t *)uart_buf, (uint16_t)len, 100);
            // LED OFF (success)
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
        }
        else
        {
            int len = snprintf(uart_buf, sizeof(uart_buf),
                               "ERROR: Readback mismatch! Expected %lu, got %lu\r\n",
                               (unsigned long)new_counter,
                               (unsigned long)readback);
            HAL_UART_Transmit(&huart1, (uint8_t *)uart_buf, (uint16_t)len, 100);
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
        }
    }

    const char *done = "Done. Entering idle loop.\r\n";
    HAL_UART_Transmit(&huart1, (uint8_t *)done, strlen(done), 100);
*/
/* ===== USER CODE END 2 ===== */


/* ===== USER CODE BEGIN WHILE (Main loop — idle) ===== */
/*
    while (1)
    {
        // Blink LED to show the system is alive
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        HAL_Delay(500);
    }
*/
/* ===== USER CODE END WHILE ===== */
