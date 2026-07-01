// ============================================================
// Nucleo-F401RE BASIC/18 — I2S Audio Tone Generation
// STM32F401RETx  |  HAL + CubeMX 6.16
// SPI3 as I2S3 Master TX + DMA → PCM5102A DAC
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <math.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
#define SINE_SAMPLES   100    // 44100 / 441 Hz ≈ 100 samples per period
#define SAMPLE_MAX     32767  // 16-bit signed max
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
// Stereo interleaved: [L0, R0, L1, R1, ...]
static int16_t sine_buf[SINE_SAMPLES * 2];
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 2 =====
    // Build 440 Hz sine LUT
    for (int i = 0; i < SINE_SAMPLES; i++) {
        int16_t sample = (int16_t)(SAMPLE_MAX * sinf(2.0f * 3.14159265f * i / SINE_SAMPLES));
        sine_buf[i * 2]     = sample;  // left channel
        sine_buf[i * 2 + 1] = sample;  // right channel (mono)
    }

    // Start I2S DMA circular transmission
    // HAL_I2S_Transmit_DMA works with 16-bit samples; pass half-words
    HAL_I2S_Transmit_DMA(&hi2s3, (uint16_t*)sine_buf, SINE_SAMPLES * 2);

    HAL_UART_Transmit(&huart2, (uint8_t*)"I2S 440Hz tone playing\r\n", 24, 100);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        // DMA handles audio in background; nothing to do in main loop
        HAL_Delay(5000);
        HAL_UART_Transmit(&huart2, (uint8_t*)"I2S running\r\n", 13, 100);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====
