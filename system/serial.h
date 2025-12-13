#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdio.h>
#include "stm32f10x.h" // 必须包含，否则 uint8_t 无法识别

/* 外部变量声明 (让其他文件能看到这些变量) */
extern char Serial_RxPacket[];
extern uint8_t Serial_RxFlag;

/* 函数声明 */
void Serial_Init(void);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString(char *String); // 修复：这里改成 char* 以匹配 .c 文件
void Serial_SendNumber(uint32_t Number, uint8_t Length);
void Serial_Printf(char *format, ...);

#endif
