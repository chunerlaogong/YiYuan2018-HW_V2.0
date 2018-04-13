#include "stm8l15x_conf.h"
#include "SYSIni.h"

void CLK_RTC_Ini(void)
{
	CLK_HSICmd(ENABLE);
	CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);
	CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
	CLK_RTCClockConfig(CLK_RTCCLKSource_LSI,CLK_RTCCLKDiv_1); //�ڲ�����ʱ��
	/* Wait for LSI clock to be ready */
//	while (CLK_GetFlagStatus(CLK_FLAG_LSIRDY) == RESET);
	
	CLK_PeripheralClockConfig(CLK_Peripheral_ADC1,ENABLE);
	CLK_PeripheralClockConfig(CLK_Peripheral_RTC,ENABLE);
	CLK_PeripheralClockConfig(CLK_Peripheral_USART3, ENABLE);//����3ʱ��ʹ��
	RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);
	RTC_ITConfig(RTC_IT_WUT, ENABLE);
	
	RTC_WakeUpCmd(DISABLE);
	RTC_AlarmCmd(DISABLE);
	RTC_ITConfig(RTC_IT_ALRA, DISABLE);
	RTC_ITConfig(RTC_IT_WUT, DISABLE); 
}

void GoSleep(unsigned int nS)
{
	RTC_WakeUpCmd(DISABLE);//�ر��Զ����ѹ���
	CLK_RTCClockConfig(CLK_RTCCLKSource_LSI,CLK_RTCCLKDiv_1);
	RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);
	RTC_ITConfig(RTC_IT_WUT, ENABLE);
	RTC_SetWakeUpCounter(nS);	// 2375*second
	RTC_WakeUpCmd(ENABLE);//�����Զ����ѹ���
}