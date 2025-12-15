#include "BH1750.h"
#include "BH1750_Reg.h"
#include "MyI2C2.h"
#include "Delay.h"
#include <math.h>

void BH1750_Init(void)
{
    MyI2C2_Start();
    MyI2C2_SendByte(BH1750_ADDRESS_WRITE);
    MyI2C2_ReceiveAck();
    MyI2C2_SendByte(BH1750_POWER_ON);
    MyI2C2_ReceiveAck();
    MyI2C2_Stop();
    
    Delay_ms(10);
    
    // 1. 在初始化时就启动连续测量
    MyI2C2_Start();
    MyI2C2_SendByte(BH1750_ADDRESS_WRITE);
    MyI2C2_ReceiveAck();
    MyI2C2_SendByte(BH1750_CONT_H_MODE); // 设置为连续高分辨率模式
    MyI2C2_ReceiveAck();
    MyI2C2_Stop();
    
    // 给传感器第一次测量的时间，防止刚上电马上读数据读到0
    Delay_ms(180); 
}

float BH1750_ReadLightLevel(void)
{
    uint8_t data[2];
    uint16_t raw_data;
    float lux;
    
    // 2. 读取函数中删除 "发送命令" 和 "Delay_ms(180)"
    // 直接读取寄存器中最新的值
    
    MyI2C2_Start();
    MyI2C2_SendByte(BH1750_ADDRESS_READ);
    MyI2C2_ReceiveAck();
    
    data[0] = MyI2C2_ReceiveByte();
    MyI2C2_SendAck(0); // 根据你的I2C库，0通常代表ACK（继续读）
    data[1] = MyI2C2_ReceiveByte();
    MyI2C2_SendAck(1); // 1通常代表NACK（停止读）
    MyI2C2_Stop();
    
    raw_data = (data[0] << 8) | data[1];
    lux = raw_data / 1.2;
    
    return lux;
}
