/*
 * FileName: RTC_Driver.c
 * Describe: 
 * Author: HL 
 * Date: 2017-10-31
 */

/* Includes */
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>

#include "stm8l15x_conf.h"
#include "MyNBIOT.h"
#include "RTC_Driver.h"

/* Defines */

/* static variables definitions */

/* static functions definitions */

/* variables */
RTC_InitTypeDef		RTC_InitStr;
sCalendar			rtc_Calendar;

/* Functions Definitions */
/*
 * Name:Calendar_Init
 * Describe: 初始化Calendar
 * Input: Null
 * Output: Null
 * Author: HL
 * Data:2017-10-31
 */
uint16_t tmp[6];
void Calendar_Init(void){							// 初始时间2017-10-31 13:41:00 星期二
	RTC_InitStr.RTC_HourFormat = RTC_HourFormat_24;	
	RTC_InitStr.RTC_AsynchPrediv = 124;
	RTC_InitStr.RTC_SynchPrediv = 303;
	RTC_Init(&RTC_InitStr);
	
	RTC_DateStructInit(&rtc_Calendar.localCalendar.RTC_DateStr);
	rtc_Calendar.localCalendar.RTC_DateStr.RTC_WeekDay =RTC_Weekday_Tuesday;
	rtc_Calendar.localCalendar.RTC_DateStr.RTC_Date = 31;
	rtc_Calendar.localCalendar.RTC_DateStr.RTC_Month = RTC_Month_October;
	rtc_Calendar.localCalendar.RTC_DateStr.RTC_Year = 17;
	RTC_SetDate(RTC_Format_BIN, &rtc_Calendar.localCalendar.RTC_DateStr);
	
	RTC_TimeStructInit(&rtc_Calendar.localCalendar.RTC_TimeStr);
	rtc_Calendar.localCalendar.RTC_TimeStr.RTC_Hours = 13;
	rtc_Calendar.localCalendar.RTC_TimeStr.RTC_Minutes = 41;
	rtc_Calendar.localCalendar.RTC_TimeStr.RTC_Seconds = 0;
	RTC_SetTime(RTC_Format_BIN, &rtc_Calendar.localCalendar.RTC_TimeStr);
}

/*
 * Test Get Date and Time
 * Output: Current Date and Time
 */
void getLocalDateAndTime(void){
	while (RTC_WaitForSynchro() != SUCCESS);	// wait the Calander sychonized
	RTC_GetTime(RTC_Format_BIN, &rtc_Calendar.localCalendar.RTC_TimeStr);
	RTC_GetDate(RTC_Format_BIN, &rtc_Calendar.localCalendar.RTC_DateStr);
}

/*
 * Name: 
 * 
 */
void setDateAndTime(pCalendar pcalendar){
	RTC_SetTime(RTC_Format_BIN, &pcalendar->InternetCalendar.RTC_TimeStr);
	RTC_SetDate(RTC_Format_BIN, &pcalendar->InternetCalendar.RTC_DateStr);
	while (RTC_WaitForSynchro() != SUCCESS);	// wait the Calander sychonized
	RTC_GetTime(RTC_Format_BIN, &pcalendar->localCalendar.RTC_TimeStr);
	RTC_GetDate(RTC_Format_BIN, &pcalendar->localCalendar.RTC_DateStr);
}

/*
 * Name: getLocalTimeStamp
 * Describe:  转换本地时间成时间戳
 * Input: Calendar
 * Output: NULL, save timstamp into struct
 */
void getLocalTimeStamp(pCalendar pcalendar){
	struct tm stm;
	int16_t iY,iM,iD,iH,iMin,iS;
	
	/* get Date & Time*/
	RTC_GetTime(RTC_Format_BIN, &pcalendar->localCalendar.RTC_TimeStr);
	RTC_GetDate(RTC_Format_BIN, &pcalendar->localCalendar.RTC_DateStr);
	
	memset(&stm, 0, sizeof(stm));
	
	iY = pcalendar->localCalendar.RTC_DateStr.RTC_Year;
	iM = pcalendar->localCalendar.RTC_DateStr.RTC_Month;
	iD = pcalendar->localCalendar.RTC_DateStr.RTC_Date;
	iH = pcalendar->localCalendar.RTC_TimeStr.RTC_Hours;
	iMin = pcalendar->localCalendar.RTC_TimeStr.RTC_Minutes;
	iS = pcalendar->localCalendar.RTC_TimeStr.RTC_Seconds;
	
	stm.tm_year = iY + 100;	//refer to time.h
	stm.tm_mon = iM - 1;
	stm.tm_mday = iD;
	stm.tm_hour = iH;
	stm.tm_min = iMin;
	stm.tm_sec = iS;
	
	pcalendar->local_stamp = mktime(&stm);
}

uint32_t getInternetTimeStamp(pNB pMyNB){
	struct tm stm;
	uint16_t tmp[6];
	if(do_NB_CMD(pMyNB, AT_CCLK) == SUCCESS)
	{
		char *ptr = strstr(pMyNB->Rx_buff, "+CCLK:");
		sscanf(ptr, "+CCLK:%d/%d/%d,%d:%d:%d", &tmp[0], &tmp[1], &tmp[2], &tmp[3], &tmp[4], &tmp[5]);
		stm.tm_year = tmp[0] + 100;
		stm.tm_mon 	= tmp[1] - 1;
		stm.tm_mday = tmp[2];
		stm.tm_hour = tmp[3];
		stm.tm_min 	= tmp[4];
		stm.tm_sec 	= tmp[5];
		return mktime(&stm);
	}
	else
	{
		return 0;
	}
}