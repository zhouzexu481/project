#include "stm32f10x.h"
#include "Buzzer.h"
#include "config.h" 

static uint8_t auto_mode = 1;
static uint8_t current_state = 0; // 新增状态变量

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
    // 如果是高电平触发请用 SetBits，低电平触发请用 ResetBits
    // 假设是高电平响 (根据您的 PA0 规划)
    GPIO_SetBits(BUZZER_PORT, BUZZER_PIN); 
    current_state = 1;
}

void Buzzer_Off(void)
{
    GPIO_ResetBits(BUZZER_PORT, BUZZER_PIN);
    current_state = 0;
}

void Buzzer_AutoControl(float air_quality)
{
    if (auto_mode) {
        if (air_quality >= AIR_QUALITY_THRESHOLD) {
            Buzzer_On();
        } else {
            Buzzer_Off();
        }
    }
}

uint8_t Buzzer_GetAutoMode(void) { return auto_mode; }

// 新增函数的实现
uint8_t Buzzer_GetState(void) { return current_state; }
