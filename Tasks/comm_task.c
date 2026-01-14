#include "comm_task.h"
#include "task_manager.h"
#include "serial.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "AD.h"

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
    
    // 初始化串口3用于连接ESP32C3 (波特率115200)
    Serial3_Init(115200);

    printf("System Started! Waiting for commands...\r\n");
    
    while(1) 
    {
        /* 1. 检查有没有收到串口数据 (来自 PC 或 ESP32) */
        if(Serial_RxFlag == 1) 
        {
            Serial_RxFlag = 0; // 清除标志位
            Process_Command(Serial_RxPacket); // 处理命令 (如 "LED 100")
        }
        
        /* 2. 定时发送数据 (通过串口1 和 串口3) */
        if(xQueuePeek(TaskManager_GetSensorQueue(), &data, 0) == pdPASS) 
        {
            /* 串口1: 调试打印 (保留原有功能) */
            printf(" T:%3.1fC     H:%3.1f%%     L:%3.0fLux     A:%3.2f\r\n",
                   data.temperature, 
                   data.humidity, 
                   data.light_intensity, 
                   data.air_quality);

            /* 串口3: 发送给ESP32 (OneNet JSON格式) */
            // 格式示例: {"temp":25.5,"humi":50.0,"lux":100,"air":1.20}
            Serial3_Printf("{");
            Serial3_Printf("\"temp\":%.1f,", data.temperature);
            Serial3_Printf("\"humi\":%.1f,", data.humidity);
            Serial3_Printf("\"lux\":%.0f,",  data.light_intensity);
            Serial3_Printf("\"air\":%.2f",   data.air_quality); // 最后一项不加逗号
            Serial3_Printf("}\r\n"); // 发送换行符作为结束标记
        }
        // 延时 1000ms
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
    /* 1. 加湿器指令 (例如: MIST 50) */
    if (strcmp(name, "MIST") == 0)  //比较两个字符串是否一致
    {
        ctrl_cmd.cmd_type = CMD_HUMIDIFIER_CONTROL;
        ctrl_cmd.param = val; // 0-100
        xQueueSend(TaskManager_GetControlQueue(), &ctrl_cmd, 0);
        printf("-> Set Mist to %d\r\n", val);
    }
    /* 2. 蜂鸣器指令 (例如: BUZZ 1 或 BUZZ 0) */
    else if (strcmp(name, "BUZZ") == 0) 
    {
        ctrl_cmd.cmd_type = CMD_BUZZER_CONTROL;
        ctrl_cmd.param = val; // 0 或 1
        xQueueSend(TaskManager_GetControlQueue(), &ctrl_cmd, 0);
        printf("-> Set Buzzer to %s\r\n", val ? "ON" : "OFF");
    }
    /* 3. LED灯光指令匹配 (格式: LED <0-100>) */
    else if (strcmp(name, "LED") == 0) 
    {
        ctrl_cmd.cmd_type = CMD_LED_CONTROL;
        ctrl_cmd.param = val;
        xQueueSend(TaskManager_GetControlQueue(), &ctrl_cmd, 0);
        printf("-> Set LED to %d\r\n", val);
    }
    /* 4. 风扇电机指令匹配 (格式: MOTOR <0-100>) */
    else if (strcmp(name, "MOTOR") == 0) 
    {
        ctrl_cmd.cmd_type = CMD_MOTOR_CONTROL;
        ctrl_cmd.param = val;
        xQueueSend(TaskManager_GetControlQueue(), &ctrl_cmd, 0);
        printf("-> Set Fan to %d\r\n", val);
    }
    /* 5. 系统模式指令匹配 (格式: MODE <0/1>) */
    /* 0: 自动模式, 1: 手动模式 */
    else if (strcmp(name, "MODE") == 0) 
    {
        ctrl_cmd.cmd_type = CMD_SYSTEM_MODE;
        ctrl_cmd.param = val;
        xQueueSend(TaskManager_GetControlQueue(), &ctrl_cmd, 0);
        printf("-> Set Mode to %d\r\n", val);
    }
    /* 6. 帮助指令匹配 (格式: HELP) */
    else if (strcmp(name, "HELP") == 0) 
    {
        Show_Help();
    }
    /* 7. 未知指令处理 */
    /* 如果前面所有的 if 都没匹配上，说明用户发了一个我不认识的词 */
    else 
    {
        printf("Error: Unknown Command!\r\n");
    }
}

/* 打印帮助信息 */
static void Show_Help(void)
{
    printf("\r\n=== Command List ===\r\n");
    printf("1. LED <0-100>    (e.g., LED 50)\r\n");     //LED
    printf("2. MOTOR <0-100>  (e.g., MOTOR 80)\r\n");   //风扇
    printf("3. MIST <0-100>   (e.g., MIST 60)\r\n");  // 加湿器
    printf("4. BUZZ <0/1>     (e.g., BUZZ 1)\r\n");   // 蜂鸣器
    printf("5. MODE <0/1>     (0:Auto, 1:Manual)\r\n");
    printf("====================\r\n");
        
}
