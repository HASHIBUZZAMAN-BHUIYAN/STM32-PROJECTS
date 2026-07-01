// ============================================================
// Black Pill BASIC/16 — I2S Audio Basics (PCM5102A DAC)
// STM32F411CEU6  |  HAL + CubeMX 6.16  |  100 MHz
// I2S3: PC10=CK, PC12=SD, PA4=WS  |  44100Hz 16-bit stereo
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <math.h>
#include <string.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
#define SAMPLE_RATE 44100
#define LUT_SIZE    100   // 44100/100 = 441 Hz ≈ A4
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
static int16_t audio_lut[LUT_SIZE * 2]; // stereo interleaved (L, R, L, R, ...)
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static void build_sine(void) {
    for (uint32_t i = 0; i < LUT_SIZE; i++) {
        int16_t s = (int16_t)(28000.0f * sinf(2.0f * 3.14159265f * i / LUT_SIZE));
        audio_lut[2*i]   = s; // L
        audio_lut[2*i+1] = s; // R
    }
}

void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s) { (void)hi2s; }
void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s)     { (void)hi2s; }
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    build_sine();
    // DMA circular: continuously stream LUT → PCM5102A
    HAL_I2S_Transmit_DMA(&hi2s3, (uint16_t*)audio_lut, LUT_SIZE * 2);
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
