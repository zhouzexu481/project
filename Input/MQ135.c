#include "stm32f10x.h"
#include "MQ135.h"
#include "AD.h"
#include <math.h>

/* 校准参数 */
#define MQ135_RL            10.0f        // 负载电阻 kOhm
#define MQ135_RO            9.83f        // 洁净空气中的基准电阻
#define MQ135_PARA_A        116.602f     // 曲线系数 A
#define MQ135_PARA_B        -2.769f      // 曲线指数 B

//获取PPM浓度
uint16_t MQ135_GetPPM(void)
{
    uint16_t ad_value;
    float voltage, rs, ratio;
    float ppm;
    
    /* 1. 直接读取ADC原始值 */
    ad_value = AD_GetValue();
    
    /* 2. 转换为电压 (0~4095 -> 0~3.3V) */
    voltage = (float)ad_value * 3.3f / 4096.0f;
    
    /* 3. 电压限幅 (防止分母为0或计算异常) */
    if(voltage < 0.1f) voltage = 0.1f;
    if(voltage > 3.2f) voltage = 3.2f;
    
    /* 4. 物理公式计算 */
    rs = (3.3f - voltage) / voltage * MQ135_RL; // 算出传感器电阻 Rs
    ratio = rs / MQ135_RO;                      // 算出电阻比 Rs/Ro
    ppm = MQ135_PARA_A * pow(ratio, MQ135_PARA_B); // 算出 PPM
    
    /* 5. 结果限幅 */
    if(ppm > 5000) ppm = 5000;
    
    return (uint16_t)ppm;
}

//获取空气质量等级 (1-5级)
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
