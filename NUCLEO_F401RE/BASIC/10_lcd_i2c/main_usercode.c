// ============================================================
// Nucleo-F401RE BASIC/10 — 16x2 LCD via I2C Backpack (PCF8574)
// STM32F401RETx  |  HAL + CubeMX 6.16
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
// Change to 0x3F if 0x27 doesn't work (depends on PCF8574 variant)
#define LCD_ADDR  (0x27 << 1)
// PCF8574 pin mapping (common backpack wiring):
// P0=RS, P1=RW, P2=EN, P3=Backlight, P4=D4, P5=D5, P6=D6, P7=D7
#define LCD_RS   0x01
#define LCD_RW   0x02
#define LCD_EN   0x04
#define LCD_BL   0x08  // backlight
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
static uint8_t lcd_backlight = LCD_BL;
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static void lcd_write_byte(uint8_t data)
{
    HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, &data, 1, 10);
}

static void lcd_pulse_en(uint8_t data)
{
    lcd_write_byte(data | LCD_EN);
    HAL_Delay(1);
    lcd_write_byte(data & ~LCD_EN);
    HAL_Delay(1);
}

static void lcd_send_nibble(uint8_t nibble, uint8_t rs)
{
    uint8_t data = (nibble << 4) | lcd_backlight | rs;
    lcd_pulse_en(data);
}

static void lcd_send_byte(uint8_t byte, uint8_t rs)
{
    lcd_send_nibble(byte >> 4, rs);
    lcd_send_nibble(byte & 0x0F, rs);
}

static void lcd_cmd(uint8_t cmd)  { lcd_send_byte(cmd, 0); HAL_Delay(2); }
static void lcd_data(uint8_t ch)  { lcd_send_byte(ch, LCD_RS); }

static void lcd_init(void)
{
    HAL_Delay(50);
    // Initialize in 4-bit mode (sequence per HD44780 datasheet)
    lcd_send_nibble(0x03, 0); HAL_Delay(5);
    lcd_send_nibble(0x03, 0); HAL_Delay(1);
    lcd_send_nibble(0x03, 0); HAL_Delay(1);
    lcd_send_nibble(0x02, 0); HAL_Delay(1); // switch to 4-bit
    lcd_cmd(0x28); // 4-bit, 2 lines, 5x8 font
    lcd_cmd(0x0C); // display on, cursor off
    lcd_cmd(0x06); // entry mode: increment, no shift
    lcd_cmd(0x01); // clear display
    HAL_Delay(2);
}

static void lcd_set_cursor(uint8_t col, uint8_t row)
{
    uint8_t row_offsets[] = {0x00, 0x40};
    lcd_cmd(0x80 | (col + row_offsets[row & 1]));
}

static void lcd_print(const char *str)
{
    while (*str) lcd_data((uint8_t)*str++);
}

static void lcd_clear(void) { lcd_cmd(0x01); HAL_Delay(2); }
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    lcd_init();
    lcd_set_cursor(0, 0);
    lcd_print("Nucleo F401RE");
    lcd_set_cursor(0, 1);
    lcd_print("I2C LCD Ready!");
    HAL_Delay(2000);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    uint32_t counter = 0;
    char line[17];
    while (1)
    {
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print("Nucleo F401RE");
        lcd_set_cursor(0, 1);
        snprintf(line, sizeof(line), "Count: %lu", counter++);
        lcd_print(line);
        HAL_Delay(1000);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====
