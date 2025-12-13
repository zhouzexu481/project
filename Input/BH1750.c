#include "BH1750.h"
#include "MyI2C2.h"
#include "task_manager.h" // <--- 再次确认这行存在
#include "Delay.h"
#include <math.h>

#define BH1750_ADDRESS_WRITE    0x46
#define BH1750_ADDRESS_READ     0x47
#define BH1750_POWER_ON         0x01
#define BH1750_CONT_HR_MODE     0x10

void BH1750_Init(void)
{
    
    MyI2C2_Start();
    MyI2C2_SendByte(BH1750_ADDRESS_WRITE);
    MyI2C2_ReceiveAck();
    MyI2C2_SendByte(BH1750_POWER_ON);
    MyI2C2_ReceiveAck();
    MyI2C2_Stop();
    
    Delay_ms(10);
    
    MyI2C2_Start();
    MyI2C2_SendByte(BH1750_ADDRESS_WRITE);
    MyI2C2_ReceiveAck();
    MyI2C2_SendByte(BH1750_CONT_HR_MODE);
    MyI2C2_ReceiveAck();
    MyI2C2_Stop();
    
    Delay_ms(180);
}

float BH1750_ReadLightLevel(void)
{
    uint8_t data[2];
    uint16_t raw_data;
    float lux;
    
    MyI2C2_Start();
    MyI2C2_SendByte(BH1750_ADDRESS_WRITE);
    MyI2C2_ReceiveAck();
    MyI2C2_SendByte(BH1750_CONT_HR_MODE);
    MyI2C2_ReceiveAck();
    MyI2C2_Stop();
    
    Delay_ms(180);
    
    MyI2C2_Start();
    MyI2C2_SendByte(BH1750_ADDRESS_READ);
    MyI2C2_ReceiveAck();
    
    data[0] = MyI2C2_ReceiveByte();
    MyI2C2_SendAck(0);
    data[1] = MyI2C2_ReceiveByte();
    MyI2C2_SendAck(1);
    MyI2C2_Stop();
     
    raw_data = (data[0] << 8) | data[1];
    lux = raw_data / 1.2;
    
    return lux;
}
