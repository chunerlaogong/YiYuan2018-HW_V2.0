#ifndef _R3100__H__
#define _R3100__H__
#include "Common.h"
#include "struct.h"

U8 Send_R3100ToRW_Test(void);

U8 SetMagBase();//���õشű���

void EMDealGeomagneticValue_VectorDifference(void);//����ش�����
void Gather_DataOfR3100();//ȡ�ش�����
void judge_changeOfmodule(Pcontroler_Symple TagCng);//����״̬��ת�ж�
extern unsigned char SendCarStatus(Pcontroler_Symple TagCng,unsigned char CarStatus);
unsigned long Getdataa(Pcontroler_Symple TagCng,unsigned char repeat); 
void GetMag(U8* nsMag,U8 cTemp);
unsigned char SetMagBase();
#endif


