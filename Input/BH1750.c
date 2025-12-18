#include "BH1750.h"
#include "BH1750_Reg.h"
#include "MyI2C2.h"
#include "Delay.h"
#include <math.h>

// 写初始化函数(上电, 设置测量模式), 读取光照数据
void BH1750_Init(void)
{
    MyI2C2_Start();
    MyI2C2_SendByte(BH1750_ADDRESS_WRITE);      // 写入地址
    MyI2C2_ReceiveAck();
    MyI2C2_SendByte(BH1750_POWER_ON);           // 写入指令：上电
    MyI2C2_ReceiveAck();
    MyI2C2_Stop();
    
    Delay_ms(10);
    
    // 1. 在初始化时设置测量模式
    MyI2C2_Start();
    MyI2C2_SendByte(BH1750_ADDRESS_WRITE);
    MyI2C2_ReceiveAck();
    MyI2C2_SendByte(BH1750_CONT_H_MODE); // 设置为连续高分辨率模式
    MyI2C2_ReceiveAck();
    MyI2C2_Stop();
    
    // 延时等待第一次测量时间，防止上电直接读取数据读到0
    Delay_ms(180); 
}

float BH1750_ReadLightLevel(void)
{
    uint8_t data[2];
    uint16_t raw_data;
    float lux;
    
    // 直接读取传感器内的光照值
    // BH1750返回的是16位数据
    MyI2C2_Start();
    MyI2C2_SendByte(BH1750_ADDRESS_READ);       // 读取地址
    MyI2C2_ReceiveAck();
    
    data[0] = MyI2C2_ReceiveByte();
    MyI2C2_SendAck(0); // 0: 发送ACK, 继续读取
    data[1] = MyI2C2_ReceiveByte();
    MyI2C2_SendAck(1); // 1: 发送NACK, 停止读取
    MyI2C2_Stop();
    
    raw_data = (data[0] << 8) | data[1];
    lux = raw_data / 1.2;   // 1.2 是 BH1750 的测量分辨率系数
    
    return lux;
}
