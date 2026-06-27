/*
 * main_usercode.c — 06_adc_dma
 *
 * STM32F103C8T6 Blue Pill
 * ADC1 Channel 1 (PA1) with DMA circular mode, 16-sample buffer.
 *
 * INSTRUCTIONS:
 *   Copy each labelled section into the matching USER CODE BEGIN/END block
 *   inside the CubeMX-generated main.c. Do NOT replace the generated
 *   peripheral init calls (MX_ADC1_Init, MX_DMA_Init, etc.).
 *
 * NOTE: Code is not tested on hardware.
 *       Verify handle names (hadc1, hdma_adc1) match your CubeMX output.
 */

// ===== USER CODE BEGIN Includes =====
#include <stdint.h>
// ===== USER CODE END Includes =====


// ===== USER CODE BEGIN PD (Private Defines) =====
#define ADC_BUF_LEN  16U
// ===== USER CODE END PD =====


// ===== USER CODE BEGIN PV (Private Variables) =====
/* DMA destination buffer — must be global (not stack) so DMA can reach it */
static volatile uint32_t adc_dma_buf[ADC_BUF_LEN];
static volatile uint32_t adc_average = 0;
// ===== USER CODE END PV =====


// ===== USER CODE BEGIN 2 (after all MX_xxx_Init calls, inside main()) =====
/*
 * Start ADC in DMA mode.
 * HAL_ADC_Start_DMA kicks off continuous conversions; DMA fills the buffer
 * in circular fashion — no CPU involvement per sample.
 */
if (HAL_ADC_Start_DMA(&hadc1,
                       (uint32_t *)adc_dma_buf,
                       ADC_BUF_LEN) != HAL_OK)
{
    /* Initialization failed — trap here or handle gracefully */
    Error_Handler();
}
// ===== USER CODE END 2 =====


// ===== USER CODE BEGIN WHILE (inside the infinite while(1) loop) =====
{
    /* Compute average of DMA buffer — buffer is updated by DMA in background */
    uint32_t sum = 0;
    for (uint32_t i = 0; i < ADC_BUF_LEN; i++)
    {
        sum += adc_dma_buf[i];
    }
    adc_average = sum / ADC_BUF_LEN;   /* 0–4095, 12-bit result */

    /*
     * Blink onboard LED (PC13, active-low) to show the loop is running.
     * Replace with UART transmit (project 07) to observe adc_average value.
     */
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    HAL_Delay(500);   /* 500 ms → 1 Hz blink */
}
// ===== USER CODE END WHILE =====


/*
 * -----------------------------------------------------------------------
 * Optional DMA half/full transfer callbacks (place outside main(), in the
 * USER CODE BEGIN 4 section or a separate source file).
 *
 * HAL calls these automatically; no manual ISR registration is needed.
 * -----------------------------------------------------------------------
 */

// ===== USER CODE BEGIN 4 =====

/**
 * @brief  DMA half-transfer complete callback.
 *         Called when the first half of adc_dma_buf is filled.
 *         Process the first half here if double-buffering is needed.
 */
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1)
    {
        /* Optional: process first half of buffer (indices 0 .. ADC_BUF_LEN/2-1) */
        (void)hadc;  /* suppress unused-parameter warning if not used */
    }
}

/**
 * @brief  DMA full-transfer complete callback.
 *         Called when all ADC_BUF_LEN samples are filled.
 *         In circular mode DMA immediately wraps to index 0 — safe to read here.
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1)
    {
        /* Optional: signal main loop via flag, or compute average here */
        (void)hadc;
    }
}

// ===== USER CODE END 4 =====
