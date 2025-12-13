#ifndef __POWER_MODE_H
#define __POWER_MODE_H

#include "stm32f10x.h"

/* 电源模式枚举 */
typedef enum {
    POWER_MODE_NORMAL,
    POWER_MODE_SLEEP,
    POWER_MODE_STOP
} PowerMode_TypeDef;

/* 唤醒源宏定义 */
#define WAKEUP_PA0    0x01
#define WAKEUP_USART  0x02

/* 函数声明 */
void PowerMode_Init(void);
void PowerMode_HandleWakeup(void);
void PowerMode_SetMode(PowerMode_TypeDef mode);
PowerMode_TypeDef PowerMode_GetCurrentMode(void);

#endif
