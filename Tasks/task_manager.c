//创建队列,创建任务

#include "task_manager.h"
#include <stdio.h>

/* 私有句柄定义 */
static QueueHandle_t xSensorQueue = NULL;
static QueueHandle_t xControlQueue = NULL;

// 初始化系统状态
static SystemMode_t current_mode = SYS_MODE_AUTO;

/* 初始化FreeRTOS对象 */
BaseType_t TaskManager_Init(void)
{
    /* 创建队列 */
    /* 使用 xQueueOverwrite 时，队列长度必须为 1 */
    xSensorQueue = xQueueCreate(1, sizeof(SensorData_t)); 
    xControlQueue = xQueueCreate(10, sizeof(ControlCommand_t)); 
    
    if (xSensorQueue == NULL || xControlQueue == NULL) {
        printf("Error: Queue Create Failed!\r\n");
        return pdFAIL;
    }

    return pdPASS;
}

/* 创建任务 */
BaseType_t TaskManager_CreateTasks(void)
{
    BaseType_t xReturn = pdPASS;

    /* 传感器任务 */
    xReturn = xTaskCreate(Sensor_Task, "SensorTask", SENSOR_TASK_STACK_SIZE, NULL, SENSOR_TASK_PRIORITY, NULL);
    if(xReturn != pdPASS) return pdFAIL;

    /* 控制任务 */
    xReturn = xTaskCreate(Control_Task, "ControlTask", CONTROL_TASK_STACK_SIZE, NULL, CONTROL_TASK_PRIORITY, NULL);
    if(xReturn != pdPASS) return pdFAIL;

    /* 显示任务 (OLED) */
    /* OLED 显示复杂内容,可改为256 */
    xReturn = xTaskCreate(Display_Task, "DisplayTask", DISPLAY_TASK_STACK_SIZE, NULL, DISPLAY_TASK_PRIORITY, NULL);
    if(xReturn != pdPASS) return pdFAIL;

    /* 通信任务 (Serial) */
    /* 512 words (2KB) 堆栈，防止 printf %f 导致溢出 === */
    xReturn = xTaskCreate(Comm_Task, "CommTask", COMM_TASK_STACK_SIZE, NULL, COMM_TASK_PRIORITY, NULL);
    if(xReturn != pdPASS) return pdFAIL;

    printf("All Tasks Created Successfully!\r\n");
    return pdPASS;
}

/* 接口实现保持不变 ... */
QueueHandle_t TaskManager_GetSensorQueue(void) { return xSensorQueue; }
QueueHandle_t TaskManager_GetControlQueue(void) { return xControlQueue; }
SystemMode_t* TaskManager_GetSystemMode(void) { return &current_mode; }
