#ifndef __MQ2_H
#define __MQ2_H

#include "stm32f10x.h"

#define MQ2_READ_TIMES 10  // MQ-2传感器ADC循环读取次数

// 函数声明
void MQ2_Init(void);               // MQ2初始化（复用AD_Init）
uint16_t MQ2_GetData(void);        // 获取ADC均值采样值
float MQ2_GetData_PPM(void);       // 获取PPM浓度（浮点型）
uint16_t MQ2_GetPPM(void);         // 获取PPM浓度（整型）
uint8_t MQ2_GetSmokeLevel(void);   // 获取烟雾等级

#endif 
