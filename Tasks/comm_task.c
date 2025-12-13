#include "comm_task.h"
#include "task_manager.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "Serial.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/**********************************************************
 * @brief  通信任务
 **********************************************************/
void Comm_Task(void *pvParameters)
{
    SensorData_t sensor_data;
    
    printf("Comm task started!\r\n");
    
    while(1) {
        /* 检查是否有接收标志 */
        if(Serial_RxFlag == 1) {
            Serial_RxFlag = 0;
            /* 处理接收到的命令 */
            Process_SerialCommand(Serial_RxPacket);
        }
        
        /* 检查是否需要发送数据 */
        if(xQueuePeek(TaskManager_GetSensorQueue(), &sensor_data, 0) == pdPASS) {
            /* 修复警告: air_quality 是 float 类型，使用 %.0f 打印 (或者强转为 int) */
            printf("[Data] Temp:%.1f C, Hum:%.1f %%, Light:%.1f Lux, Air:%.0f PPM\r\n", 
                   sensor_data.temperature, 
                   sensor_data.humidity, 
                   sensor_data.light_intensity, 
                   sensor_data.air_quality);
        }
        
        vTaskDelay(pdMS_TO_TICKS(1000)); // 建议改为 1000ms，避免串口刷屏过快
    }
}

/**********************************************************
 * @brief  处理串口命令
 **********************************************************/
static void Process_SerialCommand(char *cmd)
{
    ControlCommand_t control_cmd; 
    char *token;
    
    printf("Received command: %s\r\n", cmd);
    
    /* 解析命令 */
    token = strtok(cmd, " ");
    
    if(token != NULL) {
        if(strcmp(token, "LED") == 0) {
            token = strtok(NULL, " ");
            if(token != NULL) {
                control_cmd.cmd_type = CMD_LED_CONTROL;
                control_cmd.param1 = atoi(token);
                xQueueSend(TaskManager_GetControlQueue(), &control_cmd, 0);
            }
        }
        else if(strcmp(token, "MOTOR") == 0) {
            token = strtok(NULL, " ");
            if(token != NULL) {
                control_cmd.cmd_type = CMD_MOTOR_CONTROL;
                control_cmd.param1 = atoi(token);
                xQueueSend(TaskManager_GetControlQueue(), &control_cmd, 0);
            }
        }
        else if(strcmp(token, "MODE") == 0) {
            token = strtok(NULL, " ");
            if(token != NULL) {
                control_cmd.cmd_type = CMD_SYSTEM_MODE;
                control_cmd.param1 = atoi(token);
                xQueueSend(TaskManager_GetControlQueue(), &control_cmd, 0);
            }
        }
        else if(strcmp(token, "HELP") == 0) {
            Show_HelpInfo();
        }
        else if(strcmp(token, "STATUS") == 0) {
            printf("Use display task to show system status.\r\n");
        }
        else {
            printf("Unknown command. Type HELP for help.\r\n");
        }
    }
}

static void Show_HelpInfo(void)
{
    printf("\r\nAvailable commands:\r\n");
    printf("LED <0-100>     - Set LED brightness\r\n");
    printf("MOTOR <0-100>   - Set motor speed\r\n");
    printf("MODE <0-3>      - Set system mode\r\n");
    printf("HELP            - Show this help\r\n");
    printf("STATUS          - Show system status\r\n");
}
