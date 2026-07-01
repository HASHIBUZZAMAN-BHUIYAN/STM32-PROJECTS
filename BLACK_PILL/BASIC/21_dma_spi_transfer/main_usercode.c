// ============================================================
// Black Pill BASIC/21 — DMA SPI Transfer (W25Q32 Flash)
// STM32F411CEU6  |  HAL + CubeMX 6.16  |  100 MHz
// SPI1: PA5=SCK, PA6=MISO, PA7=MOSI, PA4=CS
// DMA2 Stream3(TX) + Stream0(RX), TIM2=1MHz
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
#define CS_LOW()  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET)
#define CS_HIGH() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET)

// W25Q32 commands
#define W25_READ_ID     0x9F
#define W25_WRITE_EN    0x06
#define W25_READ_STATUS 0x05
#define W25_READ        0x03
#define W25_PAGE_PROG   0x02
#define W25_SECTOR_ERASE 0x20
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
static uint8_t tx_buf[260];
static uint8_t rx_buf[260];
static volatile uint8_t spi_dma_done = 0;
static char buf[80];
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static void uart_tx(const char *s) {
    HAL_UART_Transmit(&huart1, (const uint8_t*)s, strlen(s), 200);
}

static void spi_transfer(uint8_t *tx, uint8_t *rx, uint16_t len) {
    spi_dma_done = 0;
    CS_LOW();
    HAL_SPI_TransmitReceive_DMA(&hspi1, tx, rx, len);
    while (!spi_dma_done);
    CS_HIGH();
}

// Single-byte command helper
static uint8_t spi_cmd1(uint8_t cmd) {
    uint8_t t[2] = {cmd, 0xFF}, r[2] = {0};
    spi_transfer(t, r, 2);
    return r[1];
}

static void w25_wait_busy(void) {
    uint8_t status;
    do {
        status = spi_cmd1(W25_READ_STATUS);
    } while (status & 0x01);
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi->Instance == SPI1) spi_dma_done = 1;
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    HAL_TIM_Base_Start(&htim2);
    CS_HIGH();
    HAL_Delay(100);

    uart_tx("SPI1 DMA transfer demo\r\n");

    // Read JEDEC ID
    memset(tx_buf, 0xFF, 4);
    tx_buf[0] = W25_READ_ID;
    spi_transfer(tx_buf, rx_buf, 4);
    int n = snprintf(buf, sizeof(buf), "JEDEC ID: %02X %02X %02X\r\n",
                     rx_buf[1], rx_buf[2], rx_buf[3]);
    uart_tx(buf);

    // Erase sector at address 0x000000
    tx_buf[0] = W25_WRITE_EN;
    CS_LOW(); HAL_SPI_Transmit(&hspi1, tx_buf, 1, 10); CS_HIGH();
    tx_buf[0] = W25_SECTOR_ERASE; tx_buf[1]=0; tx_buf[2]=0; tx_buf[3]=0;
    spi_transfer(tx_buf, rx_buf, 4);
    uart_tx("Erasing sector 0...\r\n");
    w25_wait_busy();
    uart_tx("Erase done\r\n");

    // Write 256-byte page at address 0
    tx_buf[0] = W25_WRITE_EN;
    CS_LOW(); HAL_SPI_Transmit(&hspi1, tx_buf, 1, 10); CS_HIGH();
    tx_buf[0]=W25_PAGE_PROG; tx_buf[1]=0; tx_buf[2]=0; tx_buf[3]=0;
    for (uint16_t i = 0; i < 256; i++) tx_buf[4+i] = (uint8_t)i;

    uint32_t t0 = __HAL_TIM_GET_COUNTER(&htim2);
    spi_transfer(tx_buf, rx_buf, 260);
    uint32_t tx_us = __HAL_TIM_GET_COUNTER(&htim2) - t0;
    w25_wait_busy();

    n = snprintf(buf, sizeof(buf), "Page write DMA: %lu us\r\n", tx_us);
    uart_tx(buf);

    // Read back and verify
    memset(tx_buf, 0xFF, 260);
    tx_buf[0]=W25_READ; tx_buf[1]=0; tx_buf[2]=0; tx_buf[3]=0;
    spi_transfer(tx_buf, rx_buf, 260);
    uint8_t ok = 1;
    for (int i = 0; i < 256; i++) if (rx_buf[4+i] != (uint8_t)i) { ok = 0; break; }
    uart_tx(ok ? "Verify: OK\r\n" : "Verify: FAIL\r\n");
    (void)n;
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        HAL_Delay(1000);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====
