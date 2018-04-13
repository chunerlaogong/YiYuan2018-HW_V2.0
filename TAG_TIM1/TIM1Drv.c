#include "stm8l15x_conf.h"
#include "TIM1Drv.h"

void TIM1_config(void)
{ 
	TIM1_TimeBaseInit(16000,TIM1_CounterMode_Down,1000,0);
	TIM1_ClearFlag(TIM1_FLAG_Update);     
	TIM1_ITConfig(TIM1_IT_Update, ENABLE);
	// enableInterrupts();
	TIM1_Cmd(ENABLE);/* Enable TIM1 */
}