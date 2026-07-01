// ============================================================
// Black Pill BASIC/11 — SSD1306 OLED Display
// STM32F411CEU6  |  HAL + CubeMX 6.16  |  100 MHz
// I2C1: PB6=SCL, PB7=SDA  |  SSD1306 @ 0x3C
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <string.h>
#include <stdio.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
#define OLED_ADDR  (0x3C << 1)
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN 0 =====
static void oled_cmd(uint8_t c) {
    uint8_t d[2] = {0x00, c};
    HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDR, d, 2, 10);
}

static void oled_init(void) {
    HAL_Delay(100);
    const uint8_t cmds[] = {
        0xAE,0x20,0x00,0xB0,0xC8,0x00,0x10,0x40,
        0x81,0xCF,0xA1,0xA6,0xA8,0x3F,0xA4,0xD3,
        0x00,0xD5,0xF0,0xD9,0x22,0xDA,0x12,0xDB,
        0x20,0x8D,0x14,0xAF
    };
    for (uint8_t i = 0; i < sizeof(cmds); i++) oled_cmd(cmds[i]);
}

static void oled_set_pos(uint8_t page, uint8_t col) {
    oled_cmd(0xB0 | page);
    oled_cmd(0x00 | (col & 0x0F));
    oled_cmd(0x10 | (col >> 4));
}

static void oled_clear(void) {
    for (uint8_t p = 0; p < 8; p++) {
        oled_set_pos(p, 0);
        uint8_t blank[129]; blank[0] = 0x40;
        memset(blank + 1, 0, 128);
        HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDR, blank, 129, 50);
    }
}

// Minimal 5x8 ASCII font (space + digits only for demo)
static const uint8_t font[][5] = {
    {0x00,0x00,0x00,0x00,0x00}, // ' '
    {0x3E,0x51,0x49,0x45,0x3E}, // '0'
    {0x00,0x42,0x7F,0x40,0x00}, // '1'
    {0x42,0x61,0x51,0x49,0x46}, // '2'
    {0x21,0x41,0x45,0x4B,0x31}, // '3'
    {0x18,0x14,0x12,0x7F,0x10}, // '4'
    {0x27,0x45,0x45,0x45,0x39}, // '5'
    {0x3C,0x4A,0x49,0x49,0x30}, // '6'
    {0x01,0x71,0x09,0x05,0x03}, // '7'
    {0x36,0x49,0x49,0x49,0x36}, // '8'
    {0x06,0x49,0x49,0x29,0x1E}, // '9'
};

static void oled_char(char ch) {
    uint8_t idx = 0;
    if (ch >= '0' && ch <= '9') idx = 1 + (ch - '0');
    uint8_t d[6]; d[0] = 0x40;
    memcpy(d + 1, font[idx], 5);
    HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDR, d, 6, 10);
}

static void oled_string(const char *s) {
    while (*s) oled_char(*s++);
}

static void oled_text(uint8_t page, uint8_t col, const char *s) {
    oled_set_pos(page, col);
    oled_string(s);
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    oled_init();
    oled_clear();
    oled_text(0, 0, "Black Pill F411");
    oled_text(2, 0, "SSD1306 OLED");
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        char tick_str[20];
        snprintf(tick_str, sizeof(tick_str), "Tick: %lu ms", HAL_GetTick());
        oled_set_pos(4, 0);
        oled_string(tick_str);
        HAL_Delay(200);
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====
