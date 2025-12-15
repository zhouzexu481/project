#include "stm32f10x.h"
#include "OLED_Font.h"
#include "Delay.h"
#include "OLED.h"

/* * 硬件接线映射:
 * GND -> PB6 (软件输出低电平供电)
 * VCC -> PB7 (软件输出高电平供电)
 * SCL -> PB8 (时钟线)
 * SDA -> PB9 (数据线)
 */

/* ================= 1. 底层通信部分 (模拟 I2C) ================= */

/* 写引脚电平，封装一下方便阅读 */
void OLED_W_SCL(uint8_t x) { GPIO_WriteBit(GPIOB, GPIO_Pin_8, (BitAction)x); }
void OLED_W_SDA(uint8_t x) { GPIO_WriteBit(GPIOB, GPIO_Pin_9, (BitAction)x); }

/* I2C 起始信号: SCL高电平时，SDA从高变低 */
void OLED_I2C_Start(void)
{
    OLED_W_SDA(1); OLED_W_SCL(1);
    OLED_W_SDA(0); OLED_W_SCL(0);
}

/* I2C 停止信号: SCL高电平时，SDA从低变高 */
void OLED_I2C_Stop(void)
{
    OLED_W_SDA(0); OLED_W_SCL(1);
    OLED_W_SDA(1);
}

/* 发送一个字节数据 */
void OLED_I2C_SendByte(uint8_t Byte)
{
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        // 依次取出字节的每一位发送
        OLED_W_SDA(!!(Byte & (0x80 >> i)));
        OLED_W_SCL(1); // 拉高SCL读取
        OLED_W_SCL(0); // 拉低SCL准备下一位
    }
    // 发送完后需要忽略一个应答位(ACK)
    OLED_W_SCL(1);
    OLED_W_SCL(0);
}

/* 写命令 (Command): 用于设置屏幕参数 */
void OLED_WriteCommand(uint8_t Command)
{
    OLED_I2C_Start();
    OLED_I2C_SendByte(0x78); // OLED地址
    OLED_I2C_SendByte(0x00); // 0x00 表示后面发的是命令
    OLED_I2C_SendByte(Command);
    OLED_I2C_Stop();
}

/* 写数据 (Data): 用于点亮屏幕像素 */
void OLED_WriteData(uint8_t Data)
{
    OLED_I2C_Start();
    OLED_I2C_SendByte(0x78);
    OLED_I2C_SendByte(0x40); // 0x40 表示后面发的是显示数据
    OLED_I2C_SendByte(Data);
    OLED_I2C_Stop();
}

/* ================= 2. 功能应用部分 ================= */

/* 屏幕初始化 (只需调用一次) */
void OLED_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    // 一次性初始化 PB6, PB7, PB8, PB9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // 配置供电引脚: PB6(GND)=0, PB7(VCC)=1
    GPIO_ResetBits(GPIOB, GPIO_Pin_6);
    GPIO_SetBits(GPIOB, GPIO_Pin_7);
    
    // 默认拉高通信线
    OLED_W_SCL(1);
    OLED_W_SDA(1);
    
    Delay_ms(20); // 等待屏幕上电稳定

    // --- 下面是一串标准的OLED启动命令，不需要背，照抄即可 ---
    OLED_WriteCommand(0xAE); // 关闭显示
    OLED_WriteCommand(0xD5); OLED_WriteCommand(0x80);
    OLED_WriteCommand(0xA8); OLED_WriteCommand(0x3F);
    OLED_WriteCommand(0xD3); OLED_WriteCommand(0x00);
    OLED_WriteCommand(0x40);
    OLED_WriteCommand(0xA1); // 左右方向正常
    OLED_WriteCommand(0xC8); // 上下方向正常
    OLED_WriteCommand(0xDA); OLED_WriteCommand(0x12);
    OLED_WriteCommand(0x81); OLED_WriteCommand(0xCF);
    OLED_WriteCommand(0xD9); OLED_WriteCommand(0xF1);
    OLED_WriteCommand(0xDB); OLED_WriteCommand(0x30);
    OLED_WriteCommand(0xA4);
    OLED_WriteCommand(0xA6);
    OLED_WriteCommand(0x8D); OLED_WriteCommand(0x14); // 打开电荷泵(必须)
    OLED_WriteCommand(0xAF); // 开启显示
    
    OLED_Clear();
}

/* 设置光标位置 (Y: 行 0-7, X: 列 0-127) */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
    OLED_WriteCommand(0xB0 | Y);                 // 设置页地址(行)
    OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4)); // 设置列高4位
    OLED_WriteCommand(0x00 | (X & 0x0F));        // 设置列低4位
}

/* 清屏 */
void OLED_Clear(void)
{
    uint8_t i, j;
    for (j = 0; j < 8; j++) {
        OLED_SetCursor(j, 0);
        for(i = 0; i < 128; i++) OLED_WriteData(0x00);
    }
}

/* 显示一个字符 */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{
    uint8_t i;
    // 字体是 8x16 像素，所以要分上下两半显示
    // 上半部分
    OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);
    for (i = 0; i < 8; i++) OLED_WriteData(OLED_F8x16[Char - ' '][i]);
    
    // 下半部分
    OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);
    for (i = 0; i < 8; i++) OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);
}

/* 显示字符串 (最常用) */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
    uint8_t i;
    for (i = 0; String[i] != '\0'; i++)
    {
        OLED_ShowChar(Line, Column + i, String[i]);
    }
}

/* 显示正整数 (简化版，去掉了Pow函数) */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    uint8_t i;
    // 倒着计算每一位，太麻烦，这里用一种简单的递推法提取每一位
    // 为了简化代码，建议直接用 sprintf 配合 ShowString，但这里保留基本实现
    
    // 这是一个简单的提取算法：先算除数
    uint32_t Divisor = 1;
    for(i = 1; i < Length; i++) Divisor *= 10;

    for (i = 0; i < Length; i++)
    {
        OLED_ShowChar(Line, Column + i, (Number / Divisor) % 10 + '0');
        Divisor /= 10;
    }
}
