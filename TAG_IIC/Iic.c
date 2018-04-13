#include "Iic.h"
#include "Type.h"

#define SLAVE_ADDRESS 0x40

void IicNop()
{
	nop();nop();nop();nop();nop();
	nop();nop();nop();nop();nop();
	nop();nop();nop();nop();nop();
	nop();nop();nop();nop();nop();
}

void IicInit()//I2C初始化
{
	IO_IIC_INIT();//IO_IIC_INIT()	PC_ODR_ODR0 = PC_ODR_ODR1 = 0
	SCL_SET();// #define SCL_CLR()	PC_DDR_DDR1 = 1
	SDA_SET();//#define SCL_SET()	PC_DDR_DDR1 = 0
}

U8 Iic8BitRead(U8 ASK)//I2C读8位数据
{
	U8 i;
	U8 Temp;
	for(i=0;i<8;i++)
	{
		SCL_SET();
		Temp=Temp<<1;
		IicNop();
		if(SDA_READ())  
			Temp++;
		SCL_CLR();
		IicNop();		
	}
	if(ASK)
		SDA_CLR();
	else
		SDA_SET();
	IicNop();
	SCL_SET();
	IicNop();
	SCL_CLR();
	IicNop();
	SDA_SET();
	IicNop();
	return Temp;
}

//===========================================================================

U8 Iic8BitWrite(U8 Temp)//I2C写8位数据
{
	U8 i;
	for(i=0;i<8;i++)
	{
		if(Temp & 0x80)
			SDA_SET();
		else
			SDA_CLR();
		Temp=Temp<<1;
		SCL_SET();//SCL上升沿读取数据
		IicNop();
		SCL_CLR();
	}
	SDA_SET();
	IicNop();
	SCL_SET();//SCL下将沿
	IicNop();
	SCL_CLR();
	IicNop();
	
	return SDA_READ();//读应答位（应答为低，非应答为高）
}
//===========================================================================
void IicStart()//在SCL为高电平的时候，SDA来一个下将沿，表示I2C开始
{
	IO_IIC_INIT();
	SDA_SET();
	SCL_SET();
	IicNop();
	SDA_CLR();
	IicNop();
	SCL_CLR();
}
//=============================================================================

void IicStop()//在SCL为高电平的时候，SDA来一个上升沿，表示I2C送数结速
{
	SDA_CLR();
	IicNop();
	SCL_CLR();
	SCL_SET();
	IicNop();
	SDA_SET();
}


void IicWrite(U8 cAddr,U8 val)
{
	IicStart();
	Iic8BitWrite(SLAVE_ADDRESS + IIC_WRITE);
	Iic8BitWrite(cAddr);
	Iic8BitWrite(val);
	IicStop();
}
U8 IicRead(U8 cAddr)
{
	IicStart();
	Iic8BitWrite(SLAVE_ADDRESS + IIC_WRITE);
	Iic8BitWrite(cAddr);
	IicStart();
	Iic8BitWrite(SLAVE_ADDRESS + IIC_READ);
	U8 cc = Iic8BitRead(0);
	IicStop();
	return cc;
}

void IicReadBytes(U8 cAddr,U8 cLen, U8* csOutBuff)
{
	IicStart();
	Iic8BitWrite(SLAVE_ADDRESS + IIC_WRITE);
	Iic8BitWrite(cAddr);
	IicStart();
	Iic8BitWrite(SLAVE_ADDRESS + IIC_READ);
	for(U8 i=0; i<cLen; i++)
	{
		if(i < cLen - 1)
		{
			csOutBuff[i] = Iic8BitRead(1);
		}
		else
		{
			csOutBuff[i] = Iic8BitRead(0);
		}
	}
	IicStop();
}

void IicWriteBytes(U8 cAddr,U8 cLen, U8* csInBuff)
{
	IicStart();
	Iic8BitWrite(SLAVE_ADDRESS + IIC_WRITE);
	Iic8BitWrite(cAddr);
	for(U8 i=0; i<cLen; i++)
	{
		Iic8BitWrite(csInBuff[i]);
	}
	IicStop();
}