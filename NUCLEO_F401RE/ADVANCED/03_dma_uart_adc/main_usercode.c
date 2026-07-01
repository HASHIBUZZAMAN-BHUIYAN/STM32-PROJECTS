// ============================================================
// Nucleo-F401RE ADVANCED/03 — DMA: UART TX + ADC Circular
// STM32F401RETx  |  HAL + CubeMX 6.16
// ADC1 DMA circular (128 samples, 1kHz), UART DMA TX
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
#define ADC_BUF_SIZE 128
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
static uint16_t adc_buf[ADC_BUF_SIZE];  // DMA fills this in circular mode
static volatile uint8_t  half_ready = 0; // set by HalfCplt callback
static volatile uint8_t  full_ready = 0; // set by CpltCallback
static char tx_buf[64];
static volatile uint8_t  uart_dma_busy = 0;
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static uint32_t compute_avg(uint16_t *data, uint32_t n) {
    uint32_t sum = 0;
    for (uint32_t i = 0; i < n; i++) sum += data[i];
    return sum / n;
}

static void send_uart_dma(uint32_t avg) {
    if (uart_dma_busy) return; // previous TX still in progress
    uart_dma_busy = 1;
    // Voltage: avg * 3300mV / 4096
    uint32_t mv = (avg * 3300U) / 4096U;
    int n = snprintf(tx_buf, sizeof(tx_buf),
                     "ADC avg=%lu (%lu.%02luV) tick=%lu\r\n",
                     avg, mv/1000, (mv%1000)/10, HAL_GetTick());
    HAL_UART_Transmit_DMA(&huart2, (uint8_t*)tx_buf, n);
}

/* ADC DMA half-complete: first 64 samples ready */
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc) {
    if (hadc->Instance == ADC1) half_ready = 1;
}

/* ADC DMA full-complete: second 64 samples ready (also wraps to beginning) */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    if (hadc->Instance == ADC1) full_ready = 1;
}

/* UART DMA TX complete — clear busy flag */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) uart_dma_busy = 0;
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    HAL_TIM_Base_Start(&htim2);
    // Start ADC in DMA circular mode — TIM2 TRGO triggers each conversion
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buf, ADC_BUF_SIZE);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        if (half_ready) {
            half_ready = 0;
            // Process first half: adc_buf[0..63]
            uint32_t avg = compute_avg(adc_buf, ADC_BUF_SIZE / 2);
            send_uart_dma(avg);
        }
        if (full_ready) {
            full_ready = 0;
            // Process second half: adc_buf[64..127]
            uint32_t avg = compute_avg(adc_buf + ADC_BUF_SIZE/2, ADC_BUF_SIZE/2);
            send_uart_dma(avg);
        }
        // CPU can do other work here — not burning cycles polling ADC or UART
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        HAL_Delay(50);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====
