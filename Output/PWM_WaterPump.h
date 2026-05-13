#ifndef _PWM_WATER_PUMP_H
#define _PWM_WATER_PUMP_H

#include "stm32f10x.h"

// 函数声明
void PWM_WaterPump_Init(void);                      // 水泵PWM初始化
void PWM_WaterPump_SetDutyCycle(uint8_t duty_cycle);// 设置水泵转速/功率 (0-100)
void PWM_WaterPump_AutoControl(float smoke_level, float humidity);  //水泵自动控制逻辑
void PWM_WaterPump_Off(void);                       // 关闭水泵
uint8_t PWM_WaterPump_GetCurrentDuty(void);         // 获取当前水泵功率状态

#endif
