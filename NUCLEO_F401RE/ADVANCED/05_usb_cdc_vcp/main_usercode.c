// ============================================================
// Nucleo-F401RE ADVANCED/05 — USB CDC Virtual COM Port
// STM32F401RETx  |  HAL + CubeMX 6.16
// USB_OTG_FS CDC: PA11=D-, PA12=D+
// ============================================================
// Requires CubeMX: USB_OTG_FS Device + USB_DEVICE CDC class
// Edit usbd_cdc_if.c: fill in CDC_Receive_FS with callback below

// ===== USER CODE BEGIN Includes =====
#include "usbd_cdc_if.h"
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PV =====
static volatile uint8_t cdc_rx_len = 0;
static uint8_t cdc_rx_buf[64];
static uint8_t cdc_tx_buf[128];
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
/*
 * In usbd_cdc_if.c, modify CDC_Receive_FS:
 *
 * extern volatile uint8_t cdc_rx_len;
 * extern uint8_t cdc_rx_buf[];
 *
 * static int8_t CDC_Receive_FS(uint8_t* Buf, uint32_t *Len) {
 *     uint32_t len = *Len > 63 ? 63 : *Len;
 *     memcpy(cdc_rx_buf, Buf, len);
 *     cdc_rx_buf[len] = '\0';
 *     cdc_rx_len = (uint8_t)len;
 *     USBD_CDC_SetRxBuffer(&hUsbDeviceFS, UserRxBufferFS);
 *     USBD_CDC_ReceivePacket(&hUsbDeviceFS);
 *     return USBD_OK;
 * }
 */

static void cdc_send(const char *s) {
    uint16_t len = (uint16_t)strlen(s);
    // CDC_Transmit_FS returns USBD_BUSY if previous TX not finished
    // Retry a few times with short delay
    for (int retry = 0; retry < 5; retry++) {
        if (CDC_Transmit_FS((uint8_t*)s, len) == USBD_OK) return;
        HAL_Delay(1);
    }
}

static void handle_cdc_command(const uint8_t *data, uint8_t len) {
    if (len >= 3 && memcmp(data, "adc", 3) == 0) {
        // Read ADC and reply
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, 10);
        uint32_t raw = HAL_ADC_GetValue(&hadc1);
        uint32_t mv  = (raw * 3300U) / 4096U;
        int n = snprintf((char*)cdc_tx_buf, sizeof(cdc_tx_buf),
                         "ADC raw=%lu mV=%lu\r\n", raw, mv);
        CDC_Transmit_FS(cdc_tx_buf, (uint16_t)n);
    } else {
        // Echo with length prefix
        int n = snprintf((char*)cdc_tx_buf, sizeof(cdc_tx_buf),
                         "[%u bytes] %.*s\r\n", len, len, (char*)data);
        CDC_Transmit_FS(cdc_tx_buf, (uint16_t)n);
    }
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    HAL_Delay(500); // wait for USB enumeration
    cdc_send("USB CDC ready. Commands: adc, or type text to echo.\r\n");
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        if (cdc_rx_len > 0) {
            uint8_t len = cdc_rx_len;
            cdc_rx_len = 0;
            handle_cdc_command(cdc_rx_buf, len);
        }
        // Heartbeat blink
        static uint32_t last = 0;
        if (HAL_GetTick() - last >= 1000) {
            last = HAL_GetTick();
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        }
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====
