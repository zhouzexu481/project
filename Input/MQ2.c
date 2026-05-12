#include "stm32f10x.h"
#include "MQ2.h"
#include "AD.h"
#include "Delay.h" 
#include <math.h>

// MQ2初始化
void MQ2_Init(void)
{
    AD_Init(); // 复用AD初始化函数，配置PA7为ADC输入
}

// 获取ADC均值采样值
uint16_t MQ2_GetData(void)
{
    uint32_t tempData = 0;
    // 多次采样求均值，滤波
    for (uint8_t i = 0; i < MQ2_READ_TIMES; i++)
    {
        tempData += AD_GetValue();
        Delay_ms(5);
    }
    tempData /= MQ2_READ_TIMES;
    return (uint16_t)tempData;
}

// 获取PPM浓度
float MQ2_GetData_PPM(void)
{
    float tempData = 0;
    
    // 均值采样
    for (uint8_t i = 0; i < MQ2_READ_TIMES; i++)
    {
        tempData += AD_GetValue();
        Delay_ms(5);
    }
    tempData /= MQ2_READ_TIMES;
    
    // 电压转换：
    float Vol = (tempData * 3.3f) / 4096.0f;
    
    // 避免除0错误
    if (Vol < 0.01f) Vol = 0.01f;
    
    // 计算传感器电阻RS
    float RS = (5.0f - Vol) / (Vol * 1.0f); 
    
    // 样板中的R0校准值
    float R0 = 25.0f;
    
    // 浓度计算公式 (已替换为MQ2公式)
    float ppm = pow(11.5428f * R0 / RS, 0.6549f);
    
    return ppm;
}

// 获取PPM浓度
uint16_t MQ2_GetPPM(void)
{
    float ppm = MQ2_GetData_PPM(); // 复用样板的浮点计算逻辑
    
    // 结果限幅 (MQ2量程较大，上限改为10000)
    if (ppm > 10000)
    {
        ppm = 10000;
    }
    
    return (uint16_t)ppm;
}

// 获取烟雾等级
uint8_t MQ2_GetSmokeLevel(void)
{
    uint16_t ppm = MQ2_GetPPM();
    
    // 简化的分级判断
    if (ppm < 300) return 1;       // 安全
    if (ppm < 800) return 2;       // 轻微
    if (ppm < 1500) return 3;      // 轻度危险
    if (ppm < 2000) return 4;      // 中度危险
    return 5;                      // 重度危险
}
