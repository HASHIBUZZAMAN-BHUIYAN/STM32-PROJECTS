// ============================================================
// Nucleo-F401RE BASIC/12 — SPI Basics (SPI2 loopback + MAX7219)
// STM32F401RETx  |  HAL + CubeMX 6.16
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
#define SPI_CS_PIN   GPIO_PIN_12
#define SPI_CS_PORT  GPIOB
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
static char uart_buf[80];
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static void spi_cs_low(void)  { HAL_GPIO_WritePin(SPI_CS_PORT, SPI_CS_PIN, GPIO_PIN_RESET); }
static void spi_cs_high(void) { HAL_GPIO_WritePin(SPI_CS_PORT, SPI_CS_PIN, GPIO_PIN_SET); }

/* MAX7219 register write (16-bit: address byte + data byte) */
static void max7219_write(uint8_t reg, uint8_t data)
{
    uint8_t tx[2] = {reg, data};
    spi_cs_low();
    HAL_SPI_Transmit(&hspi2, tx, 2, 10);
    spi_cs_high();
}

static void max7219_init(void)
{
    max7219_write(0x09, 0x00); // no decode
    max7219_write(0x0A, 0x0F); // max intensity
    max7219_write(0x0B, 0x07); // scan 8 digits
    max7219_write(0x0C, 0x01); // normal operation
    max7219_write(0x0F, 0x00); // display test off
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    spi_cs_high(); // deselect

    // --- Loopback Test ---
    HAL_UART_Transmit(&huart2, (uint8_t*)"SPI Loopback test:\r\n", 20, 100);
    uint8_t tx_buf[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x11};
    uint8_t rx_buf[8] = {0};

    HAL_SPI_TransmitReceive(&hspi2, tx_buf, rx_buf, 8, 100);

    int pass_count = 0;
    for (int i = 0; i < 8; i++) {
        if (tx_buf[i] == rx_buf[i]) pass_count++;
    }
    int n = snprintf(uart_buf, sizeof(uart_buf),
                     "  %s (%d/8 bytes match)\r\n",
                     pass_count == 8 ? "PASS" : "FAIL", pass_count);
    HAL_UART_Transmit(&huart2, (uint8_t*)uart_buf, n, 100);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    // MAX7219 pattern demo (remove loopback jumper first, connect MAX7219)
    // max7219_init();
    uint8_t pattern = 0x01;
    while (1)
    {
        // Shift pattern through MAX7219 rows (comment loopback, uncomment to use MAX7219)
        // for (uint8_t row = 1; row <= 8; row++) max7219_write(row, pattern);
        // pattern = (pattern == 0xFF) ? 0x01 : (pattern << 1) | 1;
        // HAL_Delay(200);

        // Loopback demo: periodically re-run test
        HAL_Delay(5000);
        HAL_SPI_TransmitReceive(&hspi2, tx_buf, rx_buf, 8, 100);
        n = snprintf(uart_buf, sizeof(uart_buf), "Periodic check: %s\r\n",
                     (memcmp(tx_buf, rx_buf, 8) == 0) ? "PASS" : "FAIL");
        HAL_UART_Transmit(&huart2, (uint8_t*)uart_buf, n, 100);
        (void)pattern;
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====
