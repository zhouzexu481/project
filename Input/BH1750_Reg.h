#ifndef _BH1750_REG_H
#define _BH1750_REG_H

// BH1750 I2C地址
#define BH1750_ADDRESS_WRITE    0x46    // 写地址 (0x23 << 1)
#define BH1750_ADDRESS_READ     0x47    // 读地址 (0x23 << 1 | 0x01)

// BH1750指令集
#define BH1750_POWER_DOWN       0x00    // 断电
#define BH1750_POWER_ON         0x01    // 上电
#define BH1750_RESET            0x07    // 重置数据寄存器
#define BH1750_CONT_H_MODE      0x10    // 连续高分辨率模式
#define BH1750_CONT_H_MODE2     0x11    // 连续高分辨率模式2
#define BH1750_CONT_L_MODE      0x13    // 连续低分辨率模式
#define BH1750_ONE_H_MODE       0x20    // 一次高分辨率模式
#define BH1750_ONE_H_MODE2      0x21    // 一次高分辨率模式2
#define BH1750_ONE_L_MODE       0x23    // 一次低分辨率模式

// 测量时间定义
#define BH1750_MEASUREMENT_TIME_MS  180 // 高分辨率模式测量时间

#endif
