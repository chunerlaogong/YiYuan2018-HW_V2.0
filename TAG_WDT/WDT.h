#ifndef __WDT_H
#define __WDT_H

//void test_Product(Pcontroler_Symple TagCng, pNB pMyNB);
void test_Product(Pcontroler_Symple TagCng, pNBiotAT pMyNBAT);
void dealServerData(pNB pMyNB, Pcontroler_Symple TagCng);
void  GetVcc();
void inToSleep(Pcontroler_Symple TagCng);
void OutFromSleep(Pcontroler_Symple TagCng);
void mov_data(unsigned char *source,unsigned char *dist,unsigned char lenth);
void  AlarmJude(Pcontroler_Symple TagCng);//�еشű䶯�ŷ�����

extern void judge_changeOfmodule(Pcontroler_Symple TagCng);//����״̬��ת�ж�

unsigned char SendCarStatus(Pcontroler_Symple TagCng,unsigned char CarStatus);

extern unsigned char SendAlarmPackage(Pcontroler_Symple TagCng,unsigned char RequestAckFlag);
uint8_t SendBerthStatusChange(Pcontroler_Symple TagCng,unsigned char CarStatus);

extern void VRAD_init(void);
extern void AD_init(void);
extern unsigned short VREF_Value(void);
extern unsigned short Battery_Read();

#endif



