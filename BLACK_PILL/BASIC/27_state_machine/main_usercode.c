// ============================================================
// Black Pill BASIC/27 — State Machine (Traffic Light)
// STM32F411CEU6  |  HAL + CubeMX 6.16  |  100 MHz
// PA5=Red, PA6=Amber, PA7=Green LEDs, PA0=Emergency button
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <string.h>
#include <stdio.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
typedef enum {
    STATE_RED, STATE_RED_AMBER, STATE_GREEN, STATE_AMBER, STATE_EMERGENCY
} TrafficState_t;

#define RED_LED    GPIO_PIN_5
#define AMBER_LED  GPIO_PIN_6
#define GREEN_LED  GPIO_PIN_7
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
static TrafficState_t state = STATE_RED;
static uint32_t state_enter_tick = 0;
static volatile uint8_t emergency = 0;
static char buf[48];
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static void set_lights(uint8_t r, uint8_t a, uint8_t g) {
    HAL_GPIO_WritePin(GPIOA, RED_LED,   r ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, AMBER_LED, a ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GREEN_LED, g ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static const char* state_name(TrafficState_t s) {
    switch(s) {
        case STATE_RED:       return "RED";
        case STATE_RED_AMBER: return "RED+AMBER";
        case STATE_GREEN:     return "GREEN";
        case STATE_AMBER:     return "AMBER";
        case STATE_EMERGENCY: return "EMERGENCY";
        default: return "?";
    }
}

static void enter_state(TrafficState_t new_state) {
    state = new_state;
    state_enter_tick = HAL_GetTick();
    int n = snprintf(buf, sizeof(buf), "→ %s\r\n", state_name(new_state));
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, n, 50);
    (void)n;
    switch (new_state) {
        case STATE_RED:       set_lights(1,0,0); break;
        case STATE_RED_AMBER: set_lights(1,1,0); break;
        case STATE_GREEN:     set_lights(0,0,1); break;
        case STATE_AMBER:     set_lights(0,1,0); break;
        case STATE_EMERGENCY: set_lights(0,0,0); break;
    }
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    // All LEDs off initially
    set_lights(0, 0, 0);
    HAL_UART_Transmit(&huart1,
        (uint8_t*)"Traffic light state machine\r\nPress PA0 for emergency\r\n", 53, 100);
    enter_state(STATE_RED);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        uint32_t elapsed = HAL_GetTick() - state_enter_tick;

        // Emergency interrupt preempts everything
        if (emergency && state != STATE_EMERGENCY) {
            emergency = 0;
            enter_state(STATE_EMERGENCY);
            continue;
        }

        switch (state) {
            case STATE_RED:
                if (elapsed >= 5000) enter_state(STATE_RED_AMBER);
                break;
            case STATE_RED_AMBER:
                if (elapsed >= 1000) enter_state(STATE_GREEN);
                break;
            case STATE_GREEN:
                if (elapsed >= 5000) enter_state(STATE_AMBER);
                break;
            case STATE_AMBER:
                if (elapsed >= 2000) enter_state(STATE_RED);
                break;
            case STATE_EMERGENCY:
                // Flash red during emergency
                if ((elapsed / 300) % 2)
                    HAL_GPIO_WritePin(GPIOA, RED_LED, GPIO_PIN_SET);
                else
                    HAL_GPIO_WritePin(GPIOA, RED_LED, GPIO_PIN_RESET);
                if (elapsed >= 3000) enter_state(STATE_RED);
                break;
        }

        // Onboard LED mirrors Red LED (PC13 active-low)
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13,
            HAL_GPIO_ReadPin(GPIOA, RED_LED) ? GPIO_PIN_RESET : GPIO_PIN_SET);
        HAL_Delay(50);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====

// ===== USER CODE BEGIN 4 =====
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_0) emergency = 1;
}
// ===== USER CODE END 4 =====
