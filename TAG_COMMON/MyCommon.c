/*
 *@file: 	MyCommon.c
 *@author:	HL
 *@date:	2017-12-18
 *@version:	V1.0
 */

/* includes */
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "stm8l15x_conf.h"
#include "MyCommon.h"

/* functions */

void mini_delay(uint8_t cc)//延时cc us，最小值2us
{
    uint8_t i;
    asm("nop");	
    asm("nop");	
    asm("nop");	
    asm("nop");	
    asm("nop");	
    asm("nop");	
    asm("nop");	    
    asm("nop");	
    asm("nop");	
    asm("nop");	
    asm("nop");	
	for ( i = 2; i < cc; i++)
    {
        
        asm("nop");	
        asm("nop");	
        asm("nop");	
        asm("nop");	
        asm("nop");	
        asm("nop");
        asm("nop");
        asm("nop");
    }

}

void DelayOneMs(uint32_t d_ms)
{
	uint32_t i;
	uint16_t j;
	for(i=0; i<d_ms; i++)
	{
		for (j=0; j<409; j++)//352
		{
			mini_delay(2);
		}
	}
}
/*
 * @name: 	Hex_to_ASCII
 * @author:	COPY
 * @date:	2017-11-23
 * @brief:
 */
void Hex_to_ASCII(char *pHex, char *pASCII, uint8_t len)
{
	uint8_t i = 0, j = 0;
	char str[100];
	for(i = 0; i < len; i++)
	{
		str[j] = (*pHex) >> 4;
		str[j+1] = (*pHex) & 0x0f;
		pHex++;
		j += 2;
	}
	for(i = 0; i < j; i++)
	{
		sprintf((char *)pASCII, "%X", str[i]);
		pASCII++;
	}
}

/*
 * @name: 	NBiotPortConfig
 * @author:	HL
 * @date:	2017-11-23
 * @brief:	PE0:SDA, PE1:SCL,PB0:Power,PD0:Interrupt Input
 */
void NBIotPortConfig(void){
	GPIO_Init(GPIOE, GPIO_Pin_6, GPIO_Mode_Out_PP_Low_Fast);	//PA2 TX
	GPIO_Init(GPIOE, GPIO_Pin_7, GPIO_Mode_In_PU_No_IT);		//PA3 RX
}

/*
 * @name: 	BTPortConfig
 * @author:	HL
 * @date:	2017-11-23
 * @brief:	PE0:SDA, PE1:SCL,PB0:Power,PD0:Interrupt Input
 */
void BTPortConfig(void){
	GPIO_Init(GPIOA, GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Fast);	//PA2 TX
	GPIO_Init(GPIOA, GPIO_Pin_3, GPIO_Mode_In_PU_No_IT);		//PA3 RX
}

/*
 * @name: 	NFC_Port_Config
 * @author:	HL
 * @date:	2017-11-23
 * @port:	PortD, Pin3
 */
void NFC_Port_Config(void){
	GPIO_Init(GPIOD, GPIO_Pin_3, GPIO_Mode_In_FL_IT);	//Input floating, external interrupt
}

/*
 * @name: 	SensorR3100PortConfig
 * @author:	HL
 * @date:	2017-11-23
 * @brief:	PE0:SDA, PE1:SCL,PB0:Power,PD0:Interrupt Input
 */
void SensorR3100PortConfig(void){
	GPIO_Init(GPIOE, GPIO_Pin_0, GPIO_Mode_Out_PP_Low_Fast);
	GPIO_Init(GPIOE, GPIO_Pin_1, GPIO_Mode_Out_PP_Low_Fast);
	GPIO_Init(GPIOB, GPIO_Pin_0, GPIO_Mode_Out_PP_Low_Fast);
	GPIO_Init(GPIOD, GPIO_Pin_0, GPIO_Mode_In_FL_IT);
}