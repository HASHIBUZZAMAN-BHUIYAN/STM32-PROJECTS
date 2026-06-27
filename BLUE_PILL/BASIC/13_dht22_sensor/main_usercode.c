/**
 * @file    main_usercode.c
 * @brief   DHT22 single-wire bit-bang driver using TIM3 for µs timing.
 *
 * Paste each labelled block into the corresponding USER CODE BEGIN/END region
 * inside the CubeMX-generated main.c.
 *
 * CubeMX config required:
 *   - TIM3: up-counting, PSC=71, ARR=0xFFFF, no interrupts, no channels
 *     → tick period = (71+1)/72MHz = 1 µs
 *   - PB0: GPIO_Output (initial state; toggled to input during read)
 *   - USART1: 115200 8N1, TX=PA9
 *   - SysTick as HAL timebase
 *
 * HARDWARE: 4.7 kΩ pull-up resistor from PB0 to 3.3 V (mandatory).
 *
 * NOT TESTED ON HARDWARE — reference template only.
 */

/* ===== USER CODE BEGIN Includes ===== */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
/* ===== USER CODE END Includes ===== */


/* ===== USER CODE BEGIN PD (Private Defines) ===== */
#define DHT22_PORT       GPIOB
#define DHT22_PIN        GPIO_PIN_0

/*
 * Bit decision threshold (µs).
 * DHT22 encodes "0" as ~28 µs HIGH, "1" as ~70 µs HIGH.
 * A threshold of 50 µs cleanly separates the two.
 */
#define DHT22_BIT_THRESHOLD_US  50U

/* Maximum wait for a line transition (µs) — avoid infinite loops */
#define DHT22_TIMEOUT_US        200U
/* ===== USER CODE END PD ===== */


/* ===== USER CODE BEGIN PV (Private Variables) ===== */
typedef struct {
    float    humidity;      /* e.g. 55.3  (%) */
    float    temperature;   /* e.g. 24.1  (°C) */
    uint8_t  valid;         /* 1 = checksum passed, 0 = error */
} DHT22_Data_t;

static DHT22_Data_t dht_data;
/* ===== USER CODE END PV ===== */


/* ===== USER CODE BEGIN 0 (Private function prototypes & implementations) ===== */

extern TIM_HandleTypeDef  htim3;
extern UART_HandleTypeDef huart1;

/* -----------------------------------------------------------------------
 * TIM3-based microsecond helpers
 * --------------------------------------------------------------------- */

/**
 * @brief Start TIM3 counter from zero.
 *        Call once after MX_TIM3_Init().
 */
static void TIM3_Start(void)
{
    __HAL_TIM_SET_COUNTER(&htim3, 0);
    HAL_TIM_Base_Start(&htim3);
}

/**
 * @brief Busy-wait for exactly `us` microseconds using TIM3.
 */
static void Delay_us(uint32_t us)
{
    __HAL_TIM_SET_COUNTER(&htim3, 0);
    while (__HAL_TIM_GET_COUNTER(&htim3) < us) { /* spin */ }
}

/**
 * @brief Read TIM3 counter value (microseconds since last reset).
 */
static uint32_t GetTick_us(void)
{
    return __HAL_TIM_GET_COUNTER(&htim3);
}

/* -----------------------------------------------------------------------
 * GPIO helper: re-configure PB0 as Output or Input on the fly
 * --------------------------------------------------------------------- */

static void DHT22_PinOutput(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin   = DHT22_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DHT22_PORT, &GPIO_InitStruct);
}

static void DHT22_PinInput(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin  = DHT22_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;  /* backup; external 4.7k is mandatory */
    HAL_GPIO_Init(DHT22_PORT, &GPIO_InitStruct);
}

/* -----------------------------------------------------------------------
 * DHT22 protocol
 * --------------------------------------------------------------------- */

/**
 * @brief Wait until the data line reaches the expected level or times out.
 * @param level   GPIO_PIN_SET or GPIO_PIN_RESET
 * @return elapsed µs, or 0xFFFF on timeout.
 */
static uint32_t DHT22_WaitLevel(GPIO_PinState level)
{
    __HAL_TIM_SET_COUNTER(&htim3, 0);
    while (HAL_GPIO_ReadPin(DHT22_PORT, DHT22_PIN) != level)
    {
        if (GetTick_us() >= DHT22_TIMEOUT_US) return 0xFFFF;
    }
    return GetTick_us();
}

/**
 * @brief Read 40 bits from DHT22 and parse into DHT22_Data_t.
 *
 * Protocol steps:
 *   1. Host pulls LOW ≥ 18 ms  (start signal)
 *   2. Host releases (input)
 *   3. Sensor responds: LOW 80 µs, then HIGH 80 µs
 *   4. 40 data bits: each starts with 50 µs LOW, then HIGH duration
 *      determines bit value (< 50 µs = 0, ≥ 50 µs = 1)
 *   5. Checksum = sum of first 4 bytes (low 8 bits)
 *
 * @param out   Pointer to result structure.
 * @return 1 on success (checksum OK), 0 on error.
 */
static int DHT22_Read(DHT22_Data_t *out)
{
    uint8_t  data[5] = {0};
    uint32_t pulse_us;

    /* --- Step 1: Start signal — pull LOW for ~18 ms --- */
    DHT22_PinOutput();
    HAL_GPIO_WritePin(DHT22_PORT, DHT22_PIN, GPIO_PIN_RESET);
    HAL_Delay(18);                      /* 18 ms using SysTick */

    /* --- Step 2: Release line, switch to input --- */
    DHT22_PinInput();
    Delay_us(30);                       /* wait for sensor to take over */

    /* --- Step 3: Sensor acknowledge pulse (LOW 80 µs, HIGH 80 µs) --- */
    if (DHT22_WaitLevel(GPIO_PIN_RESET) == 0xFFFF) { out->valid = 0; return 0; }
    if (DHT22_WaitLevel(GPIO_PIN_SET)   == 0xFFFF) { out->valid = 0; return 0; }
    if (DHT22_WaitLevel(GPIO_PIN_RESET) == 0xFFFF) { out->valid = 0; return 0; }

    /* --- Step 4: Read 40 bits --- */
    for (int bit = 0; bit < 40; bit++)
    {
        /* Each bit starts with a ~50 µs LOW */
        if (DHT22_WaitLevel(GPIO_PIN_SET) == 0xFFFF) { out->valid = 0; return 0; }

        /* Measure HIGH duration to determine 0 or 1 */
        __HAL_TIM_SET_COUNTER(&htim3, 0);
        if (DHT22_WaitLevel(GPIO_PIN_RESET) == 0xFFFF) { out->valid = 0; return 0; }
        pulse_us = GetTick_us();

        /* Shift bit into the correct byte */
        data[bit / 8] <<= 1;
        if (pulse_us >= DHT22_BIT_THRESHOLD_US) {
            data[bit / 8] |= 1;
        }
    }

    /* --- Step 5: Checksum --- */
    uint8_t checksum = (uint8_t)(data[0] + data[1] + data[2] + data[3]);
    if (checksum != data[4]) { out->valid = 0; return 0; }

    /* Parse humidity (data[0..1]), temperature (data[2..3]) */
    uint16_t rh_raw = ((uint16_t)data[0] << 8) | data[1];
    uint16_t t_raw  = ((uint16_t)(data[2] & 0x7F) << 8) | data[3];

    out->humidity    = rh_raw / 10.0f;
    out->temperature = t_raw  / 10.0f;
    if (data[2] & 0x80) out->temperature = -out->temperature; /* sign bit */
    out->valid = 1;
    return 1;
}

/* ===== USER CODE END 0 ===== */


/* ===== USER CODE BEGIN 2 (inside main(), after MX_TIM3_Init()) ===== */
/*
    TIM3_Start();   // Start free-running 1 µs timer
*/
/* ===== USER CODE END 2 ===== */


/* ===== USER CODE BEGIN WHILE (main loop) ===== */
/*
    char uart_buf[64];

    while (1)
    {
        // DHT22 requires a minimum 2-second interval between reads
        HAL_Delay(2000);

        int ok = DHT22_Read(&dht_data);

        if (ok)
        {
            // Format: "RH: 55.3 %  T: 24.1 C\r\n"
            int rh_int  = (int)dht_data.humidity;
            int rh_frac = (int)((dht_data.humidity - rh_int) * 10);
            int t_int   = (int)dht_data.temperature;
            int t_frac  = (int)((dht_data.temperature  - t_int)  * 10);
            if (t_int < 0)  t_frac = -t_frac;

            snprintf(uart_buf, sizeof(uart_buf),
                     "RH: %d.%d %%  T: %d.%d C\r\n",
                     rh_int, rh_frac, t_int, t_frac);
        }
        else
        {
            snprintf(uart_buf, sizeof(uart_buf), "DHT22 read error\r\n");
        }

        HAL_UART_Transmit(&huart1, (uint8_t *)uart_buf,
                          strlen(uart_buf), 100);

        // Heartbeat: toggle PC13 LED
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    }
*/
/* ===== USER CODE END WHILE ===== */
