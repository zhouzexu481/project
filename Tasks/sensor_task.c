#include "task_manager.h"
#include "AHT20.h"
#include "BH1750.h"
#include "MQ135.h"
#include <stdio.h>

void Sensor_Task(void *pvParameters)
{
    SensorData_t data = {0}; 
    
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(1000); // 1秒采集一次

    while(1)
    {
		/* 读取数据 */
		AHT20_ReadData(&data.temperature, &data.humidity);
		data.light_intensity = BH1750_ReadLightLevel();
		data.air_quality = MQ135_GetPPM();
		
		/*只有在成功读取数据后，才更新队列 */
		/* 注意：配套的队列长度必须为 1 */
		xQueueOverwrite(TaskManager_GetSensorQueue(), &data);
		
		// 可选调试信息
		// printf("Sensor Update: T=%.1f H=%.1f\r\n", data.temperature, data.humidity);
   
        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
}
