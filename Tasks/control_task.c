#include "task_manager.h"
#include "PWM_Motor.h"
#include "PWM_WaterPump.h"  // 【修正】与底层驱动文件名保持一致
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
					LED_SetBrightness((uint8_t)cmd.param); 
				break;
				
                case CMD_MOTOR_CONTROL:
					Motor_SetSpeed((uint8_t)cmd.param);
				break;
				
				case CMD_PWM_WaterPump_CONTROL:
					PWM_WaterPump_SetDutyCycle((uint8_t)cmd.param); // 【修正】函数名与驱动一致
				break;

				case CMD_BUZZER_CONTROL:
				if(cmd.param > 0) 
				{
					Buzzer_On();
				}
				else 
				{
					Buzzer_Off();
				}
				break;
				
				case CMD_SYSTEM_MODE:
                    /* 读取指针，修改当前模式的值 */
                    *TaskManager_GetSystemMode() = (SystemMode_t)cmd.param;
                break;
                // ... 其他命令加在这里
            }
        }

        /* 2. 自动控制逻辑 */
        // 参数3设为0：只看一眼，绝不等待，防止卡顿
        if(xQueuePeek(TaskManager_GetSensorQueue(), &data, 0) == pdPASS)
        {
            // 如果系统处于正常模式，执行自动控制
            if( *TaskManager_GetSystemMode() == SYS_MODE_AUTO)
            {
	            Motor_TemperatureControl(data.temperature);      // 温控风扇
                
                //将烟雾浓度和湿度同时传递给水泵控制器
                PWM_WaterPump_AutoControl(data.smoke_level, data.humidity);    
                
                LED_AutoControlByLight(data.light_intensity);    // 光控灯
                Buzzer_AutoControl(data.smoke_level);            // 烟雾报警
            }
        }
		
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
