#include "stm32f10x.h"
#include "MQ135.h"
#include "AD.h"
#include "Delay.h" 
#include <math.h>

// MQ135初始化
void MQ135_Init(void)
{
    AD_Init(); // 复用AD初始化函数，配置PA7为ADC输入
}

// 获取ADC均值采样值
uint16_t MQ135_GetData(void)
{
    uint32_t tempData = 0;
    // 多次采样求均值，滤波
    for (uint8_t i = 0; i < MQ135_READ_TIMES; i++)
    {
        tempData += AD_GetValue();
        Delay_ms(5);
    }
    tempData /= MQ135_READ_TIMES;
    return (uint16_t)tempData;
}

// 获取PPM浓度
float MQ135_GetData_PPM(void)
{
    float tempData = 0;
    
    // 均值采样
    for (uint8_t i = 0; i < MQ135_READ_TIMES; i++)
    {
        tempData += AD_GetValue();
        Delay_ms(5);
    }
    tempData /= MQ135_READ_TIMES;
    
    // 电压转换：
    float Vol = (tempData * 5.0f) / 4096.0f;
    
    // 避免除0错误
    if (Vol < 0.01f) Vol = 0.01f;
    
    // 计算传感器电阻RS
    float RS = (5.0f - Vol) / (Vol * 0.5f);
    
    // 样板中的R0校准值
    float R0 = 6.64f;
    
    // 浓度计算公式
    float ppm = pow(11.5428f * R0 / RS, 0.6549f);
    
    return ppm;
}

// 获取PPM浓度
uint16_t MQ135_GetPPM(void)
{
    float ppm = MQ135_GetData_PPM(); // 复用样板的浮点计算逻辑
    
    // 结果限幅
    if (ppm > 1000)
    {
        ppm = 1000;
    }
    
    return (uint16_t)ppm;
}

// 获取空气质量等级
uint8_t MQ135_GetAirQualityLevel(void)
{
    uint16_t ppm = MQ135_GetPPM();
    
    // 简化的分级判断
    if (ppm < 100) return 1;       // 优
    if (ppm < 200) return 2;       // 良
    if (ppm < 500) return 3;       // 轻度污染
    if (ppm < 1000) return 4;      // 中度污染
    return 5;                      // 重度污染
}
