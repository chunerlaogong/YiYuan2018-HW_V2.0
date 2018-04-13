#ifndef _RF6936_SPI_PIC__H__
#define _RF6936_SPI_PIC__H__

#include "Common.h"
#include  "McuTarger.h"



enum RF_STATE
{
	RS_OFF,
	RS_REC,
	RS_REC_ING,
	RS_REC_GET,
	RS_REC_ERR,

	RS_SEND,
	RS_SEND_ING,
	RS_SEND_END,
	RS_SEND_ERR,
};
U8 RfResend(U8 count);
U8 GetRfRx();
void RfSetCh(U8 cCh);
void RfSetPowerOut(U8 cPow);


#define ENABLE_RF_IRQ_INT()  GPIOD->CR2|=0x10;		//PD_CR2_C24 = 1
#define DISABLE_RF_IRQ_INT() GPIOD->CR2&=0xef		//PD_CR2_C24 = 0

//#define ENABLE_RF_IRQ_INT()	PD_CR2_C24 = 1
//#define DISABLE_RF_IRQ_INT() PD_CR2_C24 = 0

#define RF_RUN_CH 0

#define IF_RF_IRQ()  INT0IF

void EndRfRecv();
U8 RfReadRssi();
U8 GetRfState();
void SpiInit();//≥ı ºªØSPI
void SpiWriteByte(U8 cAddr,U8 cc);
void SpiWirteByteBuff(U8 cAddr,U8* pBuff,U8 cLen);
void SpiWirteByteConst(U8 cAddr,const U8* pBuff,U8 cLen);
void SpiReadByteBuff(U8 cAddr,U8* pBuff,U8 cLen);
U8 SpiReadByte(U8 cAddr);
void RfChangeState(U8 cState);

void RfInit(U8 cCH, U8 cPow);
void RfTest();
U8 RfSend(unsigned char* buf,unsigned char count,U8 cCH);
U8 RfGetPocket(U8* buff,U8* pcRSSI);
void RfReRecv();
void RfInitForRecv(U8 cCH, U8 cPow);
void RfCtrlRecv(U8 cCh);
void RfSleep();
void RfWakeUp();
void RfListenCh(U8 cChannel);
void OnRfIRQInt();
#endif //_RF6936_SPI_PIC__H__
