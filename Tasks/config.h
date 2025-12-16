#ifndef __CONFIG_H
#define __CONFIG_H

#include "stm32f10x.h"

/* ---------------- 任务配置 ---------------- */
#define SENSOR_TASK_STACK_SIZE    256
#define CONTROL_TASK_STACK_SIZE   128
#define DISPLAY_TASK_STACK_SIZE   256
#define COMM_TASK_STACK_SIZE      512

/* ---------------- 优先级配置 ---------------- */
#define CONTROL_TASK_PRIORITY     4
#define SENSOR_TASK_PRIORITY      3
#define COMM_TASK_PRIORITY        2
#define DISPLAY_TASK_PRIORITY     2

/* ---------------- 硬件引脚定义 ---------------- */
// 蜂鸣器: PA0
#define BUZZER_PIN                GPIO_Pin_0
#define BUZZER_PORT               GPIOA

/* ---------------- 类型定义 ---------------- */

/* 系统模式：0-自动(正常), 1-手动 */
typedef enum {
    SYS_MODE_AUTO = 0,
    SYS_MODE_MANUAL = 1,
} SystemMode_t;

/* 控制命令类型 */
typedef enum {
    CMD_LED_CONTROL = 0,
    CMD_MOTOR_CONTROL,
    CMD_HUMIDIFIER_CONTROL,
    CMD_BUZZER_CONTROL,
    CMD_SYSTEM_MODE
} ControlCommandType_t;

/* 命令结构体 */
typedef struct {
    ControlCommandType_t cmd_type;	//存入命令
    int32_t param;					//参数
} ControlCommand_t;

/* 传感器数据结构体 */
typedef struct {
    float temperature;
    float humidity;
    float light_intensity;
    float air_quality;
} SensorData_t;

#endif
