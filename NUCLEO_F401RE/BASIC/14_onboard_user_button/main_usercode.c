// ============================================================
// Nucleo-F401RE BASIC/14 — Onboard B1 Button Multi-Mode
// STM32F401RETx  |  HAL + CubeMX 6.16
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <string.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
#define BTN_DOUBLE_MS   300   // max interval for double press
#define BTN_LONG_MS    1000   // min duration for long press
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
typedef enum { MODE_IDLE, MODE_TOGGLE, MODE_FAST_BLINK, MODE_OFF } LedMode_t;
static volatile LedMode_t led_mode = MODE_IDLE;

static volatile uint32_t press_time  = 0;
static volatile uint32_t last_press  = 0;
static volatile uint8_t  press_count = 0;
static volatile uint8_t  btn_pressed = 0;
static volatile uint8_t  btn_released= 0;
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static void send_str(const char *s)
{
    HAL_UART_Transmit(&huart2, (const uint8_t*)s, strlen(s), 100);
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    send_str("B1 Button demo: single/double/long press\r\n");
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        if (btn_released) {
            btn_released = 0;
            uint32_t hold = HAL_GetTick() - press_time;
            if (hold >= BTN_LONG_MS) {
                led_mode = MODE_OFF;
                press_count = 0;
                send_str("Long press -> LED off\r\n");
            } else {
                press_count++;
                if (press_count == 1) {
                    last_press = HAL_GetTick();
                } else if (press_count >= 2) {
                    led_mode = MODE_FAST_BLINK;
                    press_count = 0;
                    send_str("Double press -> Fast blink\r\n");
                }
            }
        }

        // Timeout single press detection
        if (press_count == 1 && (HAL_GetTick() - last_press) > BTN_DOUBLE_MS) {
            press_count = 0;
            led_mode = MODE_TOGGLE;
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
            send_str("Single press -> LED toggled\r\n");
        }

        // Execute LED mode
        switch (led_mode) {
            case MODE_FAST_BLINK:
                HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
                HAL_Delay(100);
                break;
            case MODE_OFF:
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
                HAL_Delay(10);
                break;
            default:
                HAL_Delay(10);
                break;
        }
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====

// ===== USER CODE BEGIN 4 =====
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_13) {  // B1
        if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_RESET) {
            // Button pressed (falling edge)
            press_time = HAL_GetTick();
            btn_pressed = 1;
        } else {
            // Button released (rising edge — if configured both edges)
            btn_released = 1;
        }
    }
}
// ===== USER CODE END 4 =====
