// ============================================================
// Black Pill BASIC/12 — SPI Basics (MAX7219 LED Matrix)
// STM32F411CEU6  |  HAL + CubeMX 6.16  |  100 MHz
// SPI1: PA5=SCK, PA6=MISO, PA7=MOSI, PA4=CS
// MAX7219 @ SPI mode 0, 16-bit frames
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <string.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
#define CS_LOW()  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET)
#define CS_HIGH() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET)

// MAX7219 register addresses
#define MAX_NOOP      0x00
#define MAX_DIG0      0x01
#define MAX_DECODEMODE 0x09
#define MAX_INTENSITY 0x0A
#define MAX_SCANLIMIT 0x0B
#define MAX_SHUTDOWN  0x0C
#define MAX_DISPLAYTEST 0x0F
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN 0 =====
static void max7219_send(uint8_t reg, uint8_t data) {
    uint8_t buf[2] = { reg, data };
    CS_LOW();
    HAL_SPI_Transmit(&hspi1, buf, 2, 10);
    CS_HIGH();
}

static void max7219_init(void) {
    CS_HIGH();
    HAL_Delay(10);
    max7219_send(MAX_DISPLAYTEST, 0x00); // normal operation
    max7219_send(MAX_SCANLIMIT,   0x07); // scan all 8 digits
    max7219_send(MAX_DECODEMODE,  0x00); // no BCD decode (raw bits)
    max7219_send(MAX_INTENSITY,   0x08); // brightness 8/15
    max7219_send(MAX_SHUTDOWN,    0x01); // normal operation (exit shutdown)
}

// Draw a single row (0-7) with given byte pattern
static void max7219_row(uint8_t row, uint8_t pattern) {
    max7219_send(MAX_DIG0 + row, pattern);
}

// Clear all rows
static void max7219_clear(void) {
    for (uint8_t r = 0; r < 8; r++) max7219_row(r, 0x00);
}

// Simple smiley face
static const uint8_t smiley[8] = {
    0b00111100,
    0b01000010,
    0b10100101,
    0b10000001,
    0b10100101,
    0b10011001,
    0b01000010,
    0b00111100,
};
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    max7219_init();
    max7219_clear();
    HAL_Delay(500);

    // Display smiley
    for (uint8_t r = 0; r < 8; r++) max7219_row(r, smiley[r]);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        // Scroll a single lit pixel across each row
        for (uint8_t row = 0; row < 8; row++) {
            for (uint8_t shift = 0; shift < 8; shift++) {
                max7219_clear();
                max7219_row(row, 1 << (7 - shift));
                HAL_Delay(80);
            }
        }
        // Show smiley again
        for (uint8_t r = 0; r < 8; r++) max7219_row(r, smiley[r]);
        HAL_Delay(2000);
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====
