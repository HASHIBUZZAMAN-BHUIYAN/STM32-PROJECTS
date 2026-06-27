/*
 * main_usercode.c — Project 26: DMA UART TX
 * Target: STM32F103C8T6 (Blue Pill)
 *
 * Paste each labeled block into the matching USER CODE section of the
 * CubeMX-generated main.c. Do NOT replace the generated init calls.
 *
 * CubeMX requirements:
 *   - USART1 Asynchronous 115200 8N1, DMA TX on DMA1 Channel 4 (Normal mode)
 *   - ADC1 Channel 0 (PA0), software trigger, 12-bit
 *   - SysTick for HAL_Delay
 *
 * NOTE: This code has not been tested on hardware.
 */

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====


// ===== USER CODE BEGIN PD (Private Defines) =====
#define TX_BUF_SIZE     256U
#define ADC_SAMPLES     10U
#define ADC_VREF_MV     3300U   /* 3.3 V reference */
#define ADC_MAX_COUNT   4095U   /* 12-bit */
// ===== USER CODE END PD =====


// ===== USER CODE BEGIN PV (Private Variables) =====
/*
 * Declare huart1, hadc1 as extern here if they are defined in main.c.
 * CubeMX places them in main.c; the extern declarations below allow this
 * file to reference them when compiled separately. In the merged main.c
 * these externs are not needed — remove them if you paste into main.c directly.
 */
extern UART_HandleTypeDef huart1;
extern ADC_HandleTypeDef  hadc1;

static char     tx_buf[TX_BUF_SIZE];
static uint8_t  dma_tx_busy = 0;
// ===== USER CODE END PV =====


// ===== USER CODE BEGIN 0 (Private Function Prototypes & Helpers) =====

/**
 * @brief  Read ADC1 Ch0 multiple times and return the integer average.
 * @param  samples  Number of conversions to average.
 * @return Averaged 12-bit ADC raw value, or 0 on HAL error.
 */
static uint32_t adc_read_average(uint32_t samples)
{
    uint32_t accumulator = 0;

    for (uint32_t i = 0; i < samples; i++)
    {
        if (HAL_ADC_Start(&hadc1) != HAL_OK)
        {
            return 0;
        }
        if (HAL_ADC_PollForConversion(&hadc1, 10) != HAL_OK)
        {
            HAL_ADC_Stop(&hadc1);
            return 0;
        }
        accumulator += HAL_ADC_GetValue(&hadc1);
        HAL_ADC_Stop(&hadc1);
    }

    return accumulator / samples;
}

/**
 * @brief  Wait until the UART DMA TX is no longer busy, then start a new
 *         DMA transfer.  Blocks until the previous transfer finishes so we
 *         never overwrite tx_buf mid-flight.
 *
 * @param  len  Number of bytes in tx_buf to transmit.
 */
static void uart_dma_send(uint16_t len)
{
    /* Poll until the previous transfer completes.
     * HAL_UART_STATE_READY means no ongoing TX or RX operation. */
    while (HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY)
    {
        /* Yield — in a bare-metal loop a short delay prevents hammering the bus */
        HAL_Delay(1);
    }

    dma_tx_busy = 1;

    if (HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_buf, len) != HAL_OK)
    {
        /* DMA start failed — reset and try next cycle */
        HAL_UART_AbortTransmit(&huart1);
        dma_tx_busy = 0;
    }
}

/* Optional: clear dma_tx_busy flag in the TX-complete callback */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        dma_tx_busy = 0;
    }
}

// ===== USER CODE END 0 =====


// ===== USER CODE BEGIN 2 (after MX_xxx_Init calls, inside main()) =====
/*
 * No extra init required — CubeMX handles USART1, DMA1, and ADC1 init.
 * Optionally send a startup banner:
 */
{
    const char *banner = "=== Project 26: DMA UART TX ===\r\n";
    /* Use blocking TX for the banner so we know UART is working */
    HAL_UART_Transmit(&huart1, (uint8_t *)banner, (uint16_t)strlen(banner), 100);
}
// ===== USER CODE END 2 =====


// ===== USER CODE BEGIN WHILE (inside the while(1) loop) =====
{
    uint32_t raw_adc = adc_read_average(ADC_SAMPLES);

    /* Convert raw count to millivolts: mv = (raw * VREF) / MAX_COUNT */
    uint32_t mv = (raw_adc * ADC_VREF_MV) / ADC_MAX_COUNT;

    /* Format the output line into the shared TX buffer */
    int len = snprintf(tx_buf, TX_BUF_SIZE,
                       "ADC1_CH0: %4lu raw  (%4lu mV)\r\n",
                       (unsigned long)raw_adc,
                       (unsigned long)mv);

    if (len > 0 && len < TX_BUF_SIZE)
    {
        uart_dma_send((uint16_t)len);
    }

    /* 500 ms between samples — DMA transfer typically finishes long before this */
    HAL_Delay(500);
}
// ===== USER CODE END WHILE =====
