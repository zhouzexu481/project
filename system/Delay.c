#include "stm32f10x.h"
#include "Delay.h"

static uint8_t  fac_us=0;

void Delay_Init(void)
{
    // SystemCoreClock 在 system_stm32f10x.c 中定义
    fac_us = SystemCoreClock / 8000000;
}

void Delay_us(uint32_t nus)
{
    uint32_t i;
    for(i=0; i<nus; i++)
    {
        /* === 修改点: 添加 volatile 防止编译器优化掉延时 === */
        volatile uint8_t j = fac_us;
        while(j--);
    }
}

void Delay_ms(uint32_t nms)
{
    uint32_t i;
    for(i=0; i<nms; i++)
    {
        Delay_us(1000);
    }
}

void Delay_s(uint32_t xs)
{
    while(xs--)
    {
        Delay_ms(1000);
    }
}
