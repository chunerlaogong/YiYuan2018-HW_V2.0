#ifndef protocol_h 
#define protocol_h 1
#include "struct.h"

#ifndef TRUE
#define TRUE    0x01
#endif

#ifndef FALSE
#define FALSE   0x00
#endif

#define  StyleOfFrame      0x00 
//����֡
#define  AckOfFrame        0x00
//Ӧ��
#define  Coordinate        0x10
//��վ��
#define  LoadPrg           0x20 
//�м���
#define  ReaderOfFrame     0x18 
//��д��
#define  CallerOfFrame     0x20 
//������
#define  PositOfFrame      0x28 
//��λ��
#define  TgaeOfFrame       0x00 
//��ǩ
#define  ReserveOfFrame    0x38 
//Ԥ��

#define  YueduOfFrame     0x08
/***********************************************/

#define  StyleOfFrameData      0x40
//����֡

//����֡
#define  AckOfFrame        0x00 
//Ӧ��
#define  StationOfFrame    0x08 
//��վ��
#define  RelayOfFrame      0x10
//�м���
#define  ReaderOfFrame     0x18
//��д��
#define  LoadPrg           0x20 
//������
#define  PositOfFrame      0x28 
//��λ��
#define  TgaeOfFrame       0x00 
//��ǩ
#define  ReserveOfFrame    0x38
//Ԥ��


#define  StyleOfFrameRsp        0xc0
//��λ֡
#define  BDPlaceAskOfFramePlace   0x0000 
//�㲥��λ����
#define  NDPlaceAskOfFramePlace   0x0800 
//�㲥��λ����
#define  PlaceEndOfFramePlace     0x1000 
//��λ��ֹ
#define  BDpacksendOfFramePlace   0x1800 
//�㲥��λ������
#define  NDpacksendOfFramePlace   0x2000 
//�㲥��λ������
#define  AckOfFramePlace          0x2800 
//��λӦ��
#define  AlarmOfFramePlace      0x3000 
//����
#define  ReserveOfFramePlace    0x3800 
//Ԥ��

/************************************************/

#define  SubStyleOfFrame    0x3800
#define  CastSetplace  		0x00
#define  NodeSetplace  		0x01
#define  DataCard0			0x02
#define  DataCard1			0x02
#define  AttrOfRead         0x00
#define  AttrOfWrite        0x01
#define  BatteryVolt        0x07

typedef struct Frame_ctrlArea		//֡����5byte
{
	unsigned int   UserCode;		//
	unsigned char  Frame_ctrl;	//
	unsigned char  FrameLens;			//
	unsigned char  Pas_mima;			//���ܿ���1byte
	unsigned char  text[100];			//
	//֡��(��233 byte)=֡����+������+��������
}*Pframe_ctrlArea, Sframe_ctrlArea;

typedef struct Frame_datArea	
{
	unsigned int   UserCode;		
	unsigned char  Frame_ctrl;
	unsigned char   FrameLens;
	unsigned char  Pas_mima;
	unsigned char  text[100];
}*Pframe_DataArea, Sframe_DataArea;

extern void Calculate_Crc16(unsigned char * aData, unsigned char aSize );
extern unsigned short CRC_16( unsigned char *aData, unsigned char aSize );

extern Sontroler_Symple  TagCng_symple;

void makehead(Pcontroler_Symple TagCng);
void mov_data(unsigned char *source,unsigned char *dist,unsigned char lenth);

void inToSleep(Pcontroler_Symple TagCng);
unsigned char SendAlarmPackage(Pcontroler_Symple TagCng,unsigned char RequestAckFlag);	//���ͱ�����

void MemSet(unsigned char *pSrc,unsigned char len,unsigned char value);

extern unsigned char ReadByteFlash(unsigned long startaddr);
void mov_data(unsigned char *source,unsigned char *dist,unsigned char lenth);
extern void  AlarmJude(Pcontroler_Symple TagCng);

extern void inToSleep(Pcontroler_Symple TagCng);
extern void ReadParamFrmFlash(Pcontroler_Symple TagCng);

extern void EMDealGeomagneticValue_VectorDifference(void);//����ش�����

extern unsigned long Getdataa(Pcontroler_Symple TagCng,unsigned char repeat);

extern void EMDealGeomagneticValue_VectorDifference(void);//����ش�����

unsigned char SendCarStatus(Pcontroler_Symple TagCng,unsigned char CarStatus);

#endif