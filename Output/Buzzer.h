#ifndef __BUZZER_H
#define __BUZZER_H

#include "stm32f10x.h"

void Buzzer_Init(void);
void Buzzer_On(void);
void Buzzer_Off(void);
void Buzzer_AutoControl(float air_quality);

//获取当前模式(1:自动,0:手动)
uint8_t Buzzer_GetAutoMode(void);

//获取当前状态 (1:响, 0:灭)
uint8_t Buzzer_GetState(void);

#endif
