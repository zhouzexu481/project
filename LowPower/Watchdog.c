#include "stm32f10x.h"
#include "Watchdog.h"
#include "OLED.h"
#include "Delay.h"

void Watchdog_Init(void)
{
    // 检查复位源并显示
    Watchdog_CheckResetSource();
    
    // 初始化独立看门狗
    IWDG_Init();
    
    // 窗口看门狗可选初始化（根据需求开启）
    // WWDG_Init();
}

void IWDG_Init(void)
{
    // 写使能，解除写保护
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    
    // 配置预分频器，40kHz / 16 = 2.5kHz
    IWDG_SetPrescaler(IWDG_Prescaler_16);
    
    // 配置重装值，2500 / 2.5kHz = 1秒
    IWDG_SetReload(2499);
    
    // 喂狗
    IWDG_ReloadCounter();
    
    // 启动看门狗
    IWDG_Enable();
}

void WWDG_Init(void)
{
    // 开启WWDG时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
    
    // 设置预分频器
    WWDG_SetPrescaler(WWDG_Prescaler_8);
    
    // 设置窗口值
    WWDG_SetWindowValue(0x40 | 21);    // 窗口时间约30ms
    
    // 使能WWDG并设置计数器值
    WWDG_Enable(0x40 | 54);            // 超时时间约50ms
}

void IWDG_Feed(void)
{
    IWDG_ReloadCounter();
}

void WWDG_Feed(void)
{
    WWDG_SetCounter(0x40 | 54);
}

void Watchdog_FeedAll(void)
{
    IWDG_Feed();
    // WWDG_Feed();  // 如果启用WWDG则取消注释
}

uint8_t Watchdog_CheckResetSource(void)
{
    uint8_t reset_source = 0;
    
    // 检查独立看门狗复位
    if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) == SET) {
        OLED_ShowString(1, 1, "IWDG Reset     ");
        reset_source = 1;
        Delay_ms(1000);
        RCC_ClearFlag();
    }
    // 检查窗口看门狗复位
    else if(RCC_GetFlagStatus(RCC_FLAG_WWDGRST) == SET) {
        OLED_ShowString(1, 1, "WWDG Reset     ");
        reset_source = 2;
        Delay_ms(1000);
        RCC_ClearFlag();
    }
    // 检查电源复位
    else if(RCC_GetFlagStatus(RCC_FLAG_PORRST) == SET) {
        OLED_ShowString(1, 1, "Power Reset    ");
        reset_source = 3;
        Delay_ms(1000);
        RCC_ClearFlag();
    }
    // 检查外部复位
    else if(RCC_GetFlagStatus(RCC_FLAG_PINRST) == SET) {
        OLED_ShowString(1, 1, "External Reset ");
        reset_source = 4;
        Delay_ms(1000);
        RCC_ClearFlag();
    }
    else {
        OLED_ShowString(1, 1, "Normal Start   ");
    }
    
    return reset_source;
}
