#ifndef _IIC__H__
#define _IIC__H__
#include "Common.h"
#include  "McuTarger.h"

#define IIC_READ 1
#define IIC_WRITE 0

void IicInit();
U8 Iic8BitRead(U8 ASK);//I2C读8位数据
U8 Iic8BitWrite(U8 Temp);//I2C写8位数据
void IicStart();//在SCL为高电平的时候，SDA来一个下将沿，表示I2C开始
void IicStop();//在SCL为高电平的时候，SDA来一个上升沿，表示I2C送数结速
void IicWrite(U8 cAddr,U8 val);
U8 IicRead(U8 cAddr);
void IicReadBytes(U8 cAddr,U8 cLen, U8* csOutBuff);
void IicWriteBytes(U8 cAddr,U8 cLen, U8* csInBuff);

#endif

