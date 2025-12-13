#ifndef _WATCHDOG_H
#define _WATCHDOG_H

#include "stm32f10x.h"

// 看门狗配置
#define IWDG_TIMEOUT_MS    1000    // 独立看门狗超时时间1秒
#define WWDG_TIMEOUT_MS    50      // 窗口看门狗超时时间50ms

// 函数声明
void Watchdog_Init(void);
void IWDG_Init(void);
void WWDG_Init(void);
void IWDG_Feed(void);
void WWDG_Feed(void);
void Watchdog_FeedAll(void);
uint8_t Watchdog_CheckResetSource(void);

#endif
