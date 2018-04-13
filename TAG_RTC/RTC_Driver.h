/*
 * FileName: RTC_Driver.h
 * Describe: 
 * Author: HL 
 * Date: 2017-10-31
 */
#ifndef _RTC_Driver_h
#define _RTC_Driver_h

/* Typedefs  */
typedef enum {RTCERROR = 0, RTCSUCCESS = !RTCERROR} RtcErrorStatus;

typedef struct{
	RTC_TimeTypeDef		RTC_TimeStr;
	RTC_DateTypeDef		RTC_DateStr;
}sRTCCalendar,*pRTCCalendar;

typedef struct{
	uint32_t local_stamp;
	uint32_t Internet_stamp;
	uint32_t Internet_stamp_old;
	uint32_t local_stamp_old;
	int32_t loca_stamp_diff;	//stamp_diff = Internet_stamp - local_stamp
	int32_t	Internet_stamp_diff;
	uint32_t stamp_threshold;	
	sRTCCalendar localCalendar;
	sRTCCalendar InternetCalendar;
	uint8_t init_flag;
}sCalendar,*pCalendar;

typedef enum {LOCAL = 0, INTERNET = !LOCAL}RtcLocation;

/* extern Functions declaration */
//extern void Calendar_Init(void);
//extern void getLocalDateAndTime(void);
////extern RtcErrorStatus dealInternetTime(pFrom_NB NB_module);
//extern void setDateAndTime(pCalendar pcalendar);
//extern void getInternetTimeStamp(pCalendar pcalendar);
//extern void getLocalTimeStamp(pCalendar pcalendar);
////extern void AdjustRTCClock(pFrom_NB NB_module, pCalendar pcalendar);
///* extern variables declaration */
//extern sCalendar	rtc_Calendar;
uint32_t getInternetTimeStamp(pNB pMyNB);

#endif