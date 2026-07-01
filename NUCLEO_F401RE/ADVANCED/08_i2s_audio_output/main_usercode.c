// ============================================================
// Nucleo-F401RE ADVANCED/08 — I2S Audio Output
// STM32F401RETx  |  HAL + CubeMX 6.16
// I2S3 DMA circular → PCM5102A DAC  |  44100 Hz 16-bit stereo
// PC10=CK, PC12=SD, PA4=WS, PC7=MCK
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
#define SAMPLE_RATE   44100
#define LUT_SIZE      100   // samples per cycle → 441 Hz default
#define BUF_SAMPLES   (LUT_SIZE * 2)  // stereo (L+R interleaved)
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
static int16_t audio_buf[BUF_SAMPLES * 2]; // double-buffer for DMA ping-pong
static uint16_t target_freq = 440;
static uint8_t  volume_pct  = 80;
static uint8_t  muted       = 0;
static volatile uint8_t rebuild_lut = 0;

static volatile uint8_t rx_byte;
static char cmd_buf[32];
static uint8_t cmd_idx = 0;
static volatile uint8_t cmd_ready = 0;
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static void build_sine(int16_t *buf, uint16_t freq, uint8_t vol) {
    // Samples per cycle at this frequency
    uint32_t spc = SAMPLE_RATE / freq;
    if (spc == 0) spc = 1;
    float amplitude = (32767.0f * vol) / 100.0f;
    for (uint32_t i = 0; i < spc * 2; i += 2) {
        // LUT wraps — fill double buffer with two cycles minimum
        int16_t s = (int16_t)(amplitude * sinf(2.0f * 3.14159265f * (float)(i/2) / (float)spc));
        buf[i]   = s; // L
        buf[i+1] = s; // R
    }
    // Fill rest of buffer (if any) with same waveform
    for (uint32_t i = spc*2; i < (uint32_t)(BUF_SAMPLES*2); i++) {
        buf[i] = buf[i % (spc*2)];
    }
}

void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s) {
    (void)hi2s;
    if (rebuild_lut) {
        rebuild_lut = 0;
        build_sine(audio_buf, target_freq, muted ? 0 : volume_pct);
    }
}

void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s) {
    (void)hi2s;
}

static void process_cmd(void) {
    char reply[64];
    if (strncmp(cmd_buf, "freq ", 5) == 0) {
        uint16_t f = (uint16_t)atoi(cmd_buf + 5);
        if (f >= 20 && f <= 20000) {
            target_freq = f;
            rebuild_lut = 1;
            snprintf(reply, sizeof(reply), "Freq: %u Hz\r\n", f);
        } else {
            snprintf(reply, sizeof(reply), "Range: 20-20000 Hz\r\n");
        }
    } else if (strncmp(cmd_buf, "vol ", 4) == 0) {
        int v = atoi(cmd_buf + 4);
        volume_pct = (uint8_t)(v < 0 ? 0 : v > 100 ? 100 : v);
        rebuild_lut = 1;
        snprintf(reply, sizeof(reply), "Volume: %u%%\r\n", volume_pct);
    } else if (strncmp(cmd_buf, "stop", 4) == 0) {
        muted = 1; rebuild_lut = 1;
        snprintf(reply, sizeof(reply), "Muted\r\n");
    } else if (strncmp(cmd_buf, "play", 4) == 0) {
        muted = 0; rebuild_lut = 1;
        snprintf(reply, sizeof(reply), "Playing %u Hz\r\n", target_freq);
    } else {
        snprintf(reply, sizeof(reply), "Cmds: freq <hz>, vol <0-100>, stop, play\r\n");
    }
    HAL_UART_Transmit(&huart2, (uint8_t*)reply, strlen(reply), 200);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        char c = (char)rx_byte;
        if (c == '\r' || c == '\n') {
            cmd_buf[cmd_idx] = '\0';
            if (cmd_idx > 0) cmd_ready = 1;
        } else if (cmd_idx < sizeof(cmd_buf)-1) {
            cmd_buf[cmd_idx++] = c;
        }
        HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
    }
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    build_sine(audio_buf, target_freq, volume_pct);
    // DMA circular: total samples = BUF_SAMPLES*2 (double-buffer)
    HAL_I2S_Transmit_DMA(&hi2s3, (uint16_t*)audio_buf, BUF_SAMPLES * 2);
    HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
    const char *banner = "I2S audio. Cmds: freq <hz>, vol <0-100>, stop, play\r\n";
    HAL_UART_Transmit(&huart2, (uint8_t*)banner, strlen(banner), 200);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        if (cmd_ready) {
            cmd_ready = 0;
            process_cmd();
            cmd_idx = 0;
            memset(cmd_buf, 0, sizeof(cmd_buf));
        }
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        HAL_Delay(500);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====
