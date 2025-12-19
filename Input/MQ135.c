#include "stm32f10x.h"
#include "MQ135.h"
#include "AD.h"
#include "Delay.h" 
#include <math.h>

#define MQ135_READ_TIMES 10  // MQ-135传感器ADC循环读取次数

// 获取PPM浓度
// 移植自提供的可用代码逻辑
uint16_t MQ135_GetPPM(void)
{
    uint32_t tempData = 0;
    float Vol, RS, ppm;
    float R0 = 6.64f; 

    // 均值滤波
    for (uint8_t i = 0; i < MQ135_READ_TIMES; i++)
    {
        tempData += AD_GetValue();
        Delay_ms(5);
    }
    tempData /= MQ135_READ_TIMES;
    
    // 转换为电压 (复用参考代码中的5V计算逻辑)
    Vol = (float)tempData * 5.0f / 4096.0f;
    
    // 避免除0
    if(Vol == 0) Vol = 0.01f; 
    
    // 算出传感器电阻 Rs
    RS = (5.0f - Vol) / (Vol * 0.5f);
    
    // 算出 PPM
    ppm = pow(11.5428f * R0 / RS, 0.6549f);
    
    // 结果限幅
    if(ppm > 1000) 
    {
        ppm = 1000;
    }
    
    return (uint16_t)ppm;
}

// 获取空气质量等级 (1-5级)
uint8_t MQ135_GetAirQualityLevel(void)
{
    uint16_t ppm = MQ135_GetPPM();
    
    // 简化的分级判断
    if(ppm < 100) return 1;       // 优
    if(ppm < 200) return 2;       // 良
    if(ppm < 500) return 3;       // 轻度污染
    if(ppm < 1000) return 4;      // 中度污染
    return 5;                     // 重度污染
}
