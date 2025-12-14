#ifndef __CONFIG_H
#define __CONFIG_H

#include "stm32f10x.h"

/* ---------------- 任务配置 ---------------- */
#define SENSOR_TASK_STACK_SIZE    256
#define CONTROL_TASK_STACK_SIZE   128
#define DISPLAY_TASK_STACK_SIZE   256
#define COMM_TASK_STACK_SIZE      512
#define POWER_TASK_STACK_SIZE     128

/* ---------------- 优先级配置 ---------------- */
#define CONTROL_TASK_PRIORITY     4
#define SENSOR_TASK_PRIORITY      3
#define COMM_TASK_PRIORITY        2
#define DISPLAY_TASK_PRIORITY     2
#define POWER_TASK_PRIORITY       1

/* ---------------- 硬件引脚定义 ---------------- */
// 蜂鸣器: PA0
#define BUZZER_PIN                GPIO_Pin_0
#define BUZZER_PORT               GPIOA

/* ---------------- 阈值定义 -------------------- */
#define AIR_QUALITY_THRESHOLD     100
#define TEMP_THRESHOLD_LOW        20.0f
#define TEMP_THRESHOLD_HIGH       30.0f
#define HUMI_THRESHOLD_LOW        40.0f
#define HUMI_THRESHOLD_HIGH       70.0f

#define LIGHT_THRESHOLD_DARK      10.0f
#define LIGHT_THRESHOLD_DIM       50.0f
#define LIGHT_THRESHOLD_NORMAL    200.0f
#define LIGHT_THRESHOLD_BRIGHT    500.0f

/* ---------------- 类型定义 ---------------- */
typedef enum {
    SYS_MODE_NORMAL = 0,
    SYS_MODE_SLEEP,
    SYS_MODE_EMERGENCY
} SystemMode_t;

typedef enum {
    CMD_LED_CONTROL = 0,
    CMD_MOTOR_CONTROL,
    CMD_HUMIDIFIER_CONTROL,
    CMD_BUZZER_CONTROL,
    CMD_SYSTEM_MODE
} ControlCommandType_t;

typedef struct {
    ControlCommandType_t cmd_type;
    int32_t param1;
} ControlCommand_t;

typedef struct {
    float temperature;
    float humidity;
    float light_intensity;
    float air_quality;
} SensorData_t;

typedef struct {
    SystemMode_t current_mode;
    uint32_t error_code;
    uint32_t uptime_sec;
} SystemStatus_t;

#endif
