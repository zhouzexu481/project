#ifndef _MYI2C1_H
#define _MYI2C1_H

#include "stm32f10x.h"

void MyI2C1_Init(void);
void MyI2C1_Start(void);
void MyI2C1_Stop(void);
void MyI2C1_SendByte(uint8_t Byte);
uint8_t MyI2C1_ReceiveByte(void);
void MyI2C1_SendAck(uint8_t AckBit);
uint8_t MyI2C1_ReceiveAck(void);

#endif
