/*
 * main_usercode.c — 07_uart_basics
 *
 * STM32F103C8T6 Blue Pill
 * USART1 polled TX ("Hello STM32\r\n" every second) +
 * blocking RX with 1-second timeout, echo-back.
 *
 * INSTRUCTIONS:
 *   Copy each labelled section into the matching USER CODE BEGIN/END block
 *   inside the CubeMX-generated main.c.
 *
 * NOTE: Code is not tested on hardware.
 *       Verify handle name (huart1) matches your CubeMX output.
 */

// ===== USER CODE BEGIN Includes =====
#include <string.h>   /* strlen */
// ===== USER CODE END Includes =====


// ===== USER CODE BEGIN PD (Private Defines) =====
#define UART_RX_TIMEOUT_MS   1000U   /* blocking receive timeout */
// ===== USER CODE END PD =====


// ===== USER CODE BEGIN PV (Private Variables) =====
static const char hello_msg[] = "Hello STM32\r\n";
static uint8_t    rx_byte     = 0;
// ===== USER CODE END PV =====


// ===== USER CODE BEGIN 2 (after MX_xxx_Init calls, inside main()) =====
/*
 * Optionally send a startup banner so the terminal shows something
 * immediately on reset before the main loop begins.
 */
{
    const char *banner = "=== 07_uart_basics started ===\r\n";
    HAL_UART_Transmit(&huart1,
                      (uint8_t *)banner,
                      (uint16_t)strlen(banner),
                      HAL_MAX_DELAY);
}
// ===== USER CODE END 2 =====


// ===== USER CODE BEGIN WHILE (inside the infinite while(1) loop) =====
{
    /* --- TX: send hello message ---------------------------------------- */
    HAL_StatusTypeDef tx_status;
    tx_status = HAL_UART_Transmit(&huart1,
                                  (uint8_t *)hello_msg,
                                  (uint16_t)(sizeof(hello_msg) - 1U), /* exclude NUL */
                                  HAL_MAX_DELAY);
    if (tx_status != HAL_OK)
    {
        /* TX failed — toggle LED rapidly as error indication */
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        HAL_Delay(100);
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        HAL_Delay(100);
    }

    /* --- RX: wait up to 1 second for a byte, echo it back -------------- */
    HAL_StatusTypeDef rx_status;
    rx_status = HAL_UART_Receive(&huart1,
                                 &rx_byte,
                                 1U,                   /* number of bytes */
                                 UART_RX_TIMEOUT_MS);  /* 1 s timeout */

    if (rx_status == HAL_OK)
    {
        /* Echo the received byte back to the terminal */
        HAL_UART_Transmit(&huart1,
                          &rx_byte,
                          1U,
                          HAL_MAX_DELAY);
    }
    /* HAL_TIMEOUT is normal if no byte was received — just continue */

    /* --- Blink LED to show loop is alive -------------------------------- */
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

    /*
     * HAL_Delay(500) is NOT added here because HAL_UART_Receive already
     * blocks for up to UART_RX_TIMEOUT_MS (1000 ms) on each iteration.
     * Total loop period is ~1 s (TX time + RX timeout + negligible overhead).
     * If you shorten the RX timeout, add an explicit HAL_Delay to keep
     * the TX rate at approximately 1 Hz.
     */
}
// ===== USER CODE END WHILE =====
