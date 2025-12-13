#ifndef __POWER_MANAGEMENT_H
#define __POWER_MANAGEMENT_H

#include "stm32f10x.h"
#include "Power_Mode.h"

/* 电源状态结构体 */
typedef struct {
    float battery_voltage;
    uint8_t battery_level;
    float power_consumption;
    uint8_t is_charging;
    PowerMode_TypeDef current_mode;
} PowerStatus_t;

/* 任务入口函数 (已修正名称，去除中间的下划线) */
void PowerManagement_Task(void *pvParameters);

#endif
