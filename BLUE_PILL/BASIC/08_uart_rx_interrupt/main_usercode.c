/*
 * main_usercode.c — 08_uart_rx_interrupt
 *
 * STM32F103C8T6 Blue Pill
 * USART1 interrupt-driven receive, line-buffered.
 * HAL_UART_Receive_IT arms one byte at a time; callback accumulates
 * into a line buffer and signals main on '\n'.
 *
 * INSTRUCTIONS:
 *   Copy each labelled section into the matching USER CODE BEGIN/END block
 *   inside the CubeMX-generated main.c.
 *
 * NOTE: Code is not tested on hardware.
 *       Verify handle name (huart1) matches your CubeMX output.
 *       USART1 global interrupt MUST be enabled in CubeMX NVIC tab.
 */

// ===== USER CODE BEGIN Includes =====
#include <string.h>   /* strlen, memset */
#include <stdio.h>    /* snprintf */
// ===== USER CODE END Includes =====


// ===== USER CODE BEGIN PD (Private Defines) =====
#define LINE_BUF_LEN   64U
// ===== USER CODE END PD =====


// ===== USER CODE BEGIN PV (Private Variables) =====
/* Single-byte buffer that HAL_UART_Receive_IT writes each received byte into */
static volatile uint8_t  rx_byte = 0;

/* Line accumulation buffer (filled from ISR context) */
static volatile char     line_buf[LINE_BUF_LEN];
static volatile uint16_t line_idx  = 0;

/* Flag set by ISR, cleared by main loop — no atomic needed on Cortex-M3
   for a single byte, but volatile ensures the compiler re-reads it */
static volatile uint8_t  line_ready = 0;
// ===== USER CODE END PV =====


// ===== USER CODE BEGIN 2 (after MX_xxx_Init calls, inside main()) =====
{
    /* Send startup message */
    const char *banner = "=== 08_uart_rx_interrupt ready. Type a line + Enter ===\r\n";
    HAL_UART_Transmit(&huart1,
                      (uint8_t *)banner,
                      (uint16_t)strlen(banner),
                      HAL_MAX_DELAY);

    /* Arm the first interrupt-driven receive (1 byte at a time) */
    if (HAL_UART_Receive_IT(&huart1, (uint8_t *)&rx_byte, 1U) != HAL_OK)
    {
        Error_Handler();
    }
}
// ===== USER CODE END 2 =====


// ===== USER CODE BEGIN WHILE (inside the infinite while(1) loop) =====
{
    /* Check if a complete line has been received */
    if (line_ready)
    {
        line_ready = 0;   /* clear flag before processing */

        /* Build echo reply — line_buf already has the text, minus the '\n' */
        char reply[LINE_BUF_LEN + 16];
        int  reply_len = snprintf(reply, sizeof(reply),
                                  "ECHO: %s\r\n",
                                  (const char *)line_buf);

        if (reply_len > 0)
        {
            HAL_UART_Transmit(&huart1,
                              (uint8_t *)reply,
                              (uint16_t)reply_len,
                              HAL_MAX_DELAY);
        }

        /* Clear line buffer for next line */
        memset((void *)line_buf, 0, LINE_BUF_LEN);
        line_idx = 0;
    }

    /* Main loop is free to do other work — blink LED at 1 Hz */
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    HAL_Delay(500);
}
// ===== USER CODE END WHILE =====


// ===== USER CODE BEGIN 4 (callbacks, outside main()) =====

/**
 * @brief  UART RX complete callback — called by HAL from USART1 IRQ handler.
 *         Runs in interrupt context: keep it short, no blocking calls.
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        char c = (char)rx_byte;

        if (c == '\n')
        {
            /* Newline received — null-terminate and signal main loop */
            /* Strip trailing '\r' if present (Windows line endings) */
            if (line_idx > 0 && line_buf[line_idx - 1U] == '\r')
            {
                line_buf[line_idx - 1U] = '\0';
            }
            else if (line_idx < LINE_BUF_LEN)
            {
                line_buf[line_idx] = '\0';
            }
            line_ready = 1;
        }
        else if (c == '\r')
        {
            /* Ignore standalone carriage return — handled above if before '\n' */
            /* Store it so it can be stripped when '\n' arrives */
            if (line_idx < LINE_BUF_LEN - 1U)
            {
                line_buf[line_idx++] = c;
            }
        }
        else
        {
            /* Regular character — append to buffer */
            if (line_idx < LINE_BUF_LEN - 1U)
            {
                line_buf[line_idx++] = c;
            }
            /* else: buffer full, silently discard until newline */
        }

        /* Re-arm receive interrupt for the next byte — MUST be called
           in every callback invocation to keep receiving */
        HAL_UART_Receive_IT(&huart1, (uint8_t *)&rx_byte, 1U);
    }
}

// ===== USER CODE END 4 =====
