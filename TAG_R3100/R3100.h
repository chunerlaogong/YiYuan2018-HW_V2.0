#ifndef _R3100__H__
#define _R3100__H__
#include "Common.h"
#include "struct.h"

U8 Send_R3100ToRW_Test(void);

U8 SetMagBase();//设置地磁本底

void EMDealGeomagneticValue_VectorDifference(void);//处理地磁数据
void Gather_DataOfR3100();//取地磁数据
void judge_changeOfmodule(Pcontroler_Symple TagCng);//车辆状态反转判断
extern unsigned char SendCarStatus(Pcontroler_Symple TagCng,unsigned char CarStatus);
unsigned long Getdataa(Pcontroler_Symple TagCng,unsigned char repeat); 
void GetMag(U8* nsMag,U8 cTemp);
unsigned char SetMagBase();
#endif


