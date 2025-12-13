#include "stm32f10x.h"
#include "PWM_Humidifier.h"

static uint8_t current_duty_cycle = 0;

void PWM_Humidifier_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    // PA2 (TIM2_CH3)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // TIM2 Base (与其他PWM共享)
    TIM_TimeBaseStructure.TIM_Period = 100 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 720 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    
    // TIM2 Channel 3
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);
    
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
    
    PWM_Humidifier_Off();
}

void PWM_Humidifier_SetDutyCycle(uint8_t duty_cycle)
{
    if(duty_cycle > 100) duty_cycle = 100;
    TIM_SetCompare3(TIM2, duty_cycle);
    current_duty_cycle = duty_cycle;
}

void PWM_Humidifier_Off(void) { PWM_Humidifier_SetDutyCycle(0); }
uint8_t PWM_Humidifier_GetCurrentDuty(void) { return current_duty_cycle; }

void PWM_Humidifier_AutoControl(float humidity)
{
    // 简单控制逻辑示例
    if(humidity < 40.0f) PWM_Humidifier_SetDutyCycle(100);
    else if(humidity < 60.0f) PWM_Humidifier_SetDutyCycle(50);
    else PWM_Humidifier_Off();
}
