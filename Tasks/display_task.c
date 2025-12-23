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
    char str_buf[20];
    
    OLED_Init();
    
    while(1)
    {
        /* 从队列窥探数据 (只读不取) */
        if(xQueuePeek(TaskManager_GetSensorQueue(), &data, 0) == pdPASS)
        {
            // === 1. 清空显存 (Frame Buffer) ===
            // 这步在 RAM 中进行，极快，屏幕不会闪
            OLED_Clear(); 
            
            // === 2. 绘制内容到显存 ===
            
            // --- 第一行 ---
            sprintf(str_buf, "Air:%4.2f", data.air_quality);
            OLED_ShowString(1, 1, str_buf);
            
            if(Buzzer_GetState())
                sprintf(str_buf, "Bz:ON ");
            else
                sprintf(str_buf, "Bz:OFF");
            OLED_ShowString(1, 10, str_buf); 

            // --- 第二行 ---
            sprintf(str_buf, "Tmp:%4.1f", data.temperature);
            OLED_ShowString(2, 1, str_buf);
            
            sprintf(str_buf, "Fan:%3d", Get_Motor_Speed());
            OLED_ShowString(2, 10, str_buf);

            // --- 第三行 ---
            sprintf(str_buf, "Hum:%4.1f", data.humidity);
            OLED_ShowString(3, 1, str_buf);
            
            sprintf(str_buf, "Mis:%3d", PWM_Humidifier_GetCurrentDuty());
            OLED_ShowString(3, 10, str_buf);
            
            // --- 第四行 ---
            sprintf(str_buf, "Lux:%4.0f", data.light_intensity);
            OLED_ShowString(4, 1, str_buf);
            
            sprintf(str_buf, "Led:%3d", LED_GetBrightness());
            OLED_ShowString(4, 10, str_buf);
            
            // === 3. 统一刷新 (Refresh) ===
            // 将显存一次性推送到 OLED，实现帧同步
            OLED_Refresh_Gram();
        }
        
        vTaskDelay(pdMS_TO_TICKS(500)); // 500ms 刷新一次
    }
}
