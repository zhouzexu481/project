#ifndef _MQ135_H
#define _MQ135_H

#include "stm32f10x.h"

void MQ135_Init(void);
uint16_t MQ135_ReadRaw(void);
float MQ135_ReadVoltage(void);
uint16_t MQ135_GetPPM(void);
uint8_t MQ135_GetAirQualityLevel(void);

#endif
