#include "stm32f10x.h"
#include <stdio.h>
#include <stdarg.h>

char Serial_RxPacket[100];
uint8_t Serial_RxFlag;

void Serial_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    // 复用推挽输出（TX - PA9）
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 上拉输入（RX - PA10）
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_BaudRate = 115200; 
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1, &USART_InitStruct);
    
    // 使能接收中断
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    
    // 已经在 main.c 中统一配置为 NVIC_PriorityGroup_4
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    // FreeRTOS中，普通外设中断优先级建议在 5-15 之间
    // 这里设置为 7，确保安全
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStructure);
    
    USART_Cmd(USART1, ENABLE);
}

void Serial_SendByte(uint8_t Byte)
{
    USART_SendData(USART1, Byte);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
    uint16_t i;
    for (i = 0; i < Length; i++)
    {
        Serial_SendByte(Array[i]);
    }
}

void Serial_SendString(char *String)
{
    uint8_t i;
    for (i = 0; String[i] != '\0'; i++)
    {
        Serial_SendByte(String[i]);
    }
}

uint32_t Serial_Pow(uint32_t X, uint8_t Y)
{
    uint32_t Result = 1;
    while(Y--)
    {
        Result *= X;
    }
    return Result;
}

void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for(i = 0; i < Length; i++)
    {
        Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
    }
}

// 重定向 fputc
int fputc(int ch, FILE *g)
{
    Serial_SendByte(ch);
    return ch;
}

// 自定义 printf
void Serial_Printf(char *format, ...)
{
    char String[100];
    va_list arg;
    va_start(arg, format);
    vsprintf(String, format, arg);
    va_end(arg);
    Serial_SendString(String);
}


/*功能：接收文本数据包 (以 \r\n 或 \n 结尾的字符串)
   状态机逻辑：
     状态0：接收字符，存入缓冲
     状态1：收到 \r (可选)
     状态2：收到 \n -> 封包完成
*/
void USART1_IRQHandler(void)
{
    static uint8_t RxState = 0;       // 状态变量
    static uint8_t pRxPacket = 0;     // 缓冲区索引
    uint8_t Res;

    // 检查是否是接收中断
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        // 1. 读取当前字节
        Res = USART_ReceiveData(USART1);
        
        // 2. 状态机处理 (读取字符串，直到遇到换行)
        if(RxState == 0)
        {
            // 如果收到回车 '\r'，进入状态1 (等待 '\n')
            // 如果你的串口助手只发送 '\n'，可以简化这个逻辑
            if(Res == '\r')
            {
                RxState = 1;
            }
            // 如果直接收到 '\n' (部分工具只有换行)，直接完成
            else if(Res == '\n')
            {
                Serial_RxPacket[pRxPacket] = '\0'; // 字符串结束符
                Serial_RxFlag = 1;                 // 【关键】置标志位
                pRxPacket = 0;                     // 复位索引
                RxState = 0;                       // 复位状态
            }
            else
            {
                // 正常字符，存入缓冲区
                Serial_RxPacket[pRxPacket] = Res;
                pRxPacket++;
                
                // 防止缓冲区溢出
                if(pRxPacket >= 100) pRxPacket = 0; 
            }
        }
        else if(RxState == 1)
        {
            // 期待收到 '\n'
            if(Res == '\n')
            {
                Serial_RxPacket[pRxPacket] = '\0'; // 添加字符串结束符(很重要)
                Serial_RxFlag = 1;                 // 【关键】置标志位
                pRxPacket = 0;                     // 复位索引
                RxState = 0;                       // 复位状态
            }
            else
            {
                // 如果 '\r' 后面不是 '\n'，说明之前的 '\r' 只是普通字符？
                // 这种容错处理看情况，这里简单复位
                RxState = 0;
            }
        }
        
        // 清除中断标志 (读取DR其实已经自动清除，但手动清除更保险)
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}
