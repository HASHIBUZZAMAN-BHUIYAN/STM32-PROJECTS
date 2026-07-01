// ============================================================
// Black Pill BASIC/14 — High Clock Timing (100 MHz)
// STM32F411CEU6  |  HAL + CubeMX 6.16  |  100 MHz
// TIM2 at 1 MHz for µs measurement, USART1 for output
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
#include <string.h>
#include <math.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PV =====
static char buf[80];
static uint8_t mem_src[1024];
static uint8_t mem_dst[1024];
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static void uart_tx(const char *s) {
    HAL_UART_Transmit(&huart1, (const uint8_t*)s, strlen(s), 200);
}

static uint32_t tim2_us(void) {
    return __HAL_TIM_GET_COUNTER(&htim2);
}

static void measure_and_print(const char *label, uint32_t t0, uint32_t t1) {
    int n = snprintf(buf, sizeof(buf), "%-20s %lu us\r\n", label, t1 - t0);
    uart_tx(buf);
    (void)n;
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    HAL_TIM_Base_Start(&htim2);
    memset(mem_src, 0xAA, sizeof(mem_src));

    uart_tx("Black Pill 100MHz timing demo\r\n");
    uart_tx("------------------------------\r\n");
    HAL_Delay(10);

    // 1. GPIO toggle round-trip (set + clear)
    uint32_t t0 = tim2_us();
    for (int i = 0; i < 100; i++) {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
    }
    measure_and_print("GPIO toggle x100:", t0, tim2_us());

    // 2. memcpy 1KB
    t0 = tim2_us();
    memcpy(mem_dst, mem_src, 1024);
    measure_and_print("memcpy 1KB:", t0, tim2_us());

    // 3. FPU sinf (hardware float)
    volatile float result = 0.0f;
    t0 = tim2_us();
    for (int i = 0; i < 1000; i++) result += sinf(1.234f + i * 0.001f);
    uint32_t t_fpu = tim2_us() - t0;
    int n = snprintf(buf, sizeof(buf), "%-20s %lu us (%.4f)\r\n",
                     "sinf x1000 (FPU):", t_fpu, (double)result);
    uart_tx(buf); (void)n;

    // 4. UART TX "hello\r\n" (6 bytes @ 115200 = ~52µs/byte)
    t0 = tim2_us();
    HAL_UART_Transmit(&huart1, (uint8_t*)"hello\r\n", 7, 200);
    measure_and_print("UART TX 7 bytes:", t0, tim2_us());

    uart_tx("------------------------------\r\n");
    uart_tx("Looping — LED blinks at 1Hz\r\n");
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        HAL_Delay(500);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====
