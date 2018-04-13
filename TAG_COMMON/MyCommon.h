/*
 *@file: 	MyCommon.h
 *@author:	HL
 *@date:	2017-12-18
 *@version:	V1.0
 */

#ifndef __MyCommon_h
#define __MyCommon_h

void mini_delay(uint8_t cc);
void DelayOneMs(uint32_t d_ms);
void Hex_to_ASCII(char *pHex, char *pASCII, uint8_t len);
void NBIotPortConfig(void);
void BTPortConfig(void);
void SensorR3100PortConfig(void);
void NFC_Port_Config(void);

#endif