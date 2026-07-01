// ============================================================
// Black Pill BASIC/13 — USB DFU Bootloader Guide
// STM32F411CEU6  |  HAL + CubeMX 6.16  |  100 MHz
//
// This project has no custom firmware — the firmware is
// BASIC/01 (blink) used as a DFU test payload.
//
// See README.md for the complete DFU flashing procedure.
// This file demonstrates a firmware that self-reports its
// version over USART1 so you can confirm which version is
// running after a DFU update.
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <string.h>
#include <stdio.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
#define FW_VERSION "1.0.0"
#define FW_DATE    __DATE__ " " __TIME__
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN 2 =====
    // Print firmware version — useful to confirm after DFU update
    char banner[96];
    int n = snprintf(banner, sizeof(banner),
                     "\r\nFirmware v" FW_VERSION " built " FW_DATE "\r\n"
                     "DFU guide demo. LED blinks = running.\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)banner, n, 200);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        // PC13 active-LOW: toggle for visual confirmation
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        HAL_Delay(500);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====
