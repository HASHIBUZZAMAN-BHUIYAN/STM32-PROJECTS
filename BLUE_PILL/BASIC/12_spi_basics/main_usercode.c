/**
 * @file    main_usercode.c
 * @brief   SPI1 Full-Duplex Master — loopback test + generic device pattern.
 *
 * Paste each labelled block into the corresponding USER CODE BEGIN/END region
 * inside the CubeMX-generated main.c.
 *
 * CubeMX config required:
 *   - SPI1 Full-Duplex Master, 8-bit, Motorola frame, CPOL=Low, CPHA=1Edge
 *   - PA5 SCK, PA6 MISO, PA7 MOSI  (all Alternate Function Push-Pull)
 *   - PA4 GPIO_Output, default HIGH (software NSS / CS)
 *   - USART1 115200 8N1 (optional — for pass/fail print)
 *   - SysTick as HAL timebase
 *
 * LOOPBACK: jumper PA6 (MISO) → PA7 (MOSI) for the self-test.
 *
 * NOT TESTED ON HARDWARE — reference template only.
 */

/* ===== USER CODE BEGIN Includes ===== */
#include <string.h>
#include <stdio.h>
#include <stdint.h>
/* ===== USER CODE END Includes ===== */


/* ===== USER CODE BEGIN PD (Private Defines) ===== */
#define SPI_CS_PORT      GPIOA
#define SPI_CS_PIN       GPIO_PIN_4
#define SPI_BUF_LEN      16
#define SPI_TIMEOUT_MS   5
/* ===== USER CODE END PD ===== */


/* ===== USER CODE BEGIN PV (Private Variables) ===== */
static uint8_t spi_tx_buf[SPI_BUF_LEN];
static uint8_t spi_rx_buf[SPI_BUF_LEN];
/* ===== USER CODE END PV ===== */


/* ===== USER CODE BEGIN 0 (Private function prototypes & implementations) ===== */

extern SPI_HandleTypeDef  hspi1;
extern UART_HandleTypeDef huart1;

/** Assert CS (active low) */
static inline void SPI_CS_Assert(void)
{
    HAL_GPIO_WritePin(SPI_CS_PORT, SPI_CS_PIN, GPIO_PIN_RESET);
}

/** De-assert CS */
static inline void SPI_CS_Deassert(void)
{
    HAL_GPIO_WritePin(SPI_CS_PORT, SPI_CS_PIN, GPIO_PIN_SET);
}

/**
 * @brief Transmit and receive len bytes over SPI1.
 * @return HAL_OK on success.
 */
static HAL_StatusTypeDef SPI_TransferBytes(uint8_t *tx, uint8_t *rx,
                                            uint16_t len)
{
    SPI_CS_Assert();
    HAL_StatusTypeDef ret = HAL_SPI_TransmitReceive(&hspi1, tx, rx,
                                                     len, SPI_TIMEOUT_MS);
    SPI_CS_Deassert();
    return ret;
}

/**
 * @brief Write a single register on a generic SPI device.
 *
 *   Frame format (common for many sensors/ICs):
 *     Byte 0: register address (bit7=0 → write)
 *     Byte 1: value
 *
 * @param reg   Register address (7-bit, write bit = 0).
 * @param value Value to write.
 * @return HAL_OK on success.
 */
static HAL_StatusTypeDef SPI_WriteRegister(uint8_t reg, uint8_t value)
{
    uint8_t tx[2] = { reg & 0x7F, value };  /* clear bit7 = write */
    uint8_t rx[2] = { 0 };
    return SPI_TransferBytes(tx, rx, 2);
}

/**
 * @brief Read a single register from a generic SPI device.
 *
 *   Frame format:
 *     Byte 0: register address (bit7=1 → read)
 *     Byte 1: dummy TX, actual data returned in rx[1]
 *
 * @param reg    Register address (7-bit).
 * @param out    Pointer to store received byte.
 * @return HAL_OK on success.
 */
static HAL_StatusTypeDef SPI_ReadRegister(uint8_t reg, uint8_t *out)
{
    uint8_t tx[2] = { reg | 0x80, 0x00 };   /* set bit7 = read */
    uint8_t rx[2] = { 0 };
    HAL_StatusTypeDef ret = SPI_TransferBytes(tx, rx, 2);
    if (ret == HAL_OK) *out = rx[1];
    return ret;
}

/**
 * @brief Run the loopback self-test.
 *        Sends 0x00..0x0F, expects to receive the same bytes back.
 * @return 1 if all bytes match, 0 on mismatch or SPI error.
 */
static int SPI_LoopbackTest(void)
{
    /* Build TX pattern */
    for (uint8_t i = 0; i < SPI_BUF_LEN; i++) {
        spi_tx_buf[i] = i;
    }
    memset(spi_rx_buf, 0, SPI_BUF_LEN);

    HAL_StatusTypeDef ret = SPI_TransferBytes(spi_tx_buf, spi_rx_buf,
                                              SPI_BUF_LEN);
    if (ret != HAL_OK) return 0;

    return (memcmp(spi_tx_buf, spi_rx_buf, SPI_BUF_LEN) == 0) ? 1 : 0;
}

/* ===== USER CODE END 0 ===== */


/* ===== USER CODE BEGIN 2 (inside main(), after MX_SPI1_Init()) ===== */
/*
    char msg[64];
    int  loopback_ok;

    // Ensure CS is de-asserted at start
    SPI_CS_Deassert();
    HAL_Delay(10);

    // --- LOOPBACK TEST ---
    // Requires PA6 (MISO) jumpered to PA7 (MOSI).
    loopback_ok = SPI_LoopbackTest();

    if (loopback_ok)
    {
        // PASS: slow LED blink
        snprintf(msg, sizeof(msg), "SPI loopback PASS\r\n");
    }
    else
    {
        // FAIL: fast LED blink
        snprintf(msg, sizeof(msg), "SPI loopback FAIL\r\n");
    }
    HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), 100);

    // --- GENERIC SPI DEVICE EXAMPLE ---
    // (Remove loopback jumper before connecting a real device.)
    // Write 0x42 to register 0x10:
    // HAL_StatusTypeDef dev_ret = SPI_WriteRegister(0x10, 0x42);
    // Read back register 0x10:
    // uint8_t val = 0;
    // dev_ret = SPI_ReadRegister(0x10, &val);
*/
/* ===== USER CODE END 2 ===== */


/* ===== USER CODE BEGIN WHILE (main loop) ===== */
/*
    while (1)
    {
        if (loopback_ok)
        {
            // PASS: slow blink (1 Hz)
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
            HAL_Delay(500);
        }
        else
        {
            // FAIL: fast blink (10 Hz)
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
            HAL_Delay(100);
        }
    }
*/
/* ===== USER CODE END WHILE ===== */
