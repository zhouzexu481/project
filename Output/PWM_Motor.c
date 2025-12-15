#include "stm32f10x.h"
#include "PWM_Motor.h"

static uint8_t current_speed = 0;

//配置TIM2的频率为1kHz
void Motor_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    // PA1 (TIM2_CH2)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 频率 1kHz (72M / 720 / 100)
    TIM_TimeBaseStructure.TIM_Period = 100 - 1;		//ARR
    TIM_TimeBaseStructure.TIM_Prescaler = 720 - 1;	//预分频,PSC
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    
    // TIM2 Channel 2,PA1引脚
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);
    
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
    
    Motor_Stop();
}

void Motor_SetSpeed(uint8_t Speed)
{
    if (Speed > 100) 
	{
		Speed = 100;
	}
    current_speed = Speed;
    TIM_SetCompare2(TIM2, Speed);	//2通道,TIM2
}

void Motor_Stop(void)
{
	Motor_SetSpeed(0);
}

uint8_t Get_Motor_Speed(void)
{
	return current_speed;
}

void Motor_TemperatureControl(float temperature)
{
    if (temperature > 35.0f)
	{
		Motor_SetSpeed(100);
	}
    else if (temperature > 30.0f) 
	{
		Motor_SetSpeed(60);
	}
    else if (temperature > 25.0f)
	{
		Motor_SetSpeed(30);
	}
    else
	{
		Motor_Stop();
	}
}
