// ============================================================
// Nucleo-F401RE BASIC/27 — Traffic Light State Machine
// STM32F401RETx  |  HAL + CubeMX 6.16
// ============================================================

// ===== USER CODE BEGIN PD =====
#define LED_RED    GPIO_PIN_5  // PB5
#define LED_YEL    GPIO_PIN_4  // PB4
#define LED_GRN    GPIO_PIN_3  // PB3
#define LED_PORT   GPIOB
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
typedef enum {
    ST_RED, ST_RED_YEL, ST_GREEN, ST_YELLOW, ST_EMERGENCY
} TrafficState_t;

static TrafficState_t state = ST_RED;
static volatile uint8_t emergency = 0;
static uint32_t state_enter_ms = 0;
static uint32_t emrg_count = 0;
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static void leds_set(uint8_t r, uint8_t y, uint8_t g) {
    HAL_GPIO_WritePin(LED_PORT, LED_RED, r ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_PORT, LED_YEL, y ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_PORT, LED_GRN, g ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
static void leds_all(uint8_t on) { leds_set(on, on, on); }
static uint32_t elapsed(void) { return HAL_GetTick() - state_enter_ms; }
static void enter_state(TrafficState_t s, const char *name) {
    state = s; state_enter_ms = HAL_GetTick();
    char buf[32]; int n = snprintf(buf, sizeof(buf), "State: %s\r\n", name);
    HAL_UART_Transmit(&huart2, (uint8_t*)buf, n, 50);
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    enter_state(ST_RED, "RED");
    leds_set(1, 0, 0);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        if (emergency && state != ST_EMERGENCY) {
            emergency = 0; emrg_count = 0;
            enter_state(ST_EMERGENCY, "EMERGENCY");
        }

        switch (state) {
            case ST_RED:
                leds_set(1, 0, 0);
                if (elapsed() >= 3000) enter_state(ST_RED_YEL, "RED+YEL");
                break;
            case ST_RED_YEL:
                leds_set(1, 1, 0);
                if (elapsed() >= 1000) enter_state(ST_GREEN, "GREEN");
                break;
            case ST_GREEN:
                leds_set(0, 0, 1);
                if (elapsed() >= 3000) enter_state(ST_YELLOW, "YELLOW");
                break;
            case ST_YELLOW:
                leds_set(0, 1, 0);
                if (elapsed() >= 1000) enter_state(ST_RED, "RED");
                break;
            case ST_EMERGENCY:
                leds_all(emrg_count & 1);
                HAL_Delay(200);
                emrg_count++;
                if (emrg_count >= 50) enter_state(ST_RED, "RED"); // 10s
                break;
        }
        HAL_Delay(10);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====

// ===== USER CODE BEGIN 4 =====
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_13) emergency = 1; // B1 pressed
}
// ===== USER CODE END 4 =====
