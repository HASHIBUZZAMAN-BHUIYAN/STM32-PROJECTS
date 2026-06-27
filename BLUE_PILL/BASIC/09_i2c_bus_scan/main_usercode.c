/*
 * main_usercode.c — 09_i2c_bus_scan
 *
 * STM32F103C8T6 Blue Pill
 * Scan I2C1 (PB6/PB7) for all 7-bit addresses 0x01..0x7F.
 * Print results via USART1 (PA9 TX).
 * Repeat scan every 5 seconds.
 *
 * INSTRUCTIONS:
 *   Copy each labelled section into the matching USER CODE BEGIN/END block
 *   inside the CubeMX-generated main.c.
 *
 * NOTE: Code is not tested on hardware.
 *       Verify handle names (hi2c1, huart1) match your CubeMX output.
 *       I2C1 requires external pull-up resistors — see wiring_notes.md.
 */

// ===== USER CODE BEGIN Includes =====
#include <string.h>   /* strlen */
#include <stdio.h>    /* snprintf */
// ===== USER CODE END Includes =====


// ===== USER CODE BEGIN PD (Private Defines) =====
#define I2C_SCAN_TRIALS      3U    /* attempts per address */
#define I2C_SCAN_TIMEOUT_MS  10U   /* timeout per attempt (ms) */
#define I2C_SCAN_INTERVAL_MS 5000U /* delay between full scans */
// ===== USER CODE END PD =====


// ===== USER CODE BEGIN PFP (Private Function Prototypes) =====
static void uart_print(const char *str);
// ===== USER CODE END PFP =====


// ===== USER CODE BEGIN 2 (after MX_xxx_Init calls, inside main()) =====
{
    uart_print("=== I2C Bus Scan ===\r\n");
    uart_print("Board: STM32F103C8T6 Blue Pill\r\n");
    uart_print("I2C1: PB6 (SCL) / PB7 (SDA) @ 100 kHz\r\n\r\n");
}
// ===== USER CODE END 2 =====


// ===== USER CODE BEGIN WHILE (inside the infinite while(1) loop) =====
{
    char msg[48];
    uint8_t found_count = 0;

    uart_print("Scanning 0x01 - 0x7F ...\r\n");

    for (uint8_t addr = 0x01U; addr <= 0x7FU; addr++)
    {
        /*
         * HAL_I2C_IsDeviceReady generates a START + address byte (write) +
         * STOP. If the device ACKs, it returns HAL_OK.
         * The 7-bit address is passed as (addr << 1) per STM32 HAL convention.
         */
        HAL_StatusTypeDef result = HAL_I2C_IsDeviceReady(
                                       &hi2c1,
                                       (uint16_t)(addr << 1U),
                                       I2C_SCAN_TRIALS,
                                       I2C_SCAN_TIMEOUT_MS);

        if (result == HAL_OK)
        {
            snprintf(msg, sizeof(msg), "Found device at 0x%02X\r\n", addr);
            uart_print(msg);
            found_count++;

            /* Brief LED blink for each found device */
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET); /* LED ON */
            HAL_Delay(50);
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);   /* LED OFF */
        }
        /* HAL_BUSY or HAL_TIMEOUT means no device ACKed — normal, skip */
    }

    snprintf(msg, sizeof(msg),
             "Scan complete. %u device(s) found.\r\n\r\n",
             (unsigned)found_count);
    uart_print(msg);

    /* Wait before next scan */
    HAL_Delay(I2C_SCAN_INTERVAL_MS);
}
// ===== USER CODE END WHILE =====


// ===== USER CODE BEGIN 4 (helper functions, outside main()) =====

/**
 * @brief  Transmit a null-terminated string over USART1 (blocking).
 * @param  str  Pointer to the string to send.
 */
static void uart_print(const char *str)
{
    uint16_t len = (uint16_t)strlen(str);
    if (len > 0U)
    {
        HAL_UART_Transmit(&huart1, (uint8_t *)str, len, HAL_MAX_DELAY);
    }
}

// ===== USER CODE END 4 =====
