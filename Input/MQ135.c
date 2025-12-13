#include "stm32f10x.h"
#include "MQ135.h"
#include "AD.h"
#include "Delay.h"
#include <math.h>

#define MQ135_RL_VALUE          10.0f
#define MQ135_RO_CLEAN_AIR      9.83f
#define MQ135_SCALE_FACTOR      116.6020682f
#define MQ135_EXPONENT          -2.769034857f

void MQ135_Init(void)
{
    AD_Init(); // ≥ı ºªØ ADC (PA7)
}

uint16_t MQ135_ReadRaw(void)
{
    return AD_GetValue();
}

float MQ135_ReadVoltage(void)
{
    uint16_t adcValue = MQ135_ReadRaw();
    return (float)adcValue * 3.3f / 4096.0f;
}

uint16_t MQ135_GetPPM(void)
{
    float voltage = MQ135_ReadVoltage();
    
    if(voltage < 0.1f) voltage = 0.1f;
    if(voltage > 3.2f) voltage = 3.2f;
    
    float rs = (3.3f - voltage) / voltage * MQ135_RL_VALUE;
    float ratio = rs / MQ135_RO_CLEAN_AIR;
    uint16_t ppm = (uint16_t)(MQ135_SCALE_FACTOR * pow(ratio, MQ135_EXPONENT));
    
    if(ppm > 5000) ppm = 5000;
    
    return ppm;
}

uint8_t MQ135_GetAirQualityLevel(void)
{
    uint16_t ppm = MQ135_GetPPM();
    if(ppm < 100) return 1;
    else if(ppm < 200) return 2;
    else if(ppm < 500) return 3;
    else if(ppm < 1000) return 4;
    else return 5;
}
