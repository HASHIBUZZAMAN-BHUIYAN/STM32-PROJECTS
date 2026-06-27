/*
 * Project 09 — I2C Bus Scanner
 * Board: STM32F411CEU6 Black Pill (WeAct Studio)
 *
 * Paste the contents of this file into the USER CODE sections of the
 * CubeMX-generated main.c:
 *
 *   - Includes section  → add #include <stdio.h>
 *   - USER CODE BEGIN 2 → add the initial banner print
 *   - USER CODE BEGIN 3 → add the while(1) scan loop
 *
 * Peripherals initialised by CubeMX:
 *   hi2c1  — I2C1 Standard Mode 100kHz, PB6=SCL, PB7=SDA
 *   huart1 — USART1 115200 8N1, PA9=TX, PA10=RX
 */

/* ------------------------------------------------------------------ */
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */


/* ------------------------------------------------------------------ */
/* Helper: transmit a null-terminated string over USART1 */
static void uart_print(const char *str)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)str, (uint16_t)strlen(str), HAL_MAX_DELAY);
}


/* ------------------------------------------------------------------ */
/* USER CODE BEGIN 2  (runs once after all MX_xxx_Init calls) */

    char msg[64];

    uart_print("\r\n");
    uart_print("================================================\r\n");
    uart_print("  I2C Bus Scanner -- STM32F411 Black Pill\r\n");
    uart_print("  I2C1: PB6=SCL  PB7=SDA  @ 100 kHz\r\n");
    uart_print("================================================\r\n");

/* USER CODE END 2 */


/* ------------------------------------------------------------------ */
/* USER CODE BEGIN 3  (while(1) body) */

    uint8_t found_count = 0;

    uart_print("\r\nScanning addresses 0x01 to 0x7F...\r\n");

    for (uint8_t addr = 0x01; addr <= 0x7F; addr++)
    {
        /*
         * HAL_I2C_IsDeviceReady:
         *   addr  — 7-bit address shifted left by 1 (HAL convention)
         *   3     — number of trials
         *   10    — timeout in ms per trial
         *
         * Returns HAL_OK if the device ACKs the address phase.
         */
        HAL_StatusTypeDef result = HAL_I2C_IsDeviceReady(
            &hi2c1,
            (uint16_t)(addr << 1),  /* shift to 8-bit address format */
            3,
            10
        );

        if (result == HAL_OK)
        {
            snprintf(msg, sizeof(msg), "  Found device at address: 0x%02X\r\n", addr);
            uart_print(msg);
            found_count++;
        }
    }

    /* Print summary */
    snprintf(msg, sizeof(msg),
             "Scan complete. %u device(s) found.\r\n", found_count);
    uart_print(msg);

    uart_print("--- Next scan in 5 seconds ---\r\n");

    found_count = 0;   /* reset for next scan */

    HAL_Delay(5000);

/* USER CODE END 3 */
