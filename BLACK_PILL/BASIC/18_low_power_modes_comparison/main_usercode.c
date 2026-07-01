// ============================================================
// Black Pill BASIC/18 — Low Power Modes Comparison
// STM32F411CEU6  |  HAL + CubeMX 6.16  |  100 MHz
// PA0=EXTI button, PC13=LED, USART1, RTC Alarm A for Standby
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <string.h>
#include <stdio.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
typedef enum { MODE_ACTIVE, MODE_SLEEP, MODE_STOP, MODE_STANDBY } PwrMode_t;
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
static PwrMode_t mode = MODE_ACTIVE;
static volatile uint8_t wakeup_flag = 0;
static char buf[64];
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static void uart_tx(const char *s) {
    HAL_UART_Transmit(&huart1, (const uint8_t*)s, strlen(s), 200);
}

static void rtc_alarm_set_30s(void) {
    RTC_TimeTypeDef t = {0};
    RTC_DateTypeDef d = {0};
    HAL_RTC_GetTime(&hrtc, &t, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &d, RTC_FORMAT_BIN);
    RTC_AlarmTypeDef a = {0};
    a.AlarmTime.Seconds = (t.Seconds + 30) % 60;
    a.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY | RTC_ALARMMASK_HOURS | RTC_ALARMMASK_MINUTES;
    a.Alarm = RTC_ALARM_A;
    HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A);
    HAL_RTC_SetAlarm_IT(&hrtc, &a, RTC_FORMAT_BIN);
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET); // LED off (active-low)
    uart_tx("Black Pill Low Power demo\r\nPress PA0 to cycle modes\r\n");
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        if (wakeup_flag) {
            wakeup_flag = 0;
            mode = (PwrMode_t)((mode + 1) % 4);
        }

        switch (mode) {
            case MODE_ACTIVE:
                HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
                HAL_Delay(500);
                snprintf(buf, sizeof(buf), "ACTIVE 100MHz tick=%lu\r\n", HAL_GetTick());
                uart_tx(buf);
                break;

            case MODE_SLEEP:
                uart_tx("Entering SLEEP (PA0 to wake)\r\n");
                HAL_Delay(10);
                HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
                uart_tx("Woke from SLEEP\r\n");
                mode = MODE_ACTIVE;
                break;

            case MODE_STOP:
                uart_tx("Entering STOP (PA0 to wake)\r\n");
                HAL_Delay(10);
                HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
                SystemClock_Config(); // restore 100MHz after Stop
                uart_tx("Woke from STOP\r\n");
                mode = MODE_ACTIVE;
                break;

            case MODE_STANDBY:
                uart_tx("Entering STANDBY (RTC alarm in 30s)\r\n");
                HAL_Delay(100);
                rtc_alarm_set_30s();
                __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU | PWR_FLAG_SB);
                HAL_PWR_EnterSTANDBYMode();
                break; // unreachable
        }
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====

// ===== USER CODE BEGIN 4 =====
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_0) wakeup_flag = 1;
}
// ===== USER CODE END 4 =====
