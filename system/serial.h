#ifndef __SERIAL_H
#define __SERIAL_H
#include <stdio.h>
extern char  Serial_RxPacket[];
extern uint8_t Serial_RxFlag;

void Serial_Init(void);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array,uint16_t Length);
void Serial_SendString(uint8_t *String);
void Serial_SendNumber(uint32_t Number,uint8_t Length);
void Serial_Printf(char *format, ...);

void Serial3_Init(uint32_t baud);
void Serial3_SendByte(uint8_t Byte);
void Serial3_SendString(char *String);
void Serial3_Printf(char *format, ...);

#endif
