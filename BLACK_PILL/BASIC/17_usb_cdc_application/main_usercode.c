/**
 * @file    main_usercode.c
 * @brief   Project 17 — USB CDC Virtual COM Port + ADC1 CH0 streaming
 * @board   STM32F411CEU6 Black Pill
 *
 * Drop the labelled blocks into the corresponding USER CODE BEGIN/END
 * sections of the CubeMX-generated main.c.
 *
 * CubeMX peripherals required:
 *   USB_OTG_FS  — Device Only
 *   USB_DEVICE  — CDC class
 *   ADC1        — IN0 (PA0), single SW-triggered, 12-bit
 *   PC13        — GPIO Output (LED, active-low)
 *   TIM11       — timebase source (recommended when USB is active)
 */

/* ── USER CODE BEGIN Includes ──────────────────────────────────────────── */
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "usbd_cdc_if.h"   /* CDC_Transmit_FS, CDC receive callback */
#include "usb_device.h"    /* hUsbDeviceFS */
/* ── USER CODE END Includes ────────────────────────────────────────────── */


/* ── USER CODE BEGIN PD (Private Defines) ──────────────────────────────── */
#define ADC_VREF_MV       3300U   /* mV — 3.3 V logic supply             */
#define ADC_RESOLUTION    4095U   /* 12-bit full-scale                    */
#define TX_INTERVAL_MS    500U    /* transmit period                      */
#define CDC_TX_RETRIES    10U     /* retry attempts if USBD_BUSY          */
/* ── USER CODE END PD ──────────────────────────────────────────────────── */


/* ── USER CODE BEGIN PV (Private Variables) ────────────────────────────── */
/* Receive buffer — filled by CDC_Receive_FS callback in usbd_cdc_if.c    */
static volatile uint8_t  rx_buf[64];
static volatile uint16_t rx_len   = 0;
static volatile uint8_t  rx_ready = 0;   /* set by callback              */

static char     tx_str[80];              /* formatted transmit string     */
static uint32_t last_tx_tick = 0;
/* ── USER CODE END PV ──────────────────────────────────────────────────── */


/* ── USER CODE BEGIN 0 (Private Function Prototypes / Helpers) ─────────── */

/**
 * @brief  Read ADC1 channel 0 (PA0) with software trigger, single conversion.
 * @return 12-bit raw ADC value (0–4095)
 *
 * Requires hadc1 to be configured by MX_ADC1_Init().
 */
static uint16_t adc_read_ch0(void)
{
    extern ADC_HandleTypeDef hadc1;

    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, 10) != HAL_OK)
    {
        HAL_ADC_Stop(&hadc1);
        return 0;
    }
    uint16_t val = (uint16_t)HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);
    return val;
}

/**
 * @brief  Transmit a null-terminated string via USB CDC with retry on busy.
 * @param  s  Pointer to string
 * @return USBD_OK on success, USBD_BUSY if all retries exhausted
 */
static uint8_t cdc_print(const char *s)
{
    uint8_t len = (uint8_t)strlen(s);
    uint8_t result = USBD_BUSY;

    for (uint8_t i = 0; i < CDC_TX_RETRIES; i++)
    {
        result = CDC_Transmit_FS((uint8_t *)s, len);
        if (result == USBD_OK)
            break;
        HAL_Delay(1);
    }
    return result;
}

/**
 * @brief  Returns non-zero when USB CDC is fully enumerated and configured.
 */
static uint8_t usb_is_ready(void)
{
    extern USBD_HandleTypeDef hUsbDeviceFS;
    return (hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED) ? 1U : 0U;
}

/* ── USER CODE END 0 ───────────────────────────────────────────────────── */


/*
 * ══════════════════════════════════════════════════════════════════════════
 *  PLACE THE FOLLOWING CODE IN THE WHILE(1) LOOP OF main()
 *  i.e. between USER CODE BEGIN WHILE and USER CODE END WHILE
 * ══════════════════════════════════════════════════════════════════════════
 *
 * Also add the init block (labelled below) before the while(1).
 */


/* ── USER CODE BEGIN 2 (one-time init, before while loop) ──────────────── */
/*
    // Wait for USB enumeration (up to 3 seconds)
    uint32_t usb_wait = HAL_GetTick();
    while (!usb_is_ready() && (HAL_GetTick() - usb_wait) < 3000)
    {
        HAL_Delay(10);
    }
    last_tx_tick = HAL_GetTick();
*/
/* ── USER CODE END 2 ───────────────────────────────────────────────────── */


/* ── USER CODE BEGIN WHILE ──────────────────────────────────────────────── */
/*
    while (1)
    {
        uint32_t now = HAL_GetTick();

        // ── Periodic ADC sample + CDC transmit ──────────────────────────
        if ((now - last_tx_tick) >= TX_INTERVAL_MS)
        {
            last_tx_tick = now;

            if (usb_is_ready())
            {
                uint16_t raw  = adc_read_ch0();

                // Convert to millivolts then format as X.XXX V
                uint32_t mv   = ((uint32_t)raw * ADC_VREF_MV) / ADC_RESOLUTION;
                uint32_t v_int   = mv / 1000;
                uint32_t v_frac  = mv % 1000;

                int len = snprintf(tx_str, sizeof(tx_str),
                                   "ADC: %4u | Voltage: %lu.%03luV\r\n",
                                   raw, v_int, v_frac);
                (void)len;

                cdc_print(tx_str);

                // Blink LED on each transmission (PC13 active-low)
                HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
            }
        }

        // ── Echo any received USB CDC data ───────────────────────────────
        if (rx_ready)
        {
            rx_ready = 0;
            if (usb_is_ready() && rx_len > 0)
            {
                // Copy to local buffer so callback can reuse rx_buf
                uint8_t echo_buf[64];
                uint16_t echo_len = rx_len;
                memcpy(echo_buf, (const void *)rx_buf, echo_len);

                // Prefix echo with "ECHO: "
                char prefix[] = "ECHO: ";
                CDC_Transmit_FS((uint8_t *)prefix, sizeof(prefix) - 1);
                HAL_Delay(2);
                CDC_Transmit_FS(echo_buf, echo_len);
            }
        }
    }   // end while(1)
*/
/* ── USER CODE END WHILE ───────────────────────────────────────────────── */


/*
 * ══════════════════════════════════════════════════════════════════════════
 *  RECEIVE CALLBACK — patch into usbd_cdc_if.c
 *  Find CDC_Receive_FS() in usbd_cdc_if.c and add the body below.
 *  The variables rx_buf, rx_len, rx_ready must be declared extern or
 *  moved into usbd_cdc_if.c (simplest: declare them in usbd_cdc_if.c
 *  and extern-declare in main.c).
 * ══════════════════════════════════════════════════════════════════════════
 *
 * In usbd_cdc_if.c, CDC_Receive_FS callback:
 *
 *   extern volatile uint8_t  rx_buf[];
 *   extern volatile uint16_t rx_len;
 *   extern volatile uint8_t  rx_ready;
 *
 *   static int8_t CDC_Receive_FS(uint8_t *Buf, uint32_t *Len)
 *   {
 *       uint16_t copy_len = (*Len > 64) ? 64 : (uint16_t)*Len;
 *       memcpy((void *)rx_buf, Buf, copy_len);
 *       rx_len   = copy_len;
 *       rx_ready = 1;
 *       USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
 *       USBD_CDC_ReceivePacket(&hUsbDeviceFS);
 *       return (USBD_OK);
 *   }
 */

/*
 * ══════════════════════════════════════════════════════════════════════════
 *  COMPLETE STANDALONE main() FOR REFERENCE
 *  (shows where each block fits in the CubeMX skeleton)
 * ══════════════════════════════════════════════════════════════════════════
 *
 * int main(void)
 * {
 *     // USER CODE BEGIN 1
 *     // USER CODE END 1
 *
 *     HAL_Init();
 *     SystemClock_Config();      // configured by CubeMX
 *     MX_GPIO_Init();
 *     MX_ADC1_Init();
 *     MX_USB_DEVICE_Init();      // starts USB CDC stack
 *
 *     // USER CODE BEGIN 2
 *     uint32_t usb_wait = HAL_GetTick();
 *     while (!usb_is_ready() && (HAL_GetTick() - usb_wait) < 3000)
 *         HAL_Delay(10);
 *     last_tx_tick = HAL_GetTick();
 *     // USER CODE END 2
 *
 *     while (1)
 *     {
 *         // USER CODE BEGIN WHILE
 *         uint32_t now = HAL_GetTick();
 *
 *         if ((now - last_tx_tick) >= TX_INTERVAL_MS)
 *         {
 *             last_tx_tick = now;
 *             if (usb_is_ready())
 *             {
 *                 uint16_t raw = adc_read_ch0();
 *                 uint32_t mv  = ((uint32_t)raw * ADC_VREF_MV) / ADC_RESOLUTION;
 *                 snprintf(tx_str, sizeof(tx_str),
 *                          "ADC: %4u | Voltage: %lu.%03luV\r\n",
 *                          raw, mv / 1000, mv % 1000);
 *                 cdc_print(tx_str);
 *                 HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
 *             }
 *         }
 *
 *         if (rx_ready)
 *         {
 *             rx_ready = 0;
 *             if (usb_is_ready() && rx_len > 0)
 *             {
 *                 uint8_t  echo_buf[64];
 *                 uint16_t echo_len = rx_len;
 *                 memcpy(echo_buf, (const void *)rx_buf, echo_len);
 *                 char prefix[] = "ECHO: ";
 *                 CDC_Transmit_FS((uint8_t *)prefix, sizeof(prefix) - 1);
 *                 HAL_Delay(2);
 *                 CDC_Transmit_FS(echo_buf, echo_len);
 *             }
 *         }
 *         // USER CODE END WHILE
 *     }
 * }
 */
