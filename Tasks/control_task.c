#include "task_manager.h"
#include "PWM_Motor.h"
#include "PWM_Humidifier.h"
#include "PWM_LED.h"
#include "Buzzer.h"

void Control_Task(void *pvParameters)
{
    SensorData_t data;
    ControlCommand_t cmd;
    
    while(1)
    {
        /* 1.处理控制命令 */
        while(xQueueReceive(TaskManager_GetControlQueue(), &cmd, 0) == pdPASS)
        {
            switch(cmd.cmd_type)
            {
                case CMD_LED_CONTROL:
					LED_SetBrightness((uint8_t)cmd.param1); 
				break;
				
                case CMD_MOTOR_CONTROL:
					Motor_SetSpeed((uint8_t)cmd.param1);
				break;
                // ... 其他命令加在这里
            }
        }

        /* 2. 自动控制逻辑 (有最新数据就执行，没有就跳过) */
        // 参数3设为0：只看一眼，绝不等待，防止卡顿
        if(xQueuePeek(TaskManager_GetSensorQueue(), &data, 0) == pdPASS)
        {
            // 如果系统处于正常模式，执行自动控制
            if(TaskManager_GetSystemStatus()->current_mode == SYS_MODE_NORMAL) 
            {
                Motor_TemperatureControl(data.temperature);      // 温控风扇
                PWM_Humidifier_AutoControl(data.humidity);       // 湿控加湿
                LED_AutoControlByLight(data.light_intensity);    // 光控灯
                Buzzer_AutoControl(data.air_quality);            // 气报警
            }
        }
		
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
