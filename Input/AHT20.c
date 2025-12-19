#include "stm32f10x.h"                  // Device header
#include <stdio.h> // 需要用到 printf
#include "MyI2C1.h"
#include "AHT20_Reg.h"
#include "Delay.h"

//写入初始化命令,触发测量命令,读取数据命令

void AHT20_Init(void)
{
	/* 1. 发送初始化命令: 0xBE, 0x08, 0x00 */
	//写地址,初始化命令,参数1,参数2
	
	Delay_ms(40);						//上电延迟
	
	MyI2C1_Start();						// 起始条件
	MyI2C1_SendByte(AHT20_ADDRESS);		// 发送写地址
	MyI2C1_ReceiveAck();				// 接收应答
	
	////初始化命令
	MyI2C1_SendByte(AHT20_INIT_CMD);	// 发送初始化命令(0xBE)
	MyI2C1_ReceiveAck();				// 接收应答
	MyI2C1_SendByte(AHT20_INIT_PARAM1);	// 发送参数1(0x08)
	MyI2C1_ReceiveAck();				// 接收应答
	MyI2C1_SendByte(AHT20_INIT_PARAM2);	// 发送参数2(0x00)
	MyI2C1_ReceiveAck();				// 接收应答
	MyI2C1_Stop();						// 终止时序
	
	Delay_ms(10);						// 等待初始化完成
}

//读取温湿度数据
uint8_t AHT20_ReadData(float *Temperature, float *Humidity)
{
	uint8_t Data[6];
	uint8_t i;
	uint32_t TempRaw, HumiRaw;
	
	/* 1. 发送触发测量命令: 0xAC, 0x33, 0x00 */
	//发送写地址,测量命令,参数1,参数2
	MyI2C1_Start();						// 起始条件
	MyI2C1_SendByte(AHT20_ADDRESS);		// 发送写地址
	MyI2C1_ReceiveAck();				// 接收应答
	
	//触发测量命令
	MyI2C1_SendByte(AHT20_MEASURE_CMD);	// 发送触发测量命令(0xAC)
	MyI2C1_ReceiveAck();				// 接收应答
	MyI2C1_SendByte(AHT20_MEASURE_PARAM1); // 发送参数1(0x33)
	MyI2C1_ReceiveAck();				// 接收应答
	MyI2C1_SendByte(AHT20_MEASURE_PARAM2); // 发送参数2(0x00)
	MyI2C1_ReceiveAck();				// 接收应答
	MyI2C1_Stop();						// 终止时序
	
	Delay_ms(80);						// 等待测量(>75ms)
	
	/* 2. 读取6个字节数据 */
	//读命令
	MyI2C1_Start();						// 起始条件
	MyI2C1_SendByte(AHT20_READ_ADDRESS); //发送读地址
	MyI2C1_ReceiveAck();				// 接收应答
	
	for (i = 0; i < 6; i++)				// 循环读取6个字节
	{
		Data[i] = MyI2C1_ReceiveByte();	// 接收数据
		if (i < 5)
		{
			MyI2C1_SendAck(0);			// 发送应答(继续读)
		}
		else
		{
			MyI2C1_SendAck(1);			// 发送非应答(停止读)
		}
	}
	MyI2C1_Stop();						// 终止时序
	
	/* 3. 计算结果 */
	//映射到 -50~150℃ 的温度范围和 0~100% 的湿度范围
	
	if ((Data[0] & 0x80) == 0) 			// 检查状态位(忙标志),第7位
	{
		// 计算湿度
		HumiRaw = ((uint32_t)Data[1] << 12) | ((uint32_t)Data[2] << 4) | (Data[3] >> 4);
		*Humidity = (float)HumiRaw * 100.0 / 1048576.0;
		
		// 计算温度
		TempRaw = ((uint32_t)(Data[3] & 0x0F) << 16) | ((uint32_t)Data[4] << 8) | Data[5];
		*Temperature = (float)TempRaw * 200.0 / 1048576.0 - 50.0;
		
		return 1; // 成功
	}
	
	return 0; // 失败
}

// 复制到 AHT20.c 文件最下方
void AHT20_Check(void)
{
    uint8_t ack;
    
    printf("--- Checking AHT20 Hardware ---\r\n");
    
    MyI2C1_Start();
    MyI2C1_SendByte(AHT20_ADDRESS); // 发送写地址 (0x70)
    ack = MyI2C1_ReceiveAck();      // 等待传感器应答
    MyI2C1_Stop();
    
    if (ack == 0)
    {
        printf("Result: AHT20 FOUND! (Hardware OK)\r\n");
        printf("Current Status: I2C wiring is correct.\r\n");
    }
    else
    {
        printf("Result: AHT20 NOT FOUND! (Hardware Error)\r\n");
        printf("Check: 1. PB12(SCL)/PB13(SDA) lines.\r\n");
        printf("       2. Power (3.3V/GND).\r\n");
        printf("       3. Try swapping SCL and SDA.\r\n");
    }
    printf("-------------------------------\r\n");
}
