#include "Delay.h"
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"

static uint16_t g_fac_us = 0;

extern void xPortSysTickHandler(void);

void SysTick_Init(uint16_t sysclk)
{
    uint32_t reload;

    SysTick->CTRL &= ~(1 << 2);
    
    g_fac_us = sysclk / 8;
    
    reload = sysclk / 8;            
    reload *= 1000000 / configTICK_RATE_HZ; 
    
    SysTick->CTRL |= 1 << 1;
    SysTick->LOAD = reload;
    SysTick->CTRL |= 1 << 0;
}

void Delay_us(uint32_t xus)
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload = SysTick->LOAD;

    ticks = xus * g_fac_us;
    told = SysTick->VAL;

    while (1)
    {
        tnow = SysTick->VAL;
        
        if (tnow != told)
        {
            if (tnow < told)
            {
                tcnt += told - tnow;
            }
            else
            {
                tcnt += reload - tnow + told;
            }
            
            told = tnow;

            if (tcnt >= ticks)
            {
                break;
            }
        }
    }
}

void Delay_ms(uint32_t xms)
{
    while (xms--)
    {
        Delay_us(1000);
    }
}

void Delay_s(uint32_t xs)
{
    while (xs--)
    {
        Delay_ms(1000);
    }
}

void SysTick_Handler(void)
{
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
        xPortSysTickHandler();
    }
}
