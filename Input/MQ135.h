#ifndef __MQ135_H
#define __MQ135_H

#include "stm32f10x.h"

#define MQ135_READ_TIMES 10  // MQ-135传感器ADC循环读取次数

// 函数声明
void MQ135_Init(void);               // MQ135初始化（复用AD_Init）
uint16_t MQ135_GetData(void);        // 获取ADC均值采样值
float MQ135_GetData_PPM(void);       // 获取PPM浓度（浮点型，和样板一致）
uint16_t MQ135_GetPPM(void);         // 获取PPM浓度（整型，你的原函数）
uint8_t MQ135_GetAirQualityLevel(void); // 获取空气质量等级

#endif /* __MQ135_H */
