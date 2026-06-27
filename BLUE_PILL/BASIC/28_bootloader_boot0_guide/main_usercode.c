/*
 * main_usercode.c — Project 28: Bootloader BOOT0 Guide — Smoke Test Firmware
 * Target: STM32F103C8T6 (Blue Pill)
 *
 * PURPOSE: Minimal smoke test to confirm the firmware flashed via the
 * STM32 ROM UART bootloader is running correctly from user Flash (0x08000000).
 *
 * Expected behavior after flashing:
 *   1. USART1 prints one banner line at startup.
 *   2. Onboard LED (PC13, active-low) blinks at 1 Hz indefinitely.
 *
 * CubeMX requirements:
 *   - PC13: GPIO Output, Push-Pull, No Pull, Low speed (onboard LED)
 *   - USART1: Asynchronous, 115200 baud, 8N1  (switch BACK to 8N1 for normal use;
 *             the bootloader uses 8E1, but user firmware typically uses 8N1)
 *   - SysTick for HAL_Delay
 *
 * NOTE: This code has not been tested on hardware.
 */

// ===== USER CODE BEGIN Includes =====
#include <string.h>
// ===== USER CODE END Includes =====


// ===== USER CODE BEGIN PD (Private Defines) =====
#define LED_PIN     GPIO_PIN_13
#define LED_PORT    GPIOC
#define BLINK_MS    500U
// ===== USER CODE END PD =====


// ===== USER CODE BEGIN PV (Private Variables) =====
/*
 * extern declarations — remove if pasting directly into CubeMX main.c
 * where huart1 is already defined.
 */
extern UART_HandleTypeDef huart1;

static const char banner[] = "Bootloader flash OK - running user code\r\n";
// ===== USER CODE END PV =====


// ===== USER CODE BEGIN 2 (inside main(), after MX_xxx_Init calls) =====
/*
 * Send the startup banner via blocking UART TX.
 * If you see this line in your terminal, Flash programming succeeded.
 */
HAL_UART_Transmit(&huart1,
                  (uint8_t *)banner,
                  (uint16_t)(sizeof(banner) - 1),
                  200);

/* Ensure LED starts OFF (PC13 HIGH = LED off, active-low) */
HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);
// ===== USER CODE END 2 =====


// ===== USER CODE BEGIN WHILE (inside the while(1) loop) =====
/*
 * 1 Hz blink: 500 ms ON, 500 ms OFF.
 * Active-low: GPIO_PIN_RESET = LED ON, GPIO_PIN_SET = LED OFF.
 */
HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET); /* LED ON  */
HAL_Delay(BLINK_MS);
HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);   /* LED OFF */
HAL_Delay(BLINK_MS);
// ===== USER CODE END WHILE =====
