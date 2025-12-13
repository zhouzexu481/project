// filePath: Power_Mode.c
#include "Power_Mode.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_rcc.h"

static PowerMode_TypeDef current_power_mode = POWER_MODE_NORMAL;

void PowerMode_Init(void) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    PWR_WakeUpPinCmd(ENABLE);
}

void PowerMode_SetMode(PowerMode_TypeDef mode) {
    current_power_mode = mode;
    switch(mode) {
        case POWER_MODE_SLEEP:
            __WFI(); 
            break;
        case POWER_MODE_STOP:
            PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
            /* 从STOP唤醒后，时钟会变回HSI，必须重新配置为HSE */
            SystemInit(); 
            current_power_mode = POWER_MODE_NORMAL;
            break;
        default: break;
    }
}

PowerMode_TypeDef PowerMode_GetCurrentMode(void) {
    return current_power_mode;
}
