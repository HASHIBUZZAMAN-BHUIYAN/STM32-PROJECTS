// ============================================================
// Nucleo-F401RE BASIC/11 — SSD1306 OLED via I2C
// STM32F401RETx  |  HAL + CubeMX 6.16
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <string.h>
#include <stdio.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
#define OLED_ADDR  (0x3C << 1)
#define OLED_CMD   0x00
#define OLED_DATA  0x40
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN 0 =====
/* Minimal 5x7 font — digits + uppercase letters subset */
static const uint8_t font5x7[][5] = {
    {0x3E,0x51,0x49,0x45,0x3E}, // 0x30 '0'
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

static void oled_send(uint8_t ctrl, uint8_t *data, uint16_t len)
{
    uint8_t buf[2];
    for (uint16_t i = 0; i < len; i++) {
        buf[0] = ctrl; buf[1] = data[i];
        HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDR, buf, 2, 10);
    }
}

static void oled_cmd(uint8_t c)  { oled_send(OLED_CMD,  &c, 1); }
static void oled_byte(uint8_t b) { oled_send(OLED_DATA, &b, 1); }

static void oled_init(void)
{
    const uint8_t init[] = {
        0xAE,           // display off
        0xD5, 0x80,     // clock divide
        0xA8, 0x3F,     // mux ratio
        0xD3, 0x00,     // display offset
        0x40,           // start line 0
        0x8D, 0x14,     // charge pump on
        0x20, 0x00,     // horizontal addressing
        0xA1,           // segment remap
        0xC8,           // COM output scan direction
        0xDA, 0x12,     // COM pins
        0x81, 0xCF,     // contrast
        0xD9, 0xF1,     // pre-charge
        0xDB, 0x40,     // VCOMH
        0xA4,           // entire display on (RAM content)
        0xA6,           // normal display
        0xAF,           // display on
    };
    for (uint8_t i = 0; i < sizeof(init); i++) oled_cmd(init[i]);
}

static void oled_clear(void)
{
    oled_cmd(0x21); oled_cmd(0); oled_cmd(127);
    oled_cmd(0x22); oled_cmd(0); oled_cmd(7);
    for (int i = 0; i < 1024; i++) oled_byte(0x00);
}

static void oled_set_pos(uint8_t col, uint8_t page)
{
    oled_cmd(0xB0 | (page & 0x07));
    oled_cmd(0x00 | (col & 0x0F));
    oled_cmd(0x10 | (col >> 4));
}

static void oled_char(uint8_t c)
{
    uint8_t idx = (c >= '0' && c <= '9') ? (c - '0') : 0;
    for (int i = 0; i < 5; i++) oled_byte(font5x7[idx][i]);
    oled_byte(0x00); // spacing
}

/* Write ASCII string at current position — digits rendered with font */
static void oled_string(const char *str)
{
    while (*str) { oled_char((uint8_t)*str++); }
}

/* Write a simple text row using the font (limited char set in this demo) */
static void oled_text_row(uint8_t page, const char *str)
{
    oled_set_pos(0, page);
    oled_string(str);
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    oled_init();
    oled_clear();
    oled_text_row(0, "Hello Nucleo");
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    uint32_t tick = 0;
    char line[12];
    while (1)
    {
        snprintf(line, sizeof(line), "Tick:%lu", tick++);
        oled_text_row(2, line);
        HAL_Delay(1000);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====
