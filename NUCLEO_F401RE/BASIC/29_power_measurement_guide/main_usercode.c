// ============================================================
// Nucleo-F401RE BASIC/29 — Power Measurement Test Firmware
// Same as BASIC/20 — reused here as the power test payload
// See README.md for measurement methodology
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
#include <string.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
typedef enum { MODE_ACTIVE, MODE_SLEEP, MODE_STOP, MODE_STANDBY } PowerMode_t;
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
static PowerMode_t mode = MODE_ACTIVE;
static volatile uint8_t btn_press = 0;
static char buf[80];
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static void uart_tx(const char *s) {
    HAL_UART_Transmit(&huart2, (const uint8_t*)s, strlen(s), 200);
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    uart_tx("Power measurement demo\r\nPress B1 to cycle: Active->Sleep->Stop->Standby\r\n");
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        if (btn_press) {
            btn_press = 0;
            mode = (PowerMode_t)((mode + 1) % 4);
        }

        switch (mode) {
            case MODE_ACTIVE:
                HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
                HAL_Delay(500);
                snprintf(buf, sizeof(buf), "ACTIVE 84MHz tick=%lu\r\n", HAL_GetTick());
                uart_tx(buf);
                break;

            case MODE_SLEEP:
                uart_tx("Entering SLEEP (press B1 to wake)\r\n");
                HAL_Delay(100);
                HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
                uart_tx("Woke from SLEEP\r\n");
                mode = MODE_ACTIVE;
                break;

            case MODE_STOP:
                uart_tx("Entering STOP (press B1 to wake)\r\n");
                HAL_Delay(100);
                HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
                SystemClock_Config();
                uart_tx("Woke from STOP\r\n");
                mode = MODE_ACTIVE;
                break;

            case MODE_STANDBY: {
                uart_tx("Entering STANDBY (RTC wakes in 30s)\r\n");
                HAL_Delay(200);
                // Set RTC alarm 30s ahead
                RTC_AlarmTypeDef a = {0};
                RTC_TimeTypeDef t = {0};
                HAL_RTC_GetTime(&hrtc, &t, RTC_FORMAT_BIN);
                HAL_RTC_GetDate(&hrtc, &(RTC_DateTypeDef){0}, RTC_FORMAT_BIN);
                a.AlarmTime.Seconds = (t.Seconds + 30) % 60;
                a.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY|RTC_ALARMMASK_HOURS|RTC_ALARMMASK_MINUTES;
                a.Alarm = RTC_ALARM_A;
                HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A);
                HAL_RTC_SetAlarm_IT(&hrtc, &a, RTC_FORMAT_BIN);
                __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU|PWR_FLAG_SB);
                HAL_PWR_EnterSTANDBYMode();
                break;
            }
        }
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====

// ===== USER CODE BEGIN 4 =====
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_13) btn_press = 1;
}
// ===== USER CODE END 4 =====
