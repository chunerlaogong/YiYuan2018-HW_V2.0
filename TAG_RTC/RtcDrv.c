#include "stm8l15x_conf.h"
#include "RtcDrv.h"

//#define CLK_RTC BITs(2)
//#define WAKEUP_CLK 204
//U16 m_nCLK_RTC = 18999;

//void RtcCalibrate()
//{
//	InitRtc(9);
//	EnableRtcInt();
//	ReSetWakeup(10);
//}

//U8 InitRtc(U16 nClk)	//返回1表示时间丢失
//{
//	U8 ret = 1;
//	
//	RTC->WPR = 0xCA;//打开RTC写保护寄存器step1
//	RTC->WPR = 0x53;//打开RTC写保护寄存器step2
//	
//	while(CLK->CRTCR & CLK_CRTCR_RTCSWBSY);  	//RTC CLK寄存器 等待RTC clock change procedure is complete
//	if(CLK->CRTCR == CLK_RTC)//  #define CLK_RTC BITs(2),CLK_CRTCR=LSI clock used as RTC clock source  26uS
//	{
//		ret = 0;
//	}
//	CLK->CRTCR = CLK_RTC;//??
//	CLK->CRTCR = 0x10;//??
//	
//	RTC->CR2 = 0;//RTC_CR2寄存器，使能，Wakeup timer disable
//	while(!(RTC->ISR1 & RTC_ISR1_WUTWF));	//RTC初始化寄存器1和，等待禁止唤醒时间更新
//	
//	RTC->WUTRH = S16H(nClk);//#define S16H(X) (U8)((X)>>8)
//	RTC->WUTRL = S16L(nClk);//#define S16L(X) (U8)(X)
//	
//	RTC->CR1 = 0;////RTC控制寄存器１，RTCCLK/16 clock is selected
//	
//	//	RTC->ISR1_INIT = 1;////初始化和状态寄存器1，
//	RTC->APRER = 0x7F; //异步分频寄存器,ck_apre frequency = fRTCCLK frequency/ (PREDIV_A+1)
//	RTC->SPRERH = 0;//同步分频寄存器MSB,ck_spre frequency = ck_aprefrequency/ (PREDIV_S+1).
//	RTC->SPRERL = 0xff;//同步分频寄存器ＬSB,ck_spre frequency = ck_aprefrequency/ (PREDIV_S+1).
//	//   RTC->ISR1_INIT = 0;
//	RTC->WPR = 0;
//	return ret;
//}
//void ReSetWakeup(U16 nMS)	// mS
//{
//	nMS = m_nCLK_RTC * ((U32)nMS) / 1000L;
//	RTC->WPR = 0xCA;
//	RTC->WPR = 0x53;
//	RTC->CR2 = 0;
//	while(!(RTC->ISR1 & RTC_ISR1_WUTWF));
//	RTC->WUTRH = S16H(nMS);
//	RTC->WUTRL = S16L(nMS);
//	RTC->CR2 = BITs(2) + BITs(6);
//	RTC->WPR = 0;
//}

//void EnableRtcInt()
//{
//	RTC->WPR = 0xCA;
//	RTC->WPR = 0x53;
//	RTC->CR2 = BITs(2) + BITs(6);
//	RTC->WPR = 0;
//}
//void DisableRtcInt()
//{
//	RTC->WPR = 0xCA;
//	RTC->WPR = 0x53;
//	RTC->CR2 = 0;
//	RTC->WPR = 0;
//}
