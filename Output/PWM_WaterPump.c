#include "stm32f10x.h"
#include "PWM_WaterPump.h"

// 记录当前水泵的占空比 (0-100)
static uint8_t current_pump_duty = 0;

// 水泵PWM初始化 (使用 PA2 -> TIM2_CH3)
void PWM_WaterPump_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    
    // 1. 开启时钟：GPIOA 和 TIM2
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    // 2. 配置PA2为复用推挽输出（用于输出PWM）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 3. 配置TIM2时基单元
    // 频率计算：72MHz / (Prescaler+1) / (Period+1)
    // 72,000,000 / 720 / 100 = 1000 Hz (1kHz的PWM波)
    TIM_TimeBaseStructure.TIM_Period = 100 - 1;		// 周期为100，方便直接对应0-100的占空比
    TIM_TimeBaseStructure.TIM_Prescaler = 720 - 1;  // 预分频器
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    
    // 4. 配置TIM2 第3通道(CH3) 为PWM1模式
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // 高电平有效
    TIM_OCInitStructure.TIM_Pulse = 0;                        // 初始占空比设为0
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);
    
    // 5. 使能预装载寄存器并启动定时器
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
    
    // 初始化时确保水泵是关闭状态
    PWM_WaterPump_Off();
}

// 设置水泵的功率/占空比 (0-100)
void PWM_WaterPump_SetDutyCycle(uint8_t duty_cycle)
{
    if(duty_cycle > 100) 
	{
		duty_cycle = 100; // 限制最大值为100
	}
    // 设置 TIM2_CH3 的比较值（即占空比）
    TIM_SetCompare3(TIM2, duty_cycle);
    current_pump_duty = duty_cycle;
}

// 关闭水泵
void PWM_WaterPump_Off(void) 
{
	PWM_WaterPump_SetDutyCycle(0);
}

// 获取当前水泵占空比，用于屏幕或串口显示
uint8_t PWM_WaterPump_GetCurrentDuty(void)
{
	return current_pump_duty; 
}

// 自动控制逻辑：根据烟雾浓度开启水泵
void PWM_WaterPump_AutoControl(float smoke_level)
{
    // 假设烟雾浓度 > 800.0 PPM 时认为是火灾危险，开启水泵灭火
    if(smoke_level > 800.0f)
	{
		PWM_WaterPump_SetDutyCycle(100); // 100%全速开启水泵
	}
    else 
	{
		PWM_WaterPump_Off(); // 安全时关闭水泵
	}
}
