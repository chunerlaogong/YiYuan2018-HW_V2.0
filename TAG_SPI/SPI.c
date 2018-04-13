
#include "Spi.h"
#include "Cyrf6936.h"

#define RF_RECV_BUFF_LEN 128
static U8 m_csRfRecvbuff[RF_RECV_BUFF_LEN];
static U8 m_csRfSendbuff[RF_RECV_BUFF_LEN - 16];

U8 m_cRSSI = 0;


static U8 m_cRfRecvCount = 0;
static U8 m_cRfSendCount = 0;
static U8 m_cRfSendLen = 0;
VU8 m_cRfState = 0;
U8 GetRfState()
{
	return m_cRfState;
}
void SpiInit()//初始化SPI
{
	CLK->PCKENR1 |= BITs(4); //SYSCLK to peripheral enabled SPI
  //  RF_CS_INIT();//CS置高
	RF_CS_set;		// = 1;//PB4=1,CS置高
	SPI1->CR2 = 3;
	SPI1->CR1 = BITs(6) + BITs(2) + BITs(4)  ; // fSYSCLK/4     BITs(X) (1<<(X))  0100 1100
	//PC_CR2_C20 = 1;	//
	//PB_CR2_C25 = 1;
	//PB_CR2_C27 = 1;


}
U8 SpiSendAndGetByte(U8 cD)
{
	SPI1->DR = cD;
	while(!(SPI1->SR & SPI_SR_RXNE));	// == 0);
	return SPI1->DR;
}

void SpiWriteByte(U8 cAddr,U8 cc)
{
	RF_CS_clr;		 //= 0;
	SpiSendAndGetByte(cAddr | 0x80);
	SpiSendAndGetByte(cc);
	RF_CS_set;		// = 1;
}
void SpiWirteByteBuff(U8 cAddr,U8* pBuff,U8 cLen)
{
	U8 i;
	RF_CS_clr;		// = 0;
	SpiSendAndGetByte(cAddr | 0x80);
	for (i = 0; i<cLen; i++)
	{
		SpiSendAndGetByte(pBuff[i]);
	}
	RF_CS_set;		// = 1;
}
void SpiWirteByteConst(U8 cAddr,const U8* pBuff,U8 cLen)
{
	U8 i;
	RF_CS_clr;		// = 0;
	SpiSendAndGetByte(cAddr | 0x80);
	for (i = 0; i<cLen; i++)
	{
		SpiSendAndGetByte(pBuff[i]);
	}
	RF_CS_set;		// = 1;
}
void SpiReadByteBuff(U8 cAddr,U8* pBuff,U8 cLen)
{
	U8 i;
	RF_CS_clr;		// = 0;
	SpiSendAndGetByte(cAddr & 0x7F);
	for (i = 0; i<cLen; i++)
	{
		pBuff[i] = SpiSendAndGetByte(0xFF);
	}
	RF_CS_set;		// = 1;
}
U8 SpiReadByte(U8 cAddr)
{
	U8 cTemp;
	RF_CS_clr;		// = 0;PB4
	SpiSendAndGetByte(cAddr & 0x7F);
	cTemp = SpiSendAndGetByte(0xff);
	RF_CS_set;		// = 1;
	return cTemp;
}

void RfSetCh(U8 cCh)
{
    DISABLE_RF_IRQ_INT();
    SpiWriteByte(CHANNEL_ADR, cCh);
}

void RfSetPowerOut(U8 cPow)
{
    DISABLE_RF_IRQ_INT();
    //SpiWriteByte(TX_CFG_ADR, (cPow | BITs(3)) & 0x0F);
    SpiWriteByte(TX_CFG_ADR, (cPow  & 0x07) + BITs(3)   + BITs(5));
}
U8 RfGetPocket(U8* buff,U8* pcRSSI)
{
	for(U8 i=0; i<m_cRfRecvCount; i++)
	{
		buff[i] = m_csRfRecvbuff[i];
	}
        *pcRSSI = m_cRSSI;
	return m_cRfRecvCount;
}
void RfTest()
{
	U8 buff[36];
	U8 i;
	for (i = 0; i < 36; i++)
	{
		buff[i] = SpiReadByte(i);
	}
        delay(buff[30]);
}



