#include "stm8l15x_conf.h"
#include "GPIODrv.h"

void KEY_EXTI_config(void)
{ 
    GPIO_Init(GPIOD, GPIO_Pin_3, GPIO_Mode_In_FL_IT);/*GPIOB0设置为输入中断使能*/
    EXTI_SetPinSensitivity(EXTI_Pin_3, EXTI_Trigger_Rising);/*中断0为上升沿低电平触发*/
}
