#include <string.h>
#include <stdlib.h>
#include "struct.h"

#include "stm8l15x_conf.h"
#include "CRC.h"
#include "MyNBIOT.h"
#include "WDT.h"
#include "RTC_Driver.h"
#include "zigbee.h"
#include "Type.h"

#include "compilefiles.h"
#include "McuTarger.h"

#include "statisMagneticDensity.h"
#include "bottomvalAdjustAdaptive.h"
#include "eemFlash.h"
#include "protocol.h"
#include "Common.h"
extern Magnetic_density g_magnetic_density;		// 车辆磁干扰_有车状态下使用
extern Magnetic_density g_magnetic_base; 		// 无车磁场强度(地球磁场+环境磁场)
extern U8 ParkState;
extern U8 Send_R3100ToRW_Test(void);

extern void GetDefaultSleepTime(void);
extern void OpenTimer0(void);
extern void CloseTimer0(void);
extern void delayMs(long dls);

extern void ReadParamFrmFlash(Pcontroler_Symple TagCng);
extern U8 SetMagBase();							//设置地磁本底 

extern void Hex_to_ASCII(char *hex,char *ascii,unsigned char len);
extern sNB	NB_BC95;

extern SSensor3100  Sensor3100L;
extern SSensorbottom DynamicBottom;				//动态本底

U8 BottomFlag=0;								//输出本底标志，0--手动校准本底，1--动态本底
extern signed int RealTimeValue_x,RealTimeValue_y,RealTimeValue_z;
unsigned char Do_Writer_device(Pcontroler_Symple TagCng);                     //GJL 20170720
extern sNBiotAT NBAtCommand;
extern sParamOfNBTag TagParam;
void eeprom_init()
{
	FLASH->CR1 &= (unsigned char)(~0x01);
	FLASH->CR1 |= (unsigned char)0x00;
	FLASH->DUKR = 0xAE;
	FLASH->DUKR = 0x56;
}
extern volatile sNB_CMD MyNBIOT_CMD_data;
unsigned char SendAlarmPackage(Pcontroler_Symple TagCng,unsigned char RequestAckFlag)	//发送报警包
{
	Pframe_DataArea   RfSendPackage;
	unsigned char lens,test=0;
	
	RfSendPackage = (Pframe_DataArea)&TagCng->txReci[0];
	makehead(TagCng);
	//**********泊位宝直接发数据给服务器，此处改为组读写器的包，与原来发给读写器组包有所不同 
	RfSendPackage->UserCode = (TagCng->Config.TagNode.UserCode<<8)+(TagCng->Config.TagNode.UserCode>>8);
	RfSendPackage->Frame_ctrl = StyleOfFrameData|ReserveOfFrame;//现在为0x78;原来为 StyleOfFrameData|TgaeOfFrame
	RfSendPackage->FrameLens  =0x13;    //原来0x07;
	RfSendPackage->text[6] = 0x98;   	//两个固定位
	RfSendPackage->text[7] = 0x3E;
	
	RfSendPackage->text[8] = TagCng->Config.TagPara.AlarmValid;//有效状态
	RfSendPackage->text[9] = TagCng->Config.TagPara.AlarmValid>>8;//有效状态
	RfSendPackage->text[10] = TagCng->Config.TagPara.AlarmStatus;//车状态
	RfSendPackage->text[11] = TagCng->Config.TagPara.AlarmStatus>>8;//车状态
	
	RfSendPackage->text[12] =((U8)(Sensor3100L.EMData_x));//x实时值
	RfSendPackage->text[15] =((U8)(Sensor3100L.EMData_y));//y实时值
	RfSendPackage->text[16] =((U8)(Sensor3100L.EMData_z));//z实时值
	RfSendPackage->text[13] =((U8)((Sensor3100L.diffOfRM)));//模差
	switch (RfSendPackage->text[10] >>6)
	{
	  case 0:
		{     
			BottomFlag++;
			BottomFlag=BottomFlag %2;
			RfSendPackage->text[14] =BottomFlag;
			break;
		}
	  case 1:
		{
			if (BottomFlag==0)
				RfSendPackage->text[14] =((U8)(TagCng_symple.Config.TagPara.EMData_xBottom));//y固定本底值
			else
				RfSendPackage->text[14] =((U8)(DynamicBottom.EMData_xBottom));//y动态本底值
			break;
		}
	  case 2:
		{
			if (BottomFlag==0)
				RfSendPackage->text[14] =((U8)(TagCng_symple.Config.TagPara.EMData_yBottom));//y固定本底值
			else
				RfSendPackage->text[14] =((U8)(DynamicBottom.EMData_yBottom));//y动态本底值
			break;
		}
	  case 3:
		{
			if (BottomFlag==0)
				RfSendPackage->text[14] =((U8)(TagCng_symple.Config.TagPara.EMData_zBottom));//y固定本底值
			else
				RfSendPackage->text[14] =((U8)(DynamicBottom.EMData_zBottom));//y动态本底值
			break;
		}
	}
//	if(getCSQ((pNB)&NB_BC95) == SUCCESS)			// 获取信号强度
//	{
//		RfSendPackage->text[17] = (NB_BC95.rssi & 0x1F);
//	}
	RfSendPackage->text[18] = 0x3C;   	//地磁数据第二位CRC,此处暂时固定
	RfSendPackage->text[19] = 0x00;   	//保留3个字节
	RfSendPackage->text[20] = 0x00;
	RfSendPackage->text[21] = 0x00;
	RfSendPackage->text[22] = TagCng->Config.TagNode.Tag_id_nub[0];//3个字节的读写器ID,此处为泊位宝自身ID
	RfSendPackage->text[23] = TagCng->Config.TagNode.Tag_id_nub[1];
	RfSendPackage->text[24] = TagCng->Config.TagNode.Tag_id_nub[2];
	RfSendPackage->text[25] = 0x00;		//保留
	RfSendPackage->text[26] = TagCng->Config.TagNode.Tag_id_nub[0];//数据来源(地磁ID),此处为泊位宝自身ID
	RfSendPackage->text[27] = TagCng->Config.TagNode.Tag_id_nub[1];
	RfSendPackage->text[28] = TagCng->Config.TagNode.Tag_id_nub[2]; 

	lens = (RfSendPackage->FrameLens)+15;
	Calculate_Crc16((unsigned char *)RfSendPackage,lens);
	LED2_set;
	
	char txbuf_temp[80] = {0};
	Hex_to_ASCII((char *)TagCng->txReci, txbuf_temp, lens+2);
	#if COAP_TEST
	if(SUCCESS == DoNBiotATCmd((pNBiotAT)&NBAtCommand, 0, JUDGE_NONE, "", "AT+NMGS=%d,%s\r\n", lens+2, txbuf_temp))
	{
		LED2_clr;
		printf("COAP:send data successful ...\n");
	}
	#else
	printf("start to send NB-UDP data\n");
	if(SUCCESS == DoNBiotATCmd((pNBiotAT)&NBAtCommand, 0, JUDGE_NONE, "", "AT+NSOST=%d,%d.%d.%d.%d,%d,%d,%s\r\n", 0, TagParam.DefaultIPAddr[0],
			TagParam.DefaultIPAddr[1], TagParam.DefaultIPAddr[2], TagParam.DefaultIPAddr[3], TagParam.DefaultPort, lens+2, txbuf_temp))
	{
		LED2_clr;
		printf("UDP:send data successful ...\n");
	}
	#endif
	return test;
}

void makehead(Pcontroler_Symple TagCng)
{
	Pframe_DataArea  Package;
	Package = (Pframe_DataArea)&TagCng->txReci[0];
	Package->Pas_mima=  0x00;    
	Package->text[0] = 0;
	Package->text[1] = 0;
	Package->text[2] = 0;
	Package->text[3] =  TagCng->Config.TagNode.Tag_id_nub[0];
	Package->text[4]=   TagCng->Config.TagNode.Tag_id_nub[1];
	Package->text[5]=   TagCng->Config.TagNode.Tag_id_nub[2];
}
