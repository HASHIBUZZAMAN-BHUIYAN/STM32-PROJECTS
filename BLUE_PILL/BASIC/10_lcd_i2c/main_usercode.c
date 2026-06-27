/*
 * main_usercode.c — 10_lcd_i2c
 *
 * STM32F103C8T6 Blue Pill
 * 16x2 HD44780 LCD via PCF8574 I2C backpack.
 * Full driver implemented in USER CODE — no external library.
 *
 * Standard PCF8574 backpack pin mapping assumed:
 *   P0=RS, P1=RW, P2=EN, P3=BL, P4=D4, P5=D5, P6=D6, P7=D7
 *
 * INSTRUCTIONS:
 *   Copy each labelled section into the matching USER CODE BEGIN/END block
 *   inside the CubeMX-generated main.c.
 *
 * NOTE: Code is not tested on hardware.
 *       Verify handle name (hi2c1) matches your CubeMX output.
 *       Set LCD_I2C_ADDR below to match your PCF8574 module (0x27 or 0x3F).
 */

// ===== USER CODE BEGIN Includes =====
#include <string.h>    /* strlen */
// ===== USER CODE END Includes =====


// ===== USER CODE BEGIN PD (Private Defines) =====

/* I2C address of PCF8574 backpack — change to 0x3F if your module uses PCF8574A */
#define LCD_I2C_ADDR    (0x27U << 1U)   /* HAL uses 8-bit shifted address */

/* PCF8574 bit positions for LCD control lines */
#define LCD_BIT_RS   0x01U   /* P0 → Register Select (0=cmd, 1=data) */
#define LCD_BIT_RW   0x02U   /* P1 → Read/Write      (always 0=write) */
#define LCD_BIT_EN   0x04U   /* P2 → Enable strobe */
#define LCD_BIT_BL   0x08U   /* P3 → Backlight (1=on) */
/* Data nibble occupies P4..P7 (upper nibble of the byte sent to PCF8574) */

/* LCD commands */
#define LCD_CMD_CLEAR_DISPLAY  0x01U
#define LCD_CMD_RETURN_HOME    0x02U
#define LCD_CMD_ENTRY_MODE     0x06U   /* increment cursor, no display shift */
#define LCD_CMD_DISPLAY_ON     0x0CU   /* display on, cursor off, blink off */
#define LCD_CMD_FUNCTION_SET   0x28U   /* 4-bit, 2 lines, 5x8 font */
#define LCD_CMD_DDRAM_LINE0    0x80U   /* DDRAM address for row 0, col 0 */
#define LCD_CMD_DDRAM_LINE1    0xC0U   /* DDRAM address for row 1, col 0 */

/* I2C transmit timeout */
#define LCD_I2C_TIMEOUT_MS     10U

// ===== USER CODE END PD =====


// ===== USER CODE BEGIN PFP (Private Function Prototypes) =====
static void lcd_write_nibble(uint8_t nibble, uint8_t rs);
static void lcd_send_byte(uint8_t byte, uint8_t rs);
static void lcd_send_cmd(uint8_t cmd);
static void lcd_send_data(uint8_t data);
static void lcd_init(void);
static void lcd_set_cursor(uint8_t row, uint8_t col);
static void lcd_print(const char *str);
// ===== USER CODE END PFP =====


// ===== USER CODE BEGIN 2 (after MX_xxx_Init calls, inside main()) =====
{
    /* Wait > 40 ms after LCD power-on before initialising */
    HAL_Delay(50);

    lcd_init();

    lcd_set_cursor(0, 0);
    lcd_print("STM32 Blue Pill");

    lcd_set_cursor(1, 0);
    lcd_print("I2C LCD Test");
}
// ===== USER CODE END 2 =====


// ===== USER CODE BEGIN WHILE (inside the infinite while(1) loop) =====
{
    /* Blink onboard LED at 1 Hz to show firmware is running */
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    HAL_Delay(500);
}
// ===== USER CODE END WHILE =====


// ===== USER CODE BEGIN 4 (driver implementation, outside main()) =====

/**
 * @brief  Send one 4-bit nibble to the LCD via PCF8574.
 *
 *         The PCF8574 output byte format (standard backpack):
 *           Bit 7 6 5 4  3   2   1   0
 *                D7D6D5D4 BL  EN  RW  RS
 *
 *         To clock data into the LCD, toggle EN high then low
 *         with the data stable around it.
 *
 * @param  nibble  Upper nibble of the byte to send (bits [7:4] used as D7..D4)
 * @param  rs      Register select: 0 = command, 1 = character data
 */
static void lcd_write_nibble(uint8_t nibble, uint8_t rs)
{
    /* Pack nibble into upper 4 bits of the PCF8574 output byte */
    uint8_t data = (nibble & 0xF0U)       /* D7..D4 in upper nibble */
                 | LCD_BIT_BL              /* backlight on */
                 | (rs ? LCD_BIT_RS : 0U);/* RS bit */
    /* RW is always 0 (write), so LCD_BIT_RW is not ORed in */

    uint8_t buf[2];

    /* Pulse EN high */
    buf[0] = data | LCD_BIT_EN;
    HAL_I2C_Master_Transmit(&hi2c1, LCD_I2C_ADDR, buf, 1U, LCD_I2C_TIMEOUT_MS);
    HAL_Delay(1);   /* EN pulse width ≥ 450 ns; 1 ms is well above minimum */

    /* Pull EN low — LCD latches data on falling edge */
    buf[0] = data & ~LCD_BIT_EN;
    HAL_I2C_Master_Transmit(&hi2c1, LCD_I2C_ADDR, buf, 1U, LCD_I2C_TIMEOUT_MS);
    HAL_Delay(1);
}

/**
 * @brief  Send a full byte to the LCD in two 4-bit nibbles (high then low).
 * @param  byte  The command or data byte to send.
 * @param  rs    Register select: 0 = command, 1 = character data.
 */
static void lcd_send_byte(uint8_t byte, uint8_t rs)
{
    lcd_write_nibble(byte & 0xF0U,         rs);   /* high nibble first */
    lcd_write_nibble((byte << 4U) & 0xF0U, rs);   /* low nibble second */
}

/**
 * @brief  Send a command byte (RS=0).
 */
static void lcd_send_cmd(uint8_t cmd)
{
    lcd_send_byte(cmd, 0U);
    /* Extra delay for slow commands (clear display, return home) */
    if (cmd == LCD_CMD_CLEAR_DISPLAY || cmd == LCD_CMD_RETURN_HOME)
    {
        HAL_Delay(2);   /* these commands take up to 1.52 ms */
    }
}

/**
 * @brief  Send a character data byte (RS=1).
 */
static void lcd_send_data(uint8_t data)
{
    lcd_send_byte(data, 1U);
}

/**
 * @brief  Initialise the LCD in 4-bit mode.
 *
 *         Follows the HD44780 initialisation sequence for 4-bit interface
 *         as specified in the datasheet (Figure 24).
 */
static void lcd_init(void)
{
    /*
     * Special 4-bit init sequence:
     * Send 0x3 three times in nibble form, then 0x2 to switch to 4-bit.
     * This handles the case where the LCD is in an unknown state at power-on.
     */
    lcd_write_nibble(0x30U, 0U);   /* Function Set (8-bit attempt 1) */
    HAL_Delay(5);
    lcd_write_nibble(0x30U, 0U);   /* Function Set (8-bit attempt 2) */
    HAL_Delay(1);
    lcd_write_nibble(0x30U, 0U);   /* Function Set (8-bit attempt 3) */
    HAL_Delay(1);
    lcd_write_nibble(0x20U, 0U);   /* Switch to 4-bit mode */
    HAL_Delay(1);

    /* Now in 4-bit mode — send full bytes as two nibbles */
    lcd_send_cmd(LCD_CMD_FUNCTION_SET);   /* 4-bit, 2 lines, 5x8 */
    lcd_send_cmd(LCD_CMD_DISPLAY_ON);     /* display on, cursor off */
    lcd_send_cmd(LCD_CMD_CLEAR_DISPLAY);  /* clear screen */
    lcd_send_cmd(LCD_CMD_ENTRY_MODE);     /* cursor increments right */
}

/**
 * @brief  Move cursor to the specified row and column.
 * @param  row  0 for line 1, 1 for line 2.
 * @param  col  0–15 for a 16-character display.
 */
static void lcd_set_cursor(uint8_t row, uint8_t col)
{
    uint8_t ddram_addr;
    if (row == 0U)
    {
        ddram_addr = LCD_CMD_DDRAM_LINE0 + col;
    }
    else
    {
        ddram_addr = LCD_CMD_DDRAM_LINE1 + col;
    }
    lcd_send_cmd(ddram_addr);
}

/**
 * @brief  Print a null-terminated ASCII string at the current cursor position.
 * @param  str  Pointer to string. Up to 16 characters per line.
 */
static void lcd_print(const char *str)
{
    while (*str != '\0')
    {
        lcd_send_data((uint8_t)*str);
        str++;
    }
}

// ===== USER CODE END 4 =====
