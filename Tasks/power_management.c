#include "power_management.h"
#include "task_manager.h"
#include "Power_Mode.h"
#include "config.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Serial.h"
#include <stdio.h>

/* 驱动头文件 */
#include "PWM_Motor.h"
#include "PWM_Humidifier.h"
#include "PWM_LED.h"
#include "Buzzer.h"

static PowerStatus_t power_status = {
    .battery_voltage = 3.7f,
    .battery_level = 100,
    .power_consumption = 0.0f,
    .is_charging = 0,
    .current_mode = POWER_MODE_NORMAL
};

/* 内部函数声明 */
static void Check_BatteryLevel(void);
static void Adjust_PowerMode(void);
static void Calculate_PowerConsumption(void);
static uint8_t Should_EnterLowPower(void);
static uint8_t Check_SystemIdle(void);
static void Enter_LowPowerMode(void);

/**********************************************************
 * @brief  电源管理任务
 **********************************************************/
void PowerManagement_Task(void *pvParameters)
{
    TickType_t xLastWakeTime;
    const TickType_t xCheckPeriod = pdMS_TO_TICKS(5000); 
    
    xLastWakeTime = xTaskGetTickCount();
    
    printf("Power management task started!\r\n");
    
    PowerMode_Init();
    
    while(1) {
        vTaskDelayUntil(&xLastWakeTime, xCheckPeriod);
        
        Check_BatteryLevel();
        Adjust_PowerMode();
        Calculate_PowerConsumption();
        
        if(Should_EnterLowPower()) {
            Enter_LowPowerMode();
        }
    }
}

static void Check_BatteryLevel(void)
{
    if(power_status.battery_level > 0) {
        power_status.battery_level--;
    }
    if(power_status.battery_level < 20) {
        printf("Warning: Low battery! %d%% remaining\r\n", power_status.battery_level);
    }
}

static void Adjust_PowerMode(void)
{
    power_status.current_mode = PowerMode_GetCurrentMode();
    
    if(power_status.battery_level < 30) {
        if(power_status.current_mode != POWER_MODE_SLEEP) {
            PowerMode_SetMode(POWER_MODE_SLEEP);
        }
    } else if(power_status.battery_level < 10) {
        if(power_status.current_mode != POWER_MODE_STOP) {
            PowerMode_SetMode(POWER_MODE_STOP);
        }
    } else {
        if(power_status.current_mode != POWER_MODE_NORMAL) {
            PowerMode_SetMode(POWER_MODE_NORMAL);
        }
    }
}

static void Calculate_PowerConsumption(void)
{
    // 简单模拟功耗计算
}

static uint8_t Should_EnterLowPower(void)
{
    if(Check_SystemIdle() && power_status.battery_level < 40) {
        return 1;
    }
    return 0;
}

static uint8_t Check_SystemIdle(void)
{
    if(Serial_RxFlag) return 0;
    return 1;
}

static void Enter_LowPowerMode(void)
{
    printf("Entering low power mode...\r\n");
    Buzzer_Off();
    Motor_Stop();
    PWM_Humidifier_Off();
    LED_SetBrightness(0);
    PowerMode_SetMode(POWER_MODE_STOP);
}
