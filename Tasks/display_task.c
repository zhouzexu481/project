#include "task_manager.h"
#include "OLED.h"
#include <stdio.h>

/* 引入所有硬件头文件以获取状态 */
#include "PWM_Motor.h"      // Get_Motor_Speed()
#include "PWM_Humidifier.h" // PWM_Humidifier_GetCurrentDuty()
#include "PWM_LED.h"        // LED_GetBrightness()
#include "Buzzer.h"         // Buzzer_GetState()

void Display_Task(void *pvParameters)
{
    SensorData_t data;
    char str_buf[20]; // 缓冲区
    
    OLED_Init();
    OLED_Clear();
    
    while(1)
    {
        /* 获取传感器数据 */
        if(xQueuePeek(TaskManager_GetSensorQueue(), &data, 0) == pdPASS)
        {
            // === 第一行: 空气质量 & 蜂鸣器 ===
            // 左: Air:1234
            sprintf(str_buf, "Air:%4.2f", data.air_quality);
            OLED_ShowString(1, 1, str_buf);
            
            // 右: Bz:ON / OFF
            if(Buzzer_GetState())
			{
				sprintf(str_buf, "Bz:ON ");
			}
            else
			{
				sprintf(str_buf, "Bz:OFF");
			}
            OLED_ShowString(1, 10, str_buf); // 第10列开始显示右边 (10*8=80像素位置)

            // === 第二行: 温度 & 风扇 ===
            // 左: Tmp:25.5
            sprintf(str_buf, "Tmp:%4.1f", data.temperature);
            OLED_ShowString(2, 1, str_buf);
            
            // 右: Fan:100
            sprintf(str_buf, "Fan:%3d", Get_Motor_Speed());
            OLED_ShowString(2, 10, str_buf);

            // === 第三行: 湿度 & 加湿器 ===
            // 左: Hum:60.5
            sprintf(str_buf, "Hum:%4.1f", data.humidity);
            OLED_ShowString(3, 1, str_buf);
            
            // 右: Mis:100
            sprintf(str_buf, "Mis:%3d", PWM_Humidifier_GetCurrentDuty());
            OLED_ShowString(3, 10, str_buf);
            
            // === 第四行: 光照 & LED ===
            // 左: Lux:1234
            sprintf(str_buf, "Lux:%4.0f", data.light_intensity);
            OLED_ShowString(4, 1, str_buf);
            
            // 右: Led:100
            sprintf(str_buf, "Led:%3d", LED_GetBrightness());
            OLED_ShowString(4, 10, str_buf);
        }
        
        vTaskDelay(pdMS_TO_TICKS(500)); // 0.5秒刷新一次
    }
}
