/**
 * @file    main_usercode.c
 * @brief   05 — ADC Potentiometer Read (ADC1 CH1 on PA1) + UART1 Output
 *          Blue Pill STM32F103C8T6
 *
 * PA1:  analog input, potentiometer wiper (ADC1 Channel 1).
 * PA9:  USART1 TX — connect to RX of USB-serial adapter (3.3 V).
 * PA10: USART1 RX — connect to TX of USB-serial adapter (3.3 V).
 *
 * ADC: 12-bit, polling, single conversion, no DMA.
 * Output format (115200 baud): "ADC: 2048  =>  50%\r\n"
 *
 * Copy each block into the matching USER CODE BEGIN/END region
 * inside the CubeMX-generated Core/Src/main.c.
 *
 * NOTE: This code has NOT been tested on hardware. Review before flashing.
 *
 * CubeMX requirements:
 *   - ADC1 IN1 enabled, Single Conv, 12-bit right-aligned, no DMA
 *   - USART1 Async, 115200 baud, 8N1
 *   - PA1: Analog input (auto-set by enabling ADC1 IN1)
 *   - PA9:  USART1_TX (alternate function, auto-set by CubeMX)
 *   - PA10: USART1_RX (alternate function, auto-set by CubeMX)
 */

/* ===== USER CODE BEGIN Includes ===== */
#include <stdio.h>    /* sprintf */
#include <string.h>   /* strlen  */
/* ===== USER CODE END Includes ===== */


/* ===== USER CODE BEGIN PD (Private Defines) ===== */
#define ADC_MAX_VALUE       4095U    /* 12-bit ADC full-scale */
#define ADC_TIMEOUT_MS      100U     /* Polling timeout per conversion */
#define SAMPLE_INTERVAL_MS  500U     /* Delay between readings */
#define TX_BUF_SIZE         48U      /* UART transmit buffer size (bytes) */
/* ===== USER CODE END PD ===== */


/* ===== USER CODE BEGIN PV (Private Variables) ===== */
static char txBuf[TX_BUF_SIZE];     /* UART transmit buffer */
/* ===== USER CODE END PV ===== */


/* ===== USER CODE BEGIN 2 ===== */

    /*
     * Run ADC self-calibration.
     * HAL_ADCEx_Calibration_Run resets and calibrates the ADC offset register.
     * Recommended for STM32F1 to achieve datasheet accuracy.
     * Must be called before the first HAL_ADC_Start.
     */
    if (HAL_ADCEx_Calibration_Run(&hadc1) != HAL_OK)
    {
        Error_Handler();
    }

/* ===== USER CODE END 2 ===== */


/* ===== USER CODE BEGIN WHILE ===== */

    /* Start a single conversion */
    if (HAL_ADC_Start(&hadc1) != HAL_OK)
    {
        Error_Handler();
    }

    /* Wait for conversion to complete (polling) */
    if (HAL_ADC_PollForConversion(&hadc1, ADC_TIMEOUT_MS) == HAL_OK)
    {
        /* Read the 12-bit result (0–4095) */
        uint32_t adcValue = HAL_ADC_GetValue(&hadc1);

        /* Scale to 0–100 %
         * Integer arithmetic: multiply first to preserve precision. */
        uint32_t percent = (adcValue * 100U) / ADC_MAX_VALUE;

        /* Format output string */
        int len = snprintf(txBuf, TX_BUF_SIZE,
                           "ADC: %4lu  =>  %3lu%%\r\n",
                           (unsigned long)adcValue,
                           (unsigned long)percent);

        /* Transmit over UART1; 100 ms timeout */
        if (len > 0)
        {
            HAL_UART_Transmit(&huart1,
                              (uint8_t *)txBuf,
                              (uint16_t)len,
                              100U);
        }
    }

    /* Stop ADC (required when not in continuous mode) */
    HAL_ADC_Stop(&hadc1);

    /* Wait before next sample */
    HAL_Delay(SAMPLE_INTERVAL_MS);

/* ===== USER CODE END WHILE ===== */
