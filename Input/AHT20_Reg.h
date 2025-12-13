#ifndef _AHT20_REG_H
#define _AHT20_REG_H

// AHT20命令定义
#define AHT20_SOFT_RESET_CMD	0xBA	// 软复位命令
#define AHT20_INIT_CMD			0xBE	// 初始化命令
#define AHT20_MEASURE_CMD		0xAC	// 触发测量命令

// AHT20状态位定义
#define AHT20_STATUS_BUSY		0x80	// 忙状态位
#define AHT20_STATUS_CALIBRATED	0x08	// 校准使能位

// AHT20参数定义
#define AHT20_INIT_PARAM1		0x08	// 初始化参数1
#define AHT20_INIT_PARAM2		0x00	// 初始化参数2
#define AHT20_MEASURE_PARAM1	0x33	// 测量参数1
#define AHT20_MEASURE_PARAM2	0x00	// 测量参数2

// AHT20地址定义
#define AHT20_ADDRESS			0x70	// 写地址 (0x38 << 1)
#define AHT20_READ_ADDRESS		0x71	// 读地址 (0x38 << 1 | 0x01)

// 数据长度定义
#define AHT20_DATA_LENGTH		6		// 数据长度（6字节）

#endif
