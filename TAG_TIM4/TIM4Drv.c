/*
 * @file: 	TIM4Drv.c
 *@author:	HL
 *@date:	2017-12-18
 *@version:	V1.0
 */

#include "stm8l15x_conf.h"
#include "TIM4Drv.h"

void TIM4_config(void)
{
	CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE);
	TIM4_TimeBaseInit(TIM4_Prescaler_128,124);          //MAX INTERVAL TIME 1ms
	TIM4_ClearFlag(TIM4_FLAG_Update);       
	TIM4_ITConfig(TIM4_IT_Update, ENABLE);
//	TIM4_Cmd(ENABLE); 									/* Enable TIM4 */
}
