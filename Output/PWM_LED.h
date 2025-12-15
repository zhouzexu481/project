#ifndef __PWM_LED_H
#define __PWM_LED_H

#include "stm32f10x.h"

void LED_PWM_Init(void);
void LED_SetBrightness(uint8_t brightness);
void LED_AutoControlByLight(float light_lux);
uint8_t LED_GetBrightness(void);

#endif
