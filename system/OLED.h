#ifndef __OLED_H
#define __OLED_H

#include <stdint.h>

void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);

//Ë¢ÐÂÏÔ´æº¯Êý
void OLED_Refresh_Gram(void);

#endif
