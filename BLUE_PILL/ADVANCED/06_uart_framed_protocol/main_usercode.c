/**
 * main_usercode.c — 06_uart_framed_protocol
 *
 * Custom binary framing protocol over UART1.
 * Frame: [0xAA][LEN][CMD][PAYLOAD...][CRC8]
 * Commands: 0x01=GET_ADC, 0x02=SET_LED, 0x03=GET_TEMP
 * CRC8 polynomial 0x07.
 *
 * Paste each section into the corresponding USER CODE block in CubeMX-generated main.c.
 * This file does NOT compile standalone — it requires CubeMX-generated handles:
 *   huart1, hadc1, hi2c1
 *
 * NOT tested on hardware. Reference implementation only.
 */

// ===== USER CODE BEGIN Includes =====
#include <string.h>
#include <stdint.h>
// ===== USER CODE END Includes =====


// ===== USER CODE BEGIN PD (Private Defines) =====
#define FRAME_SOF           0xAAU
#define CMD_GET_ADC         0x01U
#define CMD_SET_LED         0x02U
#define CMD_GET_TEMP        0x03U
#define CMD_NACK            0xFFU

#define MAX_PAYLOAD         16U
#define FRAME_TIMEOUT_MS    100U

/* BMP280 I2C address (SDO tied to GND) */
#define BMP280_ADDR         (0x76U << 1)
#define BMP280_REG_TEMP_MSB 0xFAU
#define BMP280_REG_CTRL     0xF4U
#define BMP280_REG_CALIB    0x88U

/* Onboard LED is on PC13, ACTIVE-LOW */
#define LED_ON()   HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET)
#define LED_OFF()  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET)
// ===== USER CODE END PD =====


// ===== USER CODE BEGIN PM (Private Macros) =====
// ===== USER CODE END PM =====


// ===== USER CODE BEGIN PV (Private Variables) =====

/* ---- Frame parser state machine ---- */
typedef enum {
    PARSE_WAIT_SOF = 0,
    PARSE_WAIT_LEN,
    PARSE_WAIT_CMD,
    PARSE_WAIT_PAYLOAD,
    PARSE_WAIT_CRC
} ParseState_t;

typedef struct {
    ParseState_t state;
    uint8_t      len;       /* LEN field from frame           */
    uint8_t      cmd;       /* CMD byte                       */
    uint8_t      payload[MAX_PAYLOAD];
    uint8_t      payload_idx;
    uint8_t      payload_remaining;
    uint32_t     last_rx_tick; /* for partial-frame timeout   */
} FrameParser_t;

static FrameParser_t g_parser;

/* Single-byte RX buffer for interrupt-driven HAL_UART_Receive_IT */
static uint8_t g_rx_byte;

/* Flag set inside RxCplt callback; processed in main loop */
static volatile uint8_t g_frame_ready = 0;
static uint8_t g_frame_cmd;
static uint8_t g_frame_payload[MAX_PAYLOAD];
static uint8_t g_frame_payload_len;

/* BMP280 trimming parameters (read once at init) */
static uint16_t g_dig_T1;
static int16_t  g_dig_T2;
static int16_t  g_dig_T3;
// ===== USER CODE END PV =====


// ===== USER CODE BEGIN 0 (Private function prototypes & implementations before main) =====

/**
 * CRC8 over 'len' bytes of 'data' using polynomial 0x07.
 * Initial value 0x00, no final XOR.
 */
static uint8_t crc8(const uint8_t *data, uint8_t len)
{
    uint8_t crc = 0x00U;
    for (uint8_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t bit = 0; bit < 8U; bit++) {
            if (crc & 0x80U) {
                crc = (uint8_t)((crc << 1U) ^ 0x07U);
            } else {
                crc <<= 1U;
            }
        }
    }
    return crc;
}

/**
 * Build and transmit a framed response over UART1.
 * Frame: [0xAA][LEN][CMD][PAYLOAD...][CRC8]
 * LEN = 1 (cmd) + payload_len
 */
static void send_frame(uint8_t cmd, const uint8_t *payload, uint8_t payload_len)
{
    /* Maximum response frame: SOF(1) + LEN(1) + CMD(1) + MAX_PAYLOAD(16) + CRC(1) = 20 bytes */
    uint8_t buf[20];
    uint8_t idx = 0;

    uint8_t total_len = 1U + payload_len; /* CMD + PAYLOAD */

    buf[idx++] = FRAME_SOF;
    buf[idx++] = total_len;
    buf[idx++] = cmd;

    if (payload != NULL && payload_len > 0U) {
        memcpy(&buf[idx], payload, payload_len);
        idx += payload_len;
    }

    /* CRC over LEN..last_payload_byte */
    uint8_t crc = crc8(&buf[1], (uint8_t)(idx - 1U));
    buf[idx++] = crc;

    /* Blocking transmit with 50 ms timeout */
    HAL_UART_Transmit(&huart1, buf, idx, 50U);
}

/**
 * Feed one received byte into the frame parser.
 * When a complete, valid frame is received, sets g_frame_ready.
 */
static void parser_feed(uint8_t byte)
{
    /* Partial frame timeout check */
    if (g_parser.state != PARSE_WAIT_SOF) {
        if ((HAL_GetTick() - g_parser.last_rx_tick) > FRAME_TIMEOUT_MS) {
            g_parser.state = PARSE_WAIT_SOF;
        }
    }
    g_parser.last_rx_tick = HAL_GetTick();

    switch (g_parser.state) {
        case PARSE_WAIT_SOF:
            if (byte == FRAME_SOF) {
                g_parser.state = PARSE_WAIT_LEN;
            }
            /* else: discard — resync */
            break;

        case PARSE_WAIT_LEN:
            /* LEN includes CMD byte; so payload bytes = LEN - 1 */
            if (byte == 0U || byte > (MAX_PAYLOAD + 1U)) {
                /* Invalid length — resync */
                g_parser.state = PARSE_WAIT_SOF;
            } else {
                g_parser.len = byte;
                g_parser.state = PARSE_WAIT_CMD;
            }
            break;

        case PARSE_WAIT_CMD:
            g_parser.cmd = byte;
            g_parser.payload_idx = 0U;
            g_parser.payload_remaining = g_parser.len - 1U; /* bytes after CMD */
            if (g_parser.payload_remaining == 0U) {
                g_parser.state = PARSE_WAIT_CRC;
            } else {
                g_parser.state = PARSE_WAIT_PAYLOAD;
            }
            break;

        case PARSE_WAIT_PAYLOAD:
            g_parser.payload[g_parser.payload_idx++] = byte;
            g_parser.payload_remaining--;
            if (g_parser.payload_remaining == 0U) {
                g_parser.state = PARSE_WAIT_CRC;
            }
            break;

        case PARSE_WAIT_CRC: {
            /* Build the buffer that was CRC'd: [LEN][CMD][PAYLOAD...] */
            uint8_t crc_buf[MAX_PAYLOAD + 2U];
            crc_buf[0] = g_parser.len;
            crc_buf[1] = g_parser.cmd;
            uint8_t payload_count = g_parser.len - 1U;
            memcpy(&crc_buf[2], g_parser.payload, payload_count);

            uint8_t expected_crc = crc8(crc_buf, (uint8_t)(2U + payload_count));

            if (byte == expected_crc) {
                /* Valid frame — hand off to main loop */
                g_frame_cmd = g_parser.cmd;
                g_frame_payload_len = payload_count;
                memcpy(g_frame_payload, g_parser.payload, payload_count);
                g_frame_ready = 1U;
            } else {
                /* CRC mismatch — send NACK */
                uint8_t nack_payload[1] = { byte }; /* echo bad CRC */
                send_frame(CMD_NACK, nack_payload, 1U);
            }
            g_parser.state = PARSE_WAIT_SOF;
            break;
        }

        default:
            g_parser.state = PARSE_WAIT_SOF;
            break;
    }
}

/**
 * Trigger a single ADC conversion and return the 12-bit result.
 * Returns 0xFFFF on failure.
 */
static uint16_t adc_read_ch0(void)
{
    if (HAL_ADC_Start(&hadc1) != HAL_OK) {
        return 0xFFFFU;
    }
    if (HAL_ADC_PollForConversion(&hadc1, 10U) != HAL_OK) {
        HAL_ADC_Stop(&hadc1);
        return 0xFFFFU;
    }
    uint16_t val = (uint16_t)HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);
    return val;
}

/**
 * Read BMP280 trimming parameters from calibration registers (0x88–0x8D).
 * Must be called once after I2C and BMP280 are ready.
 */
static HAL_StatusTypeDef bmp280_read_calib(void)
{
    uint8_t calib[6];
    uint8_t reg = BMP280_REG_CALIB;

    HAL_StatusTypeDef st = HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, &reg, 1U, 10U);
    if (st != HAL_OK) { return st; }

    st = HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, calib, 6U, 10U);
    if (st != HAL_OK) { return st; }

    g_dig_T1 = (uint16_t)(calib[0] | ((uint16_t)calib[1] << 8U));
    g_dig_T2 = (int16_t)(calib[2] | ((int16_t)calib[3] << 8U));
    g_dig_T3 = (int16_t)(calib[4] | ((int16_t)calib[5] << 8U));

    return HAL_OK;
}

/**
 * Read BMP280 temperature.
 * Forces a single measurement (osrs_t=1, mode=forced).
 * Returns temperature in units of 0.01 degC (e.g. 2512 = 25.12 C).
 * Returns INT32_MIN on error.
 */
static int32_t bmp280_read_temp(void)
{
    /* Set forced mode, temperature oversampling x1 */
    uint8_t ctrl_buf[2] = { BMP280_REG_CTRL, 0x25U }; /* osrs_t=001, mode=01 */
    if (HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, ctrl_buf, 2U, 10U) != HAL_OK) {
        return INT32_MIN;
    }

    HAL_Delay(10U); /* Wait for measurement (~6 ms typical) */

    uint8_t reg = BMP280_REG_TEMP_MSB;
    uint8_t raw[3];
    if (HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, &reg, 1U, 10U) != HAL_OK) {
        return INT32_MIN;
    }
    if (HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, raw, 3U, 10U) != HAL_OK) {
        return INT32_MIN;
    }

    /* Raw 20-bit value */
    int32_t adc_T = (int32_t)(((uint32_t)raw[0] << 12U) |
                               ((uint32_t)raw[1] << 4U)  |
                               ((uint32_t)raw[2] >> 4U));

    /* BMP280 compensation formula (from datasheet) */
    int32_t var1 = (int32_t)((adc_T / 8) - ((int32_t)g_dig_T1 * 2));
    var1 = (var1 * (int32_t)g_dig_T2) / 2048;

    int32_t var2 = (int32_t)((adc_T / 16) - (int32_t)g_dig_T1);
    var2 = (((var2 * var2) / 4096) * (int32_t)g_dig_T3) / 16384;

    int32_t t_fine = var1 + var2;
    int32_t temp_cdeg = (t_fine * 5 + 128) / 256; /* in 0.01 degC units */

    return temp_cdeg;
}

/**
 * Process a fully-parsed, CRC-validated frame.
 */
static void process_frame(uint8_t cmd, const uint8_t *payload, uint8_t payload_len)
{
    switch (cmd) {
        case CMD_GET_ADC: {
            uint16_t adc_val = adc_read_ch0();
            uint8_t resp[2];
            resp[0] = (uint8_t)(adc_val & 0xFFU);        /* LSB */
            resp[1] = (uint8_t)((adc_val >> 8U) & 0xFFU); /* MSB */
            send_frame(CMD_GET_ADC, resp, 2U);
            break;
        }

        case CMD_SET_LED: {
            if (payload_len < 1U) {
                send_frame(CMD_NACK, NULL, 0U);
                break;
            }
            if (payload[0] != 0U) {
                LED_ON();
            } else {
                LED_OFF();
            }
            uint8_t resp[1] = { payload[0] };
            send_frame(CMD_SET_LED, resp, 1U);
            break;
        }

        case CMD_GET_TEMP: {
            int32_t temp = bmp280_read_temp();
            uint8_t resp[4];
            resp[0] = (uint8_t)((uint32_t)temp & 0xFFU);
            resp[1] = (uint8_t)(((uint32_t)temp >> 8U) & 0xFFU);
            resp[2] = (uint8_t)(((uint32_t)temp >> 16U) & 0xFFU);
            resp[3] = (uint8_t)(((uint32_t)temp >> 24U) & 0xFFU);
            send_frame(CMD_GET_TEMP, resp, 4U);
            break;
        }

        default:
            /* Unknown command — NACK */
            send_frame(CMD_NACK, NULL, 0U);
            break;
    }
}
// ===== USER CODE END 0 =====


/* --------------------------------------------------------------------------
 * Place the following inside the main() function at the indicated locations.
 * -------------------------------------------------------------------------- */

// ===== USER CODE BEGIN 2 (after MX_xxx_Init calls, before while loop) =====
/*
    memset(&g_parser, 0, sizeof(g_parser));
    g_parser.state = PARSE_WAIT_SOF;

    // Read BMP280 calibration data (non-fatal if sensor absent)
    bmp280_read_calib();

    // Arm the first byte reception
    HAL_UART_Receive_IT(&huart1, &g_rx_byte, 1U);
*/
// ===== USER CODE END 2 =====


// ===== USER CODE BEGIN WHILE =====
/*
    while (1) {
        if (g_frame_ready) {
            g_frame_ready = 0U;
            process_frame(g_frame_cmd, g_frame_payload, g_frame_payload_len);
        }
    }
*/
// ===== USER CODE END WHILE =====


/* --------------------------------------------------------------------------
 * HAL UART RX Complete Callback — place in main.c outside main(), in the
 * USER CODE BEGIN 4 section or a separate file.
 * -------------------------------------------------------------------------- */

// ===== USER CODE BEGIN 4 =====
/*
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        parser_feed(g_rx_byte);
        // Re-arm for next byte
        HAL_UART_Receive_IT(&huart1, &g_rx_byte, 1U);
    }
}
*/
// ===== USER CODE END 4 =====
