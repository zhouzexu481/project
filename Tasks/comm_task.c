#include "comm_task.h"
#include "task_manager.h"
#include "serial.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* 私有函数声明 (只在本文件内部使用) */
static void Process_Command(char *cmd_str);
static void Show_Help(void);

/**
 * @brief  通信主任务
 * 1. 处理串口收到的指令
 * 2. 定时向串口打印传感器数据
 */
void Comm_Task(void *pvParameters)
{
    SensorData_t data;
    
    printf("System Started! Waiting for commands...\r\n");
    
    while(1) 
    {
        /* 1. 检查有没有收到串口数据 */
        if(Serial_RxFlag == 1) 
        {
            Serial_RxFlag = 0; // 清除标志位，防止重复处理
            Process_Command(Serial_RxPacket); // 处理这条命令
        }
        
        /* 2. 定时发送传感器数据 (Peek模式: 查看数据但不取走, 方便Display任务也用) */
        if(xQueuePeek(TaskManager_GetSensorQueue(), &data, 0) == pdPASS) 
        {
            // 格式化打印：%.1f 表示保留1位小数
            printf("[Status] T:%.1fC  H:%.1f%%  Lux:%.0f  Air:%.0f\r\n", 
                   data.temperature, 
                   data.humidity, 
                   data.light_intensity, 
                   data.air_quality);
        }
        
        // 延时 1000ms，也就是说每秒打印一次数据
        vTaskDelay(pdMS_TO_TICKS(1000)); 
    }
}

/**
 * @brief  处理串口文本命令
 * 原理: 将字符串 "LED 100" 切割成 "LED" 和 "100"
 */
static void Process_Command(char *cmd_str)
{
    ControlCommand_t ctrl_cmd;
    char *name = NULL;  // 命令名 (如 "LED")
    char *val_str = NULL; // 数值字符串 (如 "100")
    int val = 0;

    printf("CMD Recv: %s\r\n", cmd_str); // 回显收到的命令

    /* 第一步: 获取命令名字 (以空格为分隔符) */
    name = strtok(cmd_str, " ");
    if (name == NULL) return; //如果是空命令，直接退出

    /* 第二步: 获取数值参数 */
    val_str = strtok(NULL, " "); 
    if (val_str != NULL) {
        val = atoi(val_str); // 把字符串 "100" 转成数字 100
    }

    /* 第三步: 匹配命令并发送到控制队列 */
    if (strcmp(name, "LED") == 0) 
    {
        ctrl_cmd.cmd_type = CMD_LED_CONTROL;
        ctrl_cmd.param1 = val;
        xQueueSend(TaskManager_GetControlQueue(), &ctrl_cmd, 0);
        printf("-> Set LED to %d\r\n", val);
    }
    else if (strcmp(name, "MOTOR") == 0) 
    {
        ctrl_cmd.cmd_type = CMD_MOTOR_CONTROL;
        ctrl_cmd.param1 = val;
        xQueueSend(TaskManager_GetControlQueue(), &ctrl_cmd, 0);
        printf("-> Set Fan to %d\r\n", val);
    }
    else if (strcmp(name, "MODE") == 0) 
    {
        ctrl_cmd.cmd_type = CMD_SYSTEM_MODE;
        ctrl_cmd.param1 = val;
        xQueueSend(TaskManager_GetControlQueue(), &ctrl_cmd, 0);
        printf("-> Set Mode to %d\r\n", val);
    }
    else if (strcmp(name, "HELP") == 0) 
    {
        Show_Help();
    }
    else 
    {
        printf("Error: Unknown Command!\r\n");
    }
}

/* 打印帮助信息 */
static void Show_Help(void)
{
    printf("\r\n=== Command List ===\r\n");
    printf("1. LED <0-100>    (e.g., LED 50)\r\n");
    printf("2. MOTOR <0-100>  (e.g., MOTOR 80)\r\n");
    printf("3. MODE <0-3>     (0:Auto, 1:Manual)\r\n");
    printf("====================\r\n");
}
