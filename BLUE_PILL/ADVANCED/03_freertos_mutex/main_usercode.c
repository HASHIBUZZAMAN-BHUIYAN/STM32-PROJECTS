/**
 * @file    main_usercode.c
 * @brief   FreeRTOS Mutex — shared I2C bus protection for Blue Pill (STM32F103C8T6)
 *
 * Paste each labeled block into the matching USER CODE section of the
 * CubeMX-generated main.c.  Do NOT replace CubeMX-generated init calls.
 *
 * Prerequisites (CubeMX):
 *   - FreeRTOS CMSIS_V2, heap >= 4096 bytes
 *   - I2C1 standard mode (PB6 SCL, PB7 SDA)
 *   - USART1 115200 8N1 (PA9 TX, PA10 RX)
 *
 * Devices:
 *   - BMP280 temperature sensor  — I2C addr 0x76 (SDO tied to GND)
 *   - SSD1306 OLED display        — I2C addr 0x3C
 *
 * NOTE: This code is not hardware-tested. Verify on your board.
 */

// ===== USER CODE BEGIN Includes =====
#include "cmsis_os2.h"
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====


// ===== USER CODE BEGIN PD =====
/* BMP280 I2C address (7-bit, shifted left by HAL) */
#define BMP280_ADDR         (0x76 << 1)
#define BMP280_REG_TEMP_MSB  0xFA

/* SSD1306 I2C address */
#define SSD1306_ADDR        (0x3C << 1)

#define I2C_TIMEOUT_MS       50U
#define TASK1_PERIOD_MS     500U
#define TASK2_PERIOD_MS     300U
// ===== USER CODE END PD =====


// ===== USER CODE BEGIN PV =====
osMutexId_t  i2cMutexHandle;
osThreadId_t task1Handle;
osThreadId_t task2Handle;

const osMutexAttr_t i2cMutex_attr = {
    .name      = "i2cMutex",
    .attr_bits = osMutexPrioInherit,   /* prevent priority inversion */
};

const osThreadAttr_t task1_attr = {
    .name       = "I2C_Task1",
    .stack_size = 256 * 4,
    .priority   = osPriorityNormal,
};
const osThreadAttr_t task2_attr = {
    .name       = "I2C_Task2",
    .stack_size = 256 * 4,
    .priority   = osPriorityNormal,
};
// ===== USER CODE END PV =====


// ===== USER CODE BEGIN PFP =====
void I2C_Task1_Entry(void *argument);
void I2C_Task2_Entry(void *argument);
static void UART_Print(const char *msg);
// ===== USER CODE END PFP =====


// ===== USER CODE BEGIN RTOS_MUTEX =====
i2cMutexHandle = osMutexNew(&i2cMutex_attr);
// ===== USER CODE END RTOS_MUTEX =====


// ===== USER CODE BEGIN RTOS_THREADS =====
task1Handle = osThreadNew(I2C_Task1_Entry, NULL, &task1_attr);
task2Handle = osThreadNew(I2C_Task2_Entry, NULL, &task2_attr);
// ===== USER CODE END RTOS_THREADS =====


/* -----------------------------------------------------------------------
   Task Implementations
   ----------------------------------------------------------------------- */

// ===== USER CODE BEGIN 4 =====

/**
 * @brief Print a string over UART1 (blocking).
 *        Called from task context — UART is not shared here.
 *        If UART were shared, it would need its own mutex.
 */
static void UART_Print(const char *msg)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)msg, (uint16_t)strlen(msg), 200);
}

/**
 * @brief I2C_Task1 — reads raw temperature burst from BMP280.
 *        Acquires i2cMutex before any I2C operation.
 */
void I2C_Task1_Entry(void *argument)
{
    (void)argument;
    HAL_StatusTypeDef status;
    uint8_t  regAddr = BMP280_REG_TEMP_MSB;
    uint8_t  rawBuf[3] = {0};
    char     msg[80];

    for (;;)
    {
        /* Acquire the I2C mutex — block up to 200 ms */
        if (osMutexAcquire(i2cMutexHandle, 200) == osOK)
        {
            /* Write register address */
            status = HAL_I2C_Master_Transmit(&hi2c1,
                                              BMP280_ADDR,
                                              &regAddr, 1,
                                              I2C_TIMEOUT_MS);
            if (status == HAL_OK)
            {
                /* Read 3 bytes: temp_msb, temp_lsb, temp_xlsb */
                status = HAL_I2C_Master_Receive(&hi2c1,
                                                 BMP280_ADDR,
                                                 rawBuf, 3,
                                                 I2C_TIMEOUT_MS);
            }

            osMutexRelease(i2cMutexHandle);

            if (status == HAL_OK)
            {
                uint32_t raw = ((uint32_t)rawBuf[0] << 12)
                             | ((uint32_t)rawBuf[1] <<  4)
                             | ((uint32_t)rawBuf[2] >>  4);
                snprintf(msg, sizeof(msg),
                         "[T1] BMP280 temp raw: 0x%06lX\r\n", raw);
            }
            else
            {
                snprintf(msg, sizeof(msg),
                         "[T1] BMP280 read error: %d\r\n", (int)status);
            }
            UART_Print(msg);
        }
        else
        {
            UART_Print("[T1] Mutex timeout!\r\n");
        }

        osDelay(TASK1_PERIOD_MS);
    }
}

/**
 * @brief I2C_Task2 — sends a basic command sequence to SSD1306 OLED.
 *        Acquires i2cMutex before any I2C operation.
 *
 *        The two-byte sequence {0x00, 0xAF} is the SSD1306 display-on command:
 *          0x00 = Co=0, D/C#=0 (command byte follows)
 *          0xAF = Display ON
 */
void I2C_Task2_Entry(void *argument)
{
    (void)argument;
    HAL_StatusTypeDef status;
    /* SSD1306: control byte 0x00 = command stream, 0x40 = data stream */
    uint8_t displayOnCmd[2] = {0x00, 0xAF};
    char    msg[80];

    for (;;)
    {
        if (osMutexAcquire(i2cMutexHandle, 200) == osOK)
        {
            status = HAL_I2C_Master_Transmit(&hi2c1,
                                              SSD1306_ADDR,
                                              displayOnCmd,
                                              sizeof(displayOnCmd),
                                              I2C_TIMEOUT_MS);
            osMutexRelease(i2cMutexHandle);

            if (status == HAL_OK)
            {
                UART_Print("[T2] SSD1306 write OK\r\n");
            }
            else
            {
                snprintf(msg, sizeof(msg),
                         "[T2] SSD1306 error: %d\r\n", (int)status);
                UART_Print(msg);
            }
        }
        else
        {
            UART_Print("[T2] Mutex timeout!\r\n");
        }

        osDelay(TASK2_PERIOD_MS);
    }
}

// ===== USER CODE END 4 =====
