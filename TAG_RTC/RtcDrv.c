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

//U8 InitRtc(U16 nClk)	//����1��ʾʱ�䶪ʧ
//{
//	U8 ret = 1;
//	
//	RTC->WPR = 0xCA;//��RTCд�����Ĵ���step1
//	RTC->WPR = 0x53;//��RTCд�����Ĵ���step2
//	
//	while(CLK->CRTCR & CLK_CRTCR_RTCSWBSY);  	//RTC CLK�Ĵ��� �ȴ�RTC clock change procedure is complete
//	if(CLK->CRTCR == CLK_RTC)//  #define CLK_RTC BITs(2),CLK_CRTCR=LSI clock used as RTC clock source  26uS
//	{
//		ret = 0;
//	}
//	CLK->CRTCR = CLK_RTC;//??
//	CLK->CRTCR = 0x10;//??
//	
//	RTC->CR2 = 0;//RTC_CR2�Ĵ�����ʹ�ܣ�Wakeup timer disable
//	while(!(RTC->ISR1 & RTC_ISR1_WUTWF));	//RTC��ʼ���Ĵ���1�ͣ��ȴ���ֹ����ʱ�����
//	
//	RTC->WUTRH = S16H(nClk);//#define S16H(X) (U8)((X)>>8)
//	RTC->WUTRL = S16L(nClk);//#define S16L(X) (U8)(X)
//	
//	RTC->CR1 = 0;////RTC���ƼĴ�������RTCCLK/16 clock is selected
//	
//	//	RTC->ISR1_INIT = 1;////��ʼ����״̬�Ĵ���1��
//	RTC->APRER = 0x7F; //�첽��Ƶ�Ĵ���,ck_apre frequency = fRTCCLK frequency/ (PREDIV_A+1)
//	RTC->SPRERH = 0;//ͬ����Ƶ�Ĵ���MSB,ck_spre frequency = ck_aprefrequency/ (PREDIV_S+1).
//	RTC->SPRERL = 0xff;//ͬ����Ƶ�Ĵ�����SB,ck_spre frequency = ck_aprefrequency/ (PREDIV_S+1).
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
