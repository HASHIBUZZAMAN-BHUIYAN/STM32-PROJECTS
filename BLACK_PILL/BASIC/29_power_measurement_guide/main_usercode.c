// ============================================================
// Black Pill BASIC/29 — Power Measurement Test Firmware
// STM32F411CEU6  |  HAL + CubeMX 6.16  |  100 MHz
// Same as BASIC/18 — used as power measurement payload
// See README.md for measurement methodology
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <string.h>
#include <stdio.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
typedef enum { PWR_ACTIVE, PWR_SLEEP, PWR_STOP, PWR_STANDBY } PwrState_t;
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
static PwrState_t state = PWR_ACTIVE;
static volatile uint8_t btn_flag = 0;
static char buf[64];
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static void uart_tx(const char *s) {
    HAL_UART_Transmit(&huart1, (const uint8_t*)s, strlen(s), 200);
}

static void rtc_alarm_30s(void) {
    RTC_TimeTypeDef t={0}; RTC_DateTypeDef d={0};
    HAL_RTC_GetTime(&hrtc, &t, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &d, RTC_FORMAT_BIN);
    RTC_AlarmTypeDef a={0};
    a.AlarmTime.Seconds=(t.Seconds+30)%60;
    a.AlarmMask=RTC_ALARMMASK_DATEWEEKDAY|RTC_ALARMMASK_HOURS|RTC_ALARMMASK_MINUTES;
    a.Alarm=RTC_ALARM_A;
    HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A);
    HAL_RTC_SetAlarm_IT(&hrtc, &a, RTC_FORMAT_BIN);
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    uart_tx("Black Pill power measurement firmware\r\nPress PA0 to cycle modes\r\n");
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        if (btn_flag) {
            btn_flag = 0;
            state = (PwrState_t)((state + 1) % 4);
        }
        switch (state) {
            case PWR_ACTIVE:
                HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
                snprintf(buf, sizeof(buf), "ACTIVE 100MHz t=%lu\r\n", HAL_GetTick());
                uart_tx(buf);
                HAL_Delay(500);
                break;
            case PWR_SLEEP:
                uart_tx("SLEEP (PA0 to wake)\r\n");
                HAL_Delay(10);
                HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
                uart_tx("Woke SLEEP\r\n");
                state = PWR_ACTIVE;
                break;
            case PWR_STOP:
                uart_tx("STOP (PA0 to wake)\r\n");
                HAL_Delay(10);
                HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
                SystemClock_Config();
                uart_tx("Woke STOP\r\n");
                state = PWR_ACTIVE;
                break;
            case PWR_STANDBY:
                uart_tx("STANDBY (RTC 30s)\r\n");
                HAL_Delay(100);
                rtc_alarm_30s();
                __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU|PWR_FLAG_SB);
                HAL_PWR_EnterSTANDBYMode();
                break;
        }
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====

// ===== USER CODE BEGIN 4 =====
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_0) btn_flag = 1;
}
// ===== USER CODE END 4 =====
