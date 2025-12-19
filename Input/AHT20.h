#ifndef _AHT20_H
#define _AHT20_H

#include "stm32f10x.h"

void AHT20_Init(void);
uint8_t AHT20_ReadData(float *Temperature, float *Humidity);
void AHT20_Check(void);
#endif
