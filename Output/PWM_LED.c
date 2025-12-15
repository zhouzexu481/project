#include "stm32f10x.h"
#include "PWM_LED.h"
#include "config.h"

static uint8_t current_brightness = 0;

void LED_PWM_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    // PA3 (TIM2_CH4)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    TIM_TimeBaseStructure.TIM_Period = 100 - 1;		//TIM2里的各通道配置要相同
    TIM_TimeBaseStructure.TIM_Prescaler = 720 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OC4Init(TIM2, &TIM_OCInitStructure);
    
    TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

void LED_SetBrightness(uint8_t brightness)
{
    if(brightness > 100)
	{
		brightness = 100;
	}
    current_brightness = brightness;
    TIM_SetCompare4(TIM2, brightness);
}

void LED_AutoControlByLight(float light_lux)
{
    if (light_lux < LIGHT_THRESHOLD_DARK)
	{
		LED_SetBrightness(100);
	}
    else if (light_lux < LIGHT_THRESHOLD_DIM) 
	{
		LED_SetBrightness(75);
	}
    else if (light_lux < LIGHT_THRESHOLD_NORMAL)
	{
		LED_SetBrightness(50);
	}
    else if (light_lux < LIGHT_THRESHOLD_BRIGHT)
	{
		LED_SetBrightness(25);
	}
    else
	{
		LED_SetBrightness(0);
	}
}

uint8_t LED_GetBrightness(void)
{
	return current_brightness; 
}
