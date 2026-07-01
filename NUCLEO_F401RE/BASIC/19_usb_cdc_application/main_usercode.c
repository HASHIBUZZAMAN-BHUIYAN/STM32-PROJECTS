// ============================================================
// Nucleo-F401RE BASIC/19 — USB CDC Application Port
// STM32F401RETx  |  HAL + CubeMX 6.16 + USB CDC middleware
// ============================================================
// CubeMX generates usb_device.c + usbd_cdc_if.c.
// Include usbd_cdc_if.h to call CDC_Transmit_FS().
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include "usbd_cdc_if.h"
#include <stdio.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PV =====
static char cdc_buf[64];
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 2 =====
    HAL_UART_Transmit(&huart2, (uint8_t*)"USB CDC init done\r\n", 19, 100);
    HAL_Delay(1000); // give USB time to enumerate
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        int n = snprintf(cdc_buf, sizeof(cdc_buf),
            "Nucleo F401RE | tick=%lu\r\n", HAL_GetTick());
        CDC_Transmit_FS((uint8_t*)cdc_buf, n);
        HAL_Delay(500);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====

// ===== USER CODE BEGIN 4 =====
// To receive data from PC over USB CDC, override CDC_Receive_FS in usbd_cdc_if.c:
// In usbd_cdc_if.c, USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION:
//
// static int8_t CDC_Receive_FS(uint8_t* Buf, uint32_t *Len) {
//   // Process received data from PC here
//   CDC_Transmit_FS(Buf, *Len);  // echo back
//   USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
//   USBD_CDC_ReceivePacket(&hUsbDeviceFS);
//   return USBD_OK;
// }
// ===== USER CODE END 4 =====
