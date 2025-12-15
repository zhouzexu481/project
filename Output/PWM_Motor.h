// filePath: PWM_Motor.h
#ifndef __PWM_MOTOR_H
#define __PWM_MOTOR_H

#include "stm32f10x.h"

void Motor_Init(void);
void Motor_SetSpeed(uint8_t Speed);
void Motor_Stop(void);
uint8_t Get_Motor_Speed(void);
void Motor_TemperatureControl(float temperature);

#endif
