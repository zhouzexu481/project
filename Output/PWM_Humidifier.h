#ifndef _PWM_HUMIDIFIER_H
#define _PWM_HUMIDIFIER_H

#include "stm32f10x.h"

void PWM_Humidifier_Init(void);
void PWM_Humidifier_SetDutyCycle(uint8_t duty_cycle);
void PWM_Humidifier_AutoControl(float humidity);
void PWM_Humidifier_Off(void);
uint8_t PWM_Humidifier_GetCurrentDuty(void);

#endif
