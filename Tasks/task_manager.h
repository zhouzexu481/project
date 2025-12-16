#ifndef __TASK_MANAGER_H
#define __TASK_MANAGER_H

// 1. 包含配置头文件 (获取 SystemStatus_t, SensorData_t 等定义)
#include "config.h"

// 2. 包含 FreeRTOS 核心头文件
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

/* ================== 初始化与启动 ================== */
BaseType_t TaskManager_Init(void);
BaseType_t TaskManager_CreateTasks(void);

/* ================== 数据获取接口 ================== */
QueueHandle_t TaskManager_GetSensorQueue(void);
QueueHandle_t TaskManager_GetControlQueue(void);
SystemStatus_t* TaskManager_GetSystemStatus(void);

/* ================== 任务函数声明 ================== */
void Sensor_Task(void *pvParameters);
void Control_Task(void *pvParameters);
void Display_Task(void *pvParameters);
void Comm_Task(void *pvParameters);

#endif
