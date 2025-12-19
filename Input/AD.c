#include "stm32f10x.h"
#include "AD.h"

// ADC初始化（PA7 - ADC1_Channel7）
void AD_Init(void)
{
    // 使能ADC1和GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // ADC时钟分频：PCLK2(72MHz)/6 = 12MHz（符合F1 ADC最大14MHz要求）
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
    
    // 配置PA7为模拟输入模式
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; // 模拟输入（无需配置Speed）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;     // PA7引脚
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 规则组通道配置: ADC1, 通道7, 序列1, 采样时间55.5个周期
    ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 1, ADC_SampleTime_55Cycles5);
    
    // ADC初始化参数配置
    ADC_InitTypeDef ADC_InitStruct;
    ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;          // 独立模式
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;      // 数据右对齐
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 软件触发
    ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;         // 单次转换模式
    ADC_InitStruct.ADC_ScanConvMode = DISABLE;               // 单通道模式
    ADC_InitStruct.ADC_NbrOfChannel = 1;                     // 转换通道数1
    ADC_Init(ADC1, &ADC_InitStruct);
    
    // 使能ADC1
    ADC_Cmd(ADC1, ENABLE);
    
    // ADC校准（必须步骤）
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1) == SET); // 等待校准复位完成
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1) == SET);      // 等待校准完成
}

// 获取ADC采样值（单次）
uint16_t AD_GetValue(void)
{
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);                // 软件触发转换
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); // 等待转换完成
    return ADC_GetConversionValue(ADC1);                   // 返回12位采样值（0-4095）
}
