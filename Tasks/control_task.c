#include "task_manager.h"
#include "PWM_Motor.h"
#include "PWM_Humidifier.h"
#include "PWM_LED.h"
#include "Buzzer.h"
#include <stdio.h>

/* 内部逻辑函数 */
static void AutoControlLogic(SensorData_t *data);
static void ExecuteCommand(ControlCommand_t *cmd);

void Control_Task(void *pvParameters)
{
    SensorData_t sensor_data;
    ControlCommand_t cmd;
    
    while(1)
    {
        /* 1. 优先处理控制命令 (非阻塞) */
        while(xQueueReceive(TaskManager_GetControlQueue(), &cmd, 0) == pdPASS)
        {
            ExecuteCommand(&cmd);
        }

        /* 2. 获取最新传感器数据进行自动控制 (阻塞等待，有数据来才动) */
        if(xQueuePeek(TaskManager_GetSensorQueue(), &sensor_data, pdMS_TO_TICKS(100)) == pdPASS)
        {
            SystemStatus_t *status = TaskManager_GetSystemStatus();
            
            /* 只有在正常模式下才自动控制 */
            if(status->current_mode == SYS_MODE_NORMAL) {
                AutoControlLogic(&sensor_data);
            }
        }
        else {
             // 没数据也要延时一下，防止死循环占用CPU
             vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
}

static void AutoControlLogic(SensorData_t *data)
{
    /* 温度控制电机 */
    Motor_TemperatureControl(data->temperature);
    
    /* 湿度控制加湿器 */
    PWM_Humidifier_AutoControl(data->humidity);
    
    /* 光照控制LED */
    LED_AutoControlByLight(data->light_intensity);
    
    /* 空气质量报警 */
    Buzzer_AutoControl(data->air_quality);
}

static void ExecuteCommand(ControlCommand_t *cmd)
{
    switch(cmd->cmd_type)
    {
        case CMD_LED_CONTROL:
            LED_SetBrightness((uint8_t)cmd->param1);
            break;
        case CMD_MOTOR_CONTROL:
            Motor_SetSpeed((uint8_t)cmd->param1);
            break;
        // ... 其他命令 ...
        default:
            break;
    }
}
