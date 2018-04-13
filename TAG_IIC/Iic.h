#ifndef _IIC__H__
#define _IIC__H__
#include "Common.h"
#include  "McuTarger.h"

#define IIC_READ 1
#define IIC_WRITE 0

void IicInit();
U8 Iic8BitRead(U8 ASK);//I2C��8λ����
U8 Iic8BitWrite(U8 Temp);//I2Cд8λ����
void IicStart();//��SCLΪ�ߵ�ƽ��ʱ��SDA��һ���½��أ���ʾI2C��ʼ
void IicStop();//��SCLΪ�ߵ�ƽ��ʱ��SDA��һ�������أ���ʾI2C��������
void IicWrite(U8 cAddr,U8 val);
U8 IicRead(U8 cAddr);
void IicReadBytes(U8 cAddr,U8 cLen, U8* csOutBuff);
void IicWriteBytes(U8 cAddr,U8 cLen, U8* csInBuff);

#endif

