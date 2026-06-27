/*
 * main_usercode.c — Project 27: Button-Driven State Machine
 * Target: STM32F103C8T6 (Blue Pill)
 *
 * Paste each labeled block into the matching USER CODE section of the
 * CubeMX-generated main.c.
 *
 * CubeMX requirements:
 *   - PC13: GPIO Output (onboard LED, active-low)
 *   - PA0:  GPIO_EXTI0, Pull-Up, Falling edge trigger
 *   - NVIC: EXTI0 enabled
 *   - SysTick for HAL_GetTick / HAL_Delay
 *
 * NOTE: This code has not been tested on hardware.
 */

// ===== USER CODE BEGIN Includes =====
#include <stdint.h>
// ===== USER CODE END Includes =====


// ===== USER CODE BEGIN PD (Private Defines) =====
#define LED_PIN         GPIO_PIN_13
#define LED_PORT        GPIOC

#define BTN_PIN         GPIO_PIN_0
#define BTN_PORT        GPIOA

#define DEBOUNCE_MS     50U     /* Minimum ms between accepted button events */
#define BLINK_SLOW_MS   500U
#define BLINK_FAST_MS   100U
// ===== USER CODE END PD =====


// ===== USER CODE BEGIN PFP (Private Function Prototypes) =====
static void fsm_run(void);
// ===== USER CODE END PFP =====


// ===== USER CODE BEGIN PV (Private Variables) =====

typedef enum
{
    STATE_IDLE       = 0,
    STATE_BLINK_SLOW = 1,
    STATE_BLINK_FAST = 2,
    STATE_OFF        = 3,
    STATE_COUNT      = 4   /* Total number of states — used for wrap-around */
} AppState_t;

static volatile uint8_t  btn_event    = 0;  /* Set by EXTI ISR, cleared by main loop */
static AppState_t        current_state = STATE_IDLE;
static uint32_t          last_btn_tick = 0; /* For debounce */
static uint32_t          last_blink_tick = 0;
static uint8_t           led_state    = 0;  /* 0 = OFF (pin HIGH), 1 = ON (pin LOW) */

// ===== USER CODE END PV =====


// ===== USER CODE BEGIN 0 (Functions defined before main) =====

/**
 * @brief  EXTI0 IRQ handler callback (called by HAL from EXTI0_IRQHandler).
 *         Sets the btn_event flag; actual state transition happens in main loop
 *         to avoid running FSM logic inside an ISR.
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == BTN_PIN)
    {
        btn_event = 1;
    }
}

/**
 * @brief  LED helper — ON means PC13 LOW (active-low), OFF means PC13 HIGH.
 */
static inline void led_on(void)
{
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);
    led_state = 1;
}

static inline void led_off(void)
{
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);
    led_state = 0;
}

static inline void led_toggle(void)
{
    HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
    led_state ^= 1U;
}

/**
 * @brief  Main FSM tick — call this every loop iteration.
 *         Checks for a pending button event, advances state if debounce
 *         passes, then applies per-state LED behavior.
 */
static void fsm_run(void)
{
    uint32_t now = HAL_GetTick();

    /* --- State transition on button event --- */
    if (btn_event)
    {
        btn_event = 0;  /* Clear flag first to avoid missing a future press */

        if ((now - last_btn_tick) >= DEBOUNCE_MS)
        {
            last_btn_tick = now;

            /* Advance to next state, wrap around */
            current_state = (AppState_t)((current_state + 1) % STATE_COUNT);

            /* Reset blink timer on every state change */
            last_blink_tick = now;

            /* Apply immediate LED state for non-blink states */
            switch (current_state)
            {
                case STATE_IDLE:
                    led_on();
                    break;
                case STATE_OFF:
                    led_off();
                    break;
                default:
                    /* Blink states — timer handles it below */
                    break;
            }
        }
    }

    /* --- Per-state LED output --- */
    switch (current_state)
    {
        case STATE_IDLE:
            /* Solid ON — already set on transition, nothing to do */
            break;

        case STATE_BLINK_SLOW:
            if ((now - last_blink_tick) >= BLINK_SLOW_MS)
            {
                last_blink_tick = now;
                led_toggle();
            }
            break;

        case STATE_BLINK_FAST:
            if ((now - last_blink_tick) >= BLINK_FAST_MS)
            {
                last_blink_tick = now;
                led_toggle();
            }
            break;

        case STATE_OFF:
            /* Solid OFF — already set on transition, nothing to do */
            break;

        default:
            /* Should never reach here; recover to IDLE */
            current_state = STATE_IDLE;
            led_on();
            break;
    }
}

// ===== USER CODE END 0 =====


// ===== USER CODE BEGIN 2 (inside main(), after MX_xxx_Init calls) =====
/*
 * Set initial LED state: IDLE = LED ON (PC13 LOW)
 */
led_on();
last_blink_tick = HAL_GetTick();
last_btn_tick   = HAL_GetTick();
// ===== USER CODE END 2 =====


// ===== USER CODE BEGIN WHILE (inside the while(1) loop) =====
fsm_run();
/* No HAL_Delay here — fsm_run uses HAL_GetTick for non-blocking timing */
// ===== USER CODE END WHILE =====
