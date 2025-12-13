#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"

void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_Initstructure;
	GPIO_Initstructure.GPIO_Mode=GPIO_Mode_IPU;//上拉输入
	GPIO_Initstructure.GPIO_Pin=GPIO_Pin_1 |GPIO_Pin_11;
	GPIO_Initstructure.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_Initstructure);
}

uint8_t Key_GetNum(void)
{
	uint8_t	KeyNum=0;
	if( GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==0)
	{
		vTaskDelay(pdMS_TO_TICKS(20));
		while( GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==0)
		KeyNum=1;
		vTaskDelay(pdMS_TO_TICKS(20));
	}
	if( GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)==0)
	{
		vTaskDelay(pdMS_TO_TICKS(20));
		while( GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)==0)
		KeyNum=2;
		vTaskDelay(pdMS_TO_TICKS(20));
	}
	return KeyNum;
}
