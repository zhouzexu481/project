// filePath: main.c
#include "stm32f10x.h"
#include "config.h"
#include "Delay.h"
#include "task_manager.h"
#include "Serial.h"
/* 包含所有硬件初始化头文件 */
#include "Key.h"
#include "AD.h" 
#include "Buzzer.h"
#include "PWM_Motor.h"
#include "PWM_Humidifier.h"
#include "PWM_LED.h"
#include "MyI2C1.h"
#include "MyI2C2.h"
#include "AHT20.h"
#include "BH1750.h"
#include "MQ135.h"

static void Hardware_Init(void)
{
    Key_Init();
    Buzzer_Init();
    Motor_Init();
    PWM_Humidifier_Init();
    LED_PWM_Init();
    
	AD_Init();
    MyI2C1_Init();
    MyI2C2_Init();
    Serial_Init();
	Serial2_Init();
    AHT20_Init();
	BH1750_Init();
	AHT20_Check();
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    SysTick_Init(72);       /* 滴答定时器初始化，带中断 */

    Hardware_Init();
    
    printf("\r\nSystem Starting...\r\n");
    
    /* 1. 创建队列 */
    if(TaskManager_Init() != pdPASS) {
        printf("Task Manager Init Failed!\r\n");
        while(1);
    }
    
    /* 2. 创建所有任务 */
    if(TaskManager_CreateTasks() != pdPASS) {
        printf("Task Creation Failed!\r\n");
        while(1);
    }
    
    /* 3. 启动调度器 */
    vTaskStartScheduler();
    
    /* 正常情况不会执行到这里 */
    while(1);
}

/* FreeRTOS Hooks */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    printf("Stack Overflow: %s\r\n", pcTaskName);
    while(1);
}
void vApplicationMallocFailedHook(void) {
    printf("Malloc Failed!\r\n");
    while(1);
}
void vApplicationIdleHook(void) {}
