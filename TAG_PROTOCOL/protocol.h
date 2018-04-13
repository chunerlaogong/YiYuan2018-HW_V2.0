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
//控制帧
#define  AckOfFrame        0x00
//应答
#define  Coordinate        0x10
//基站器
#define  LoadPrg           0x20 
//中继器
#define  ReaderOfFrame     0x18 
//读写器
#define  CallerOfFrame     0x20 
//唤醒器
#define  PositOfFrame      0x28 
//定位器
#define  TgaeOfFrame       0x00 
//标签
#define  ReserveOfFrame    0x38 
//预留

#define  YueduOfFrame     0x08
/***********************************************/

#define  StyleOfFrameData      0x40
//数据帧

//控制帧
#define  AckOfFrame        0x00 
//应答
#define  StationOfFrame    0x08 
//基站器
#define  RelayOfFrame      0x10
//中继器
#define  ReaderOfFrame     0x18
//读写器
#define  LoadPrg           0x20 
//唤醒器
#define  PositOfFrame      0x28 
//定位器
#define  TgaeOfFrame       0x00 
//标签
#define  ReserveOfFrame    0x38
//预留


#define  StyleOfFrameRsp        0xc0
//定位帧
#define  BDPlaceAskOfFramePlace   0x0000 
//广播定位请求
#define  NDPlaceAskOfFramePlace   0x0800 
//点播定位请求
#define  PlaceEndOfFramePlace     0x1000 
//定位终止
#define  BDpacksendOfFramePlace   0x1800 
//广播定位包发送
#define  NDpacksendOfFramePlace   0x2000 
//点播定位包发送
#define  AckOfFramePlace          0x2800 
//定位应答
#define  AlarmOfFramePlace      0x3000 
//报警
#define  ReserveOfFramePlace    0x3800 
//预留

/************************************************/

#define  SubStyleOfFrame    0x3800
#define  CastSetplace  		0x00
#define  NodeSetplace  		0x01
#define  DataCard0			0x02
#define  DataCard1			0x02
#define  AttrOfRead         0x00
#define  AttrOfWrite        0x01
#define  BatteryVolt        0x07

typedef struct Frame_ctrlArea		//帧控制5byte
{
	unsigned int   UserCode;		//
	unsigned char  Frame_ctrl;	//
	unsigned char  FrameLens;			//
	unsigned char  Pas_mima;			//加密控制1byte
	unsigned char  text[100];			//
	//帧体(≤233 byte)=帧属性+命令码+后续参数
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
unsigned char SendAlarmPackage(Pcontroler_Symple TagCng,unsigned char RequestAckFlag);	//发送报警包

void MemSet(unsigned char *pSrc,unsigned char len,unsigned char value);

extern unsigned char ReadByteFlash(unsigned long startaddr);
void mov_data(unsigned char *source,unsigned char *dist,unsigned char lenth);
extern void  AlarmJude(Pcontroler_Symple TagCng);

extern void inToSleep(Pcontroler_Symple TagCng);
extern void ReadParamFrmFlash(Pcontroler_Symple TagCng);

extern void EMDealGeomagneticValue_VectorDifference(void);//处理地磁数据

extern unsigned long Getdataa(Pcontroler_Symple TagCng,unsigned char repeat);

extern void EMDealGeomagneticValue_VectorDifference(void);//处理地磁数据

unsigned char SendCarStatus(Pcontroler_Symple TagCng,unsigned char CarStatus);

#endif