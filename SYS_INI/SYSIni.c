#include "stm8l15x_conf.h"
#include "SYSIni.h"

void CLK_RTC_Ini(void)
{
	CLK_HSICmd(ENABLE);
	CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);
	CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
	CLK_RTCClockConfig(CLK_RTCCLKSource_LSI,CLK_RTCCLKDiv_1); //内部低速时钟
	/* Wait for LSI clock to be ready */
//	while (CLK_GetFlagStatus(CLK_FLAG_LSIRDY) == RESET);
	
	CLK_PeripheralClockConfig(CLK_Peripheral_ADC1,ENABLE);
	CLK_PeripheralClockConfig(CLK_Peripheral_RTC,ENABLE);
	CLK_PeripheralClockConfig(CLK_Peripheral_USART3, ENABLE);//串口3时钟使能
	RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);
	RTC_ITConfig(RTC_IT_WUT, ENABLE);
	
	RTC_WakeUpCmd(DISABLE);
	RTC_AlarmCmd(DISABLE);
	RTC_ITConfig(RTC_IT_ALRA, DISABLE);
	RTC_ITConfig(RTC_IT_WUT, DISABLE); 
}

void GoSleep(unsigned int nS)
{
	RTC_WakeUpCmd(DISABLE);//关闭自动唤醒功能
	CLK_RTCClockConfig(CLK_RTCCLKSource_LSI,CLK_RTCCLKDiv_1);
	RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);
	RTC_ITConfig(RTC_IT_WUT, ENABLE);
	RTC_SetWakeUpCounter(nS);	// 2375*second
	RTC_WakeUpCmd(ENABLE);//开启自动唤醒功能
}