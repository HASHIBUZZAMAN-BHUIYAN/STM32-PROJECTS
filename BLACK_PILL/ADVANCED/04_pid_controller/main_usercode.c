// ============================================================
// Black Pill ADVANCED/04 — PID Controller (Software, 100Hz)
// STM32F411CEU6  |  HAL + CubeMX 6.16
// TIM3 100Hz IRQ, simulated first-order plant, USART1
// ============================================================

// ===== USER CODE BEGIN Includes =====
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>   /* fabsf — FPU accelerated on F411 */
// ===== USER CODE END Includes =====

// ===== USER CODE BEGIN PD =====
#define DT          0.01f
#define MAX_INT     500.0f
#define U_MAX       100.0f
#define U_MIN       0.0f
// ===== USER CODE END PD =====

// ===== USER CODE BEGIN PV =====
static float Kp=2.0f, Ki=0.5f, Kd=0.1f;
static float setpoint=50.0f;
static float plant_y=0.0f;    /* simulated plant state */
static float integral=0.0f, prev_err=0.0f, u_out=0.0f;
static volatile uint32_t pid_tick=0;
static char rx_buf[64]; static uint8_t rx_byte;
static uint8_t rx_idx=0;
static volatile uint8_t cmd_ready=0;
// ===== USER CODE END PV =====

// ===== USER CODE BEGIN 0 =====
static float clampf(float v,float lo,float hi){return v<lo?lo:v>hi?hi:v;}

/* Called from TIM3 IRQ at 100 Hz */
void PID_Step(void) {
    float err = setpoint - plant_y;
    integral = clampf(integral + err*DT, -MAX_INT, MAX_INT);
    float deriv = (err - prev_err) / DT;
    prev_err = err;
    u_out = clampf(Kp*err + Ki*integral + Kd*deriv, U_MIN, U_MAX);
    /* first-order lag: τ≈1s → coeff 0.9/0.1 at 100Hz */
    plant_y = 0.9f*plant_y + 0.1f*u_out;
    pid_tick++;
}

static void process_cmd(char *s) {
    char out[128];
    if (strncmp(s,"set ",4)==0) {
        setpoint = clampf(atof(s+4),0,100);
        snprintf(out,sizeof(out),"OK setpoint=%.2f\r\n",setpoint);
    } else if (strncmp(s,"pid ",4)==0) {
        float a,b,c; sscanf(s+4,"%f %f %f",&a,&b,&c);
        Kp=a; Ki=b; Kd=c;
        snprintf(out,sizeof(out),"OK pid=%.3f,%.3f,%.3f\r\n",Kp,Ki,Kd);
    } else if (strcmp(s,"status")==0) {
        snprintf(out,sizeof(out),"Kp=%.3f Ki=%.3f Kd=%.3f sp=%.2f y=%.2f u=%.2f\r\n",
            Kp,Ki,Kd,setpoint,plant_y,u_out);
    } else if (strcmp(s,"reset")==0) {
        integral=0;prev_err=0;plant_y=0;u_out=0;
        snprintf(out,sizeof(out),"OK reset\r\n");
    } else {
        snprintf(out,sizeof(out),"ERR: set|pid|status|reset\r\n");
    }
    HAL_UART_Transmit(&huart1,(uint8_t*)out,strlen(out),100);
}
// ===== USER CODE END 0 =====

/* In stm32f4xx_it.c TIM3_IRQHandler: */
// void TIM3_IRQHandler(void){ HAL_TIM_IRQHandler(&htim3); }
/* Add to HAL_TIM_PeriodElapsedCallback: */
// if(htim->Instance==TIM3){ PID_Step(); }

// ===== USER CODE BEGIN 2 =====
    HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
    HAL_TIM_Base_Start_IT(&htim3);
    HAL_UART_Transmit(&huart1,(uint8_t*)"PID demo ready\r\n",16,100);
// ===== USER CODE END 2 =====

// ===== USER CODE BEGIN WHILE =====
    uint32_t last_print=0;
    while(1) {
        if(cmd_ready){ cmd_ready=0; process_cmd(rx_buf); }
        uint32_t now=HAL_GetTick();
        if(now-last_print>=100){
            last_print=now;
            char csv[64];
            int n=snprintf(csv,sizeof(csv),"%lu,%.2f,%.2f,%.2f,%.2f\r\n",
                pid_tick,setpoint,plant_y,u_out,setpoint-plant_y);
            HAL_UART_Transmit(&huart1,(uint8_t*)csv,n,50);
        }
    }
// ===== USER CODE END WHILE =====

// ===== USER CODE BEGIN 4 =====
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *h) {
    if(h->Instance==USART1){
        if(rx_byte=='\r'||rx_byte=='\n'){
            if(rx_idx>0){rx_buf[rx_idx]=0;rx_idx=0;cmd_ready=1;}
        } else if(rx_idx<sizeof(rx_buf)-1){
            rx_buf[rx_idx++]=rx_byte;
        }
        HAL_UART_Receive_IT(&huart1,&rx_byte,1);
    }
}
// ===== USER CODE END 4 =====
