// ============================================================
// Nucleo-F401RE BASIC/23 — DMA SPI Transfer
// STM32F401RETx  |  HAL + CubeMX 6.16
// SPI2 TX via DMA1 Stream4 — CPU free during transfer
// ============================================================

// ===== USER CODE BEGIN PD =====
#define SPI_CS_PIN   GPIO_PIN_12
#define SPI_CS_PORT  GPIOB
#define TRANSFER_LEN 256
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
static uint8_t tx_buf[TRANSFER_LEN];
static volatile uint8_t dma_done = 0;
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 2 =====
    // Populate buffer with pattern
    for (int i = 0; i < TRANSFER_LEN; i++) tx_buf[i] = (uint8_t)i;

    HAL_GPIO_WritePin(SPI_CS_PORT, SPI_CS_PIN, GPIO_PIN_SET); // CS idle high
    HAL_UART_Transmit(&huart2, (uint8_t*)"DMA SPI demo\r\n", 14, 100);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        dma_done = 0;
        HAL_GPIO_WritePin(SPI_CS_PORT, SPI_CS_PIN, GPIO_PIN_RESET); // CS low
        HAL_SPI_Transmit_DMA(&hspi2, tx_buf, TRANSFER_LEN); // non-blocking!

        // CPU is free during DMA — blink LED
        while (!dma_done) {
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
            HAL_Delay(50);
        }
        HAL_GPIO_WritePin(SPI_CS_PORT, SPI_CS_PIN, GPIO_PIN_SET); // CS high

        HAL_Delay(1000);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====

// ===== USER CODE BEGIN 4 =====
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI2) {
        dma_done = 1;
        HAL_UART_Transmit(&huart2, (uint8_t*)"DMA SPI TX done\r\n", 17, 100);
    }
}
// ===== USER CODE END 4 =====
