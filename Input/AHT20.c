// AHT20.c
#include "stm32f10x.h"
#include "MyI2C1.h"
#include "AHT20.h"
#include "AHT20_Reg.h"
#include "Delay.h"

void AHT20_Init(void)
{
	MyI2C1_Start();
	MyI2C1_SendByte(AHT20_ADDRESS);
	MyI2C1_ReceiveAck();
	MyI2C1_SendByte(AHT20_SOFT_RESET_CMD);
	MyI2C1_ReceiveAck();
	MyI2C1_Stop();
	Delay_ms(20);
	
	MyI2C1_Start();
	MyI2C1_SendByte(AHT20_ADDRESS);
	MyI2C1_ReceiveAck();
	MyI2C1_SendByte(AHT20_INIT_CMD);
	MyI2C1_ReceiveAck();
	MyI2C1_SendByte(AHT20_INIT_PARAM1);
	MyI2C1_ReceiveAck();
	MyI2C1_SendByte(AHT20_INIT_PARAM2);
	MyI2C1_ReceiveAck();
	MyI2C1_Stop();
	Delay_ms(10);
}

uint8_t AHT20_ReadData(float *Temperature, float *Humidity)
{
	uint8_t i, Data[6];
	uint32_t TempRaw, HumiRaw;
	
	MyI2C1_Start();
	MyI2C1_SendByte(AHT20_ADDRESS);
	MyI2C1_ReceiveAck();
	MyI2C1_SendByte(AHT20_MEASURE_CMD);
	MyI2C1_ReceiveAck();
	MyI2C1_SendByte(AHT20_MEASURE_PARAM1);
	MyI2C1_ReceiveAck();
	MyI2C1_SendByte(AHT20_MEASURE_PARAM2);
	MyI2C1_ReceiveAck();
	MyI2C1_Stop();
	
	Delay_ms(80);
	
	MyI2C1_Start();
	MyI2C1_SendByte(AHT20_READ_ADDRESS);	//∑¢ÀÕ∂¡µÿ÷∑
	MyI2C1_ReceiveAck();
	
	for(i = 0; i < AHT20_DATA_LENGTH; i++) {
		Data[i] = MyI2C1_ReceiveByte();
		if(i < (AHT20_DATA_LENGTH - 1))
			MyI2C1_SendAck(0);
		else
			MyI2C1_SendAck(1);
	}
	MyI2C1_Stop();
	
	if(Data[0] & AHT20_STATUS_BUSY)
		return 0;
	
	HumiRaw = ((uint32_t)Data[1] << 12) | ((uint32_t)Data[2] << 4) | (Data[3] >> 4);
	*Humidity = (float)HumiRaw * 100.0 / 1048576.0;
	
	TempRaw = ((uint32_t)(Data[3] & 0x0F) << 16) | ((uint32_t)Data[4] << 8) | Data[5];
	*Temperature = (float)TempRaw * 200.0 / 1048576.0 - 50.0;
	
	return 1;
}
