#include "stm8l15x_conf.h"
#include "GPIODrv.h"

void KEY_EXTI_config(void)
{ 
    GPIO_Init(GPIOD, GPIO_Pin_3, GPIO_Mode_In_FL_IT);/*GPIOB0����Ϊ�����ж�ʹ��*/
    EXTI_SetPinSensitivity(EXTI_Pin_3, EXTI_Trigger_Rising);/*�ж�0Ϊ�����ص͵�ƽ����*/
}
