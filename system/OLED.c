#include "stm32f10x.h"
#include "OLED_Font.h"
#include "Delay.h"
#include "OLED.h"

/* ================= 核心定义 ================= */

/* * 显存缓冲区 (Frame Buffer)
 * [128]: 列数 (0-127)
 * [8]:   页数 (0-7), 每一页代表8个垂直像素
 * 总大小: 128 * 8 = 1024 Bytes = 1KB
 */
uint8_t OLED_GRAM[128][8];

/* 引脚操作宏 */
void OLED_W_SCL(uint8_t x) { GPIO_WriteBit(GPIOB, GPIO_Pin_8, (BitAction)x); }
void OLED_W_SDA(uint8_t x) { GPIO_WriteBit(GPIOB, GPIO_Pin_9, (BitAction)x); }

/* ================= 1. 底层 I2C 模拟部分 (不变) ================= */

void OLED_I2C_Start(void)
{
    OLED_W_SDA(1); OLED_W_SCL(1);
    OLED_W_SDA(0); OLED_W_SCL(0);
}

void OLED_I2C_Stop(void)
{
    OLED_W_SDA(0); OLED_W_SCL(1);
    OLED_W_SDA(1);
}

void OLED_I2C_SendByte(uint8_t Byte)
{
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        OLED_W_SDA(!!(Byte & (0x80 >> i)));
        OLED_W_SCL(1);
        OLED_W_SCL(0);
    }
    OLED_W_SCL(1);
    OLED_W_SCL(0);
}

void OLED_WriteCommand(uint8_t Command)
{
    OLED_I2C_Start();
    OLED_I2C_SendByte(0x78);
    OLED_I2C_SendByte(0x00);
    OLED_I2C_SendByte(Command);
    OLED_I2C_Stop();
}

/* 注意：OLED_WriteData 这里其实很少直接用了，主要靠 Refresh 发送 */
void OLED_WriteData(uint8_t Data)
{
    OLED_I2C_Start();
    OLED_I2C_SendByte(0x78);
    OLED_I2C_SendByte(0x40);
    OLED_I2C_SendByte(Data);
    OLED_I2C_Stop();
}

/* ================= 2. 显存核心功能 (新增与修改) ================= */

/* * [重点函数] 全屏刷新
 * 作用：将 OLED_GRAM 数组的内容一次性搬运到屏幕上
 * 简历对应点：采用“全屏刷新”策略
 */
void OLED_Refresh_Gram(void)
{
    uint8_t i, n;
    for (i = 0; i < 8; i++)
    {
        OLED_WriteCommand(0xB0 + i); // 设置页地址 (0-7)
        OLED_WriteCommand(0x00);     // 设置列低地址
        OLED_WriteCommand(0x10);     // 设置列高地址
        
        OLED_I2C_Start();
        OLED_I2C_SendByte(0x78);
        OLED_I2C_SendByte(0x40); // 准备写数据
        
        for (n = 0; n < 128; n++)
        {
            OLED_I2C_SendByte(OLED_GRAM[n][i]); // 发送缓冲区数据
        }
        OLED_I2C_Stop();
    }
}

/* 清屏：只清空数组，不操作硬件 (速度极快) */
void OLED_Clear(void)
{
    uint8_t i, n;
    for (i = 0; i < 8; i++)
    {
        for (n = 0; n < 128; n++)
        {
            OLED_GRAM[n][i] = 0x00; // 填充 0x00 (黑)
        }
    }
}

/* 画字符：修改数组，不操作硬件 */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{
    uint8_t i;
    // 你的字体是 F8x16，一个字占 2 页 (16像素高)
    // 算出该字符在显存中的坐标
    uint8_t page_up = (Line - 1) * 2;     // 上半部分页码
    uint8_t page_down = (Line - 1) * 2 + 1; // 下半部分页码
    uint8_t x_start = (Column - 1) * 8;   // 起始列 (0-127)

    // 防止数组越界
    if (x_start > 120 || page_down > 7) return;

    for (i = 0; i < 8; i++)
    {
        // 写入显存 (只修改内存，不发 I2C)
        OLED_GRAM[x_start + i][page_up] = OLED_F8x16[Char - ' '][i];
        OLED_GRAM[x_start + i][page_down] = OLED_F8x16[Char - ' '][i + 8];
    }
}

/* 显示字符串 (沿用原有逻辑，底层已指向 GRAM) */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
    uint8_t i;
    for (i = 0; String[i] != '\0'; i++)
    {
        OLED_ShowChar(Line, Column + i, String[i]);
    }
}

/* 显示数字 (沿用原有逻辑) */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    uint8_t i;
    uint32_t Divisor = 1;
    for(i = 1; i < Length; i++) Divisor *= 10;

    for (i = 0; i < Length; i++)
    {
        OLED_ShowChar(Line, Column + i, (Number / Divisor) % 10 + '0');
        Divisor /= 10;
    }
}

/* 初始化函数 */
void OLED_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    GPIO_ResetBits(GPIOB, GPIO_Pin_6); // GND
    GPIO_SetBits(GPIOB, GPIO_Pin_7);   // VCC
    
    OLED_W_SCL(1);
    OLED_W_SDA(1);
    Delay_ms(20);

    // 标准初始化命令
    OLED_WriteCommand(0xAE);
    OLED_WriteCommand(0xD5); OLED_WriteCommand(0x80);
    OLED_WriteCommand(0xA8); OLED_WriteCommand(0x3F);
    OLED_WriteCommand(0xD3); OLED_WriteCommand(0x00);
    OLED_WriteCommand(0x40);
    OLED_WriteCommand(0xA1);
    OLED_WriteCommand(0xC8);
    OLED_WriteCommand(0xDA); OLED_WriteCommand(0x12);
    OLED_WriteCommand(0x81); OLED_WriteCommand(0xCF);
    OLED_WriteCommand(0xD9); OLED_WriteCommand(0xF1);
    OLED_WriteCommand(0xDB); OLED_WriteCommand(0x30);
    OLED_WriteCommand(0xA4);
    OLED_WriteCommand(0xA6);
    OLED_WriteCommand(0x8D); OLED_WriteCommand(0x14);
    OLED_WriteCommand(0xAF);
    
    OLED_Clear();        // 1. 清空内存
    OLED_Refresh_Gram(); // 2. 刷新屏幕 (全黑)
}
