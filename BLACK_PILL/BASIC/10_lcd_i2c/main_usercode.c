// ============================================================
// Black Pill BASIC/10 — 16x2 LCD via I2C (PCF8574 Backpack)
// STM32F411CEU6  |  HAL + CubeMX 6.16  |  100 MHz
// I2C1: PB6=SCL, PB7=SDA  |  LCD address: 0x27 (PCF8574)
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <string.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
#define LCD_ADDR       (0x27 << 1)  // PCF8574 I2C address shifted for HAL
#define LCD_BACKLIGHT  0x08         // P3 = backlight
#define LCD_EN         0x04         // P2 = Enable
#define LCD_RS         0x01         // P0 = Register Select
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN 0 =====
static void lcd_i2c_write(uint8_t data) {
    HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, &data, 1, 10);
}

static void lcd_pulse_en(uint8_t data) {
    lcd_i2c_write(data | LCD_EN);
    HAL_Delay(1);
    lcd_i2c_write(data & ~LCD_EN);
    HAL_Delay(1);
}

static void lcd_send_nibble(uint8_t nibble, uint8_t rs) {
    uint8_t data = (nibble << 4) | LCD_BACKLIGHT | (rs ? LCD_RS : 0);
    lcd_pulse_en(data);
}

static void lcd_cmd(uint8_t cmd) {
    lcd_send_nibble(cmd >> 4, 0);
    lcd_send_nibble(cmd & 0x0F, 0);
    HAL_Delay(2);
}

static void lcd_char(char c) {
    lcd_send_nibble((uint8_t)c >> 4, 1);
    lcd_send_nibble((uint8_t)c & 0x0F, 1);
}

static void lcd_string(const char *s) {
    while (*s) lcd_char(*s++);
}

static void lcd_set_cursor(uint8_t row, uint8_t col) {
    uint8_t addr = (row == 0) ? 0x00 : 0x40;
    lcd_cmd(0x80 | (addr + col));
}

static void lcd_init(void) {
    HAL_Delay(50);
    // Init in 4-bit mode (HD44780 datasheet sequence)
    lcd_i2c_write(LCD_BACKLIGHT);
    HAL_Delay(1000);

    // Send 0x03 three times (8-bit mode init) then switch to 4-bit
    lcd_send_nibble(0x03, 0); HAL_Delay(5);
    lcd_send_nibble(0x03, 0); HAL_Delay(1);
    lcd_send_nibble(0x03, 0); HAL_Delay(1);
    lcd_send_nibble(0x02, 0); // Set to 4-bit mode
    HAL_Delay(1);

    lcd_cmd(0x28); // 4-bit, 2 lines, 5x8 font
    lcd_cmd(0x08); // Display off
    lcd_cmd(0x01); // Clear display
    HAL_Delay(2);
    lcd_cmd(0x06); // Entry mode: cursor right, no shift
    lcd_cmd(0x0C); // Display on, no cursor
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    lcd_init();
    lcd_set_cursor(0, 0);
    lcd_string("Black Pill F411 ");
    lcd_set_cursor(1, 0);
    lcd_string("I2C LCD Test    ");
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        // Heartbeat: blink PC13 LED (active-LOW)
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        HAL_Delay(500);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====
