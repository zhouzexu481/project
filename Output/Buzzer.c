#include "stm32f10x.h"
#include "Buzzer.h"
#include "config.h" 

static uint8_t auto_mode = 1;
static uint8_t current_state = 0; // 状态变量

void Buzzer_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = BUZZER_PIN;
    GPIO_Init(BUZZER_PORT, &GPIO_InitStructure);
    
    Buzzer_Off();
}

void Buzzer_On(void)
{
    GPIO_ResetBits(BUZZER_PORT, BUZZER_PIN); // 修改为低电平触发
    current_state = 1;
}

void Buzzer_Off(void)
{
    GPIO_SetBits(BUZZER_PORT, BUZZER_PIN); // 修改为高电平关闭
    current_state = 0;
}

//自动模式,根据空气质量,判断是否打开蜂鸣器
void Buzzer_AutoControl(float air_quality)
{
    if (auto_mode)
	{
        if (air_quality >= 100) 
		{
            Buzzer_On();
        } 
		else 
		{
            Buzzer_Off();
        }
    }
}

//返回当前模式状态
uint8_t Buzzer_GetAutoMode(void) 
{
	return auto_mode; 
}

//返回蜂鸣器当前状态
uint8_t Buzzer_GetState(void) 
{ 
	return current_state; 
}
