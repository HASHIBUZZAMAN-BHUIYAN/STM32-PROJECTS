// ============================================================
// Nucleo-F401RE BASIC/20 — Sleep/Stop/Standby Power Modes
// STM32F401RETx  |  HAL + CubeMX 6.16
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
typedef enum { DEMO_SLEEP, DEMO_STOP, DEMO_STANDBY } DemoMode_t;
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
static DemoMode_t demo_step = DEMO_SLEEP;
static volatile uint8_t woken = 0;
static char buf[64];
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static void uart_print(const char *s)
{
    HAL_UART_Transmit(&huart2, (const uint8_t*)s, strlen(s), 100);
}

static void set_rtc_alarm_seconds(uint8_t secs)
{
    RTC_AlarmTypeDef alarm = {0};
    RTC_TimeTypeDef  time  = {0};
    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
    alarm.AlarmTime.Seconds = (time.Seconds + secs) % 60;
    alarm.AlarmMask         = RTC_ALARMMASK_DATEWEEKDAY | RTC_ALARMMASK_HOURS | RTC_ALARMMASK_MINUTES;
    alarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
    alarm.Alarm              = RTC_ALARM_A;
    HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A);
    HAL_RTC_SetAlarm_IT(&hrtc, &alarm, RTC_FORMAT_BIN);
}
// ===== USER CODE END 0 =====

// ===== USER CODE BEGIN 2 =====
    uart_print("Power mode demo: press B1 to advance\r\n");
    HAL_Delay(2000);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    while (1)
    {
        switch (demo_step) {
            case DEMO_SLEEP:
                uart_print("--- Entering SLEEP (~10mA) --- press B1 to wake\r\n");
                HAL_Delay(100);
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
                HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
                // CPU resumes here after EXTI13 interrupt
                uart_print("Woke from SLEEP\r\n");
                demo_step = DEMO_STOP;
                HAL_Delay(1000);
                break;

            case DEMO_STOP:
                uart_print("--- Entering STOP (~500uA) --- press B1 to wake\r\n");
                HAL_Delay(100);
                HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
                // On wake from Stop: clocks reset to HSI; re-init required
                SystemClock_Config(); // call CubeMX-generated clock config
                uart_print("Woke from STOP (reconfigured clocks)\r\n");
                demo_step = DEMO_STANDBY;
                HAL_Delay(1000);
                break;

            case DEMO_STANDBY:
                uart_print("--- Entering STANDBY (~3uA) --- RTC alarm wakes in 10s\r\n");
                HAL_Delay(200);
                set_rtc_alarm_seconds(10);
                __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU | PWR_FLAG_SB);
                HAL_PWR_EnterSTANDBYMode();
                // Board resets on wakeup from Standby
                break;
        }
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
    }
// ===== USER CODE END 3 =====

// ===== USER CODE BEGIN 4 =====
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_13) {
        woken = 1; // B1 pressed; wakes Sleep and Stop modes
    }
}
// ===== USER CODE END 4 =====
