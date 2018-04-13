#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "stm8l15x_conf.h"
#include "eemFlash.h"
#include "Common.h"
#include "Cyrf6936.h"
#include "Type.h"
#include  "McuTarger.h"
#include "struct.h"
#include "compilefiles.h"
#include "MyCommon.h"

#define MASS_KEY1	0XAE		//写操作密钥,注意FLASH与EEPROM的密钥相反
#define MASS_KEY2	0X56
#define TAG_PARAM_START_ADDRESS		(0x50)
#define EEPROM_START_ADDRESS 		(0x1000)
#define BOOTLOADER_START_ADDRESS	(0x8000)
#define FLASH_START_ADDRESS			(0xD000)

void SysParaInt();//系统参数初始化

unsigned int UserCode;//客户码
unsigned int TagID;    // ID,3字节，最大16777215，

extern SSensorbottom DynamicBottom;//动态本底

extern uint32_t SendPackCount;//,SendPackCount_backup;//发报警包控制数，用于控制发包的时间间隔
sParamOfNBTag TagParam;

/************************************************************************************************
Function: void ReadParamFrmFlash(Pcontroler_Symple TagCng)
Description: 初始化参数并写入E2Rom
note：
Calls:          
Called By:      
Table Accessed: 
Table Updated:  
Input:               
Output:         
Return:       
Timers:  2014.02.25    
author:  ME 
************************************************************************************************/

void ReadParamFrmFlash(Pcontroler_Symple TagCng)
{
	//    ReadFlash2Buff((unsigned char *)TagCng,207,48); 
	ReadFlash2Buff((unsigned char *)TagCng,45,48); 
	SendPackCount = (TagCng_symple.Config.TagPara.HeartbeatInterval * 60)/TagCng_symple.Config.TagPara.WdtInterval;
	//  SendPackCount_backup = SendPackCount;
	//本机默认id不可为广播地址，否则置为初始化标志id
	if(TagCng->Config.TagNode.Tag_id_nub[0] ==0 && TagCng->Config.TagNode.Tag_id_nub[1]==0 && TagCng->Config.TagNode.Tag_id_nub[2] ==0) 
	{
		TagCng->Config.TagNode.Tag_id_nub[0] =TagID&0XFF;	//本机默认标签ID
		TagCng->Config.TagNode.Tag_id_nub[1] =(TagID>>8)&0xff;                                                         
		TagCng->Config.TagNode.Tag_id_nub[2] =(TagID>>16)&0xff; 
	}
}

void SysParaInt(Pcontroler_Symple TagCng)//系统参数初始化
{
  ReadParamFrmFlash(TagCng);
  //将当前获取的合法本底值作为动态本底值初始值
  DynamicBottom.EMData_xBottom=TagCng_symple.Config.TagPara.EMData_xBottom;
  DynamicBottom.EMData_yBottom=TagCng_symple.Config.TagPara.EMData_yBottom;
  DynamicBottom.EMData_zBottom=TagCng_symple.Config.TagPara.EMData_zBottom;
}

/************************************************************************************************
Function: void testRomOperate(Pcontroler_Symple TagCng) 
Description: 初始化参数并写入E2Rom
note：
Calls:          
Called By:      
Table Accessed: 
Table Updated:  
Input:               
Output:         
Return:       
Timers:  2014.02.25    
author:  ME 
************************************************************************************************/
void testRomOperate(Pcontroler_Symple TagCng)
{
	SysParaInt(TagCng);							//系统参数初始化
	//  TagCng->Config.TagNode.LEDFlag=1;		//led开启状态
	if (TagCng->Config.TagPara.ParaIntFlag==0)	//参数初始化标志
	{
		TagCng->Config.ini_sign[0] =0;			//破坏flash参数完整标志，强制初始化
	} 
	TagCng->Config.ini_sign[0] =0;				//破坏flash参数完整标志，强制初始化
	//使用版本号来作为eep参数合法验证
	if   ((TagCng->Config.ini_sign[0] ==(U8)SYSVER)
		  &&(TagCng->Config.ini_sign[1] ==(U8)(SYSVER*SYSVER))
			  &&(TagCng->Config.ini_sign[2] ==(U8)SYSVER))
	{
		nop();
	}
	else           //如果flash参数完整标志非法，则初始化本机设置，并且更新flash中的设置
	{
		TagCng->Config.Tag_status = LEAVE_FACTORY;//ACTIVATE;//LEAVE_FACTORY;//
		TagCng->Config.Tag_initNB = LEAVE_FACTORY;
		TagCng->Config.TagPara.ParaIntFlag=1;//参数初始化标志
		
		TagCng->Config.TagNode.Tag_id_nub[0] =TagID&0XFF;	//本机标签ID
		TagCng->Config.TagNode.Tag_id_nub[1] =(TagID>>8)&0xff;                                                         
		TagCng->Config.TagNode.Tag_id_nub[2] =(TagID>>16)&0xff; 
		
		TagCng->Config.TagNode.UserCode = UserCode;  	//客户码                                                                                                                                                                                                                                                            
		
		TagCng->Config.TagPara.AlarmValid = 0x4C00;		//有效标志,第10位有车，第11位无车，第14位频繁快速唤醒报警   
		TagCng->Config.TagNode.LEDFlag=0;				//led开启状态
		TagCng->Config.TagPara.GetEMBottom_RFFlag=0x01;	//0x01为打开地磁检测，
		
		TagCng->Config.TagPara.GetEMBottom_RFModThreshold=10;	//磁场强度灵敏度
		TagCng->Config.TagPara.GetEMBottom_RFAngleThreshold=10;	//磁场偏转角度灵敏度,10度
		
		TagCng->Config.TagPara.FastRouse=0;  					//快速唤醒标志    
		TagCng->Config.TagPara.FastRouseAlarm=0;  				//多次快速唤醒报警标志
		TagCng->Config.TagPara.WdtInterval = 5;					//WDT固定睡眠时间 unit:s
		TagCng->Config.TagPara.HeartbeatInterval= 1; 			//发送心跳间隔 unit:min
		
		//使用版本号来作为eep参数合法验证
		TagCng->Config.ini_sign[0]	   =(U8)SYSVER; 	//flash初始化标志
		TagCng->Config.ini_sign[1]	   =(U8)(SYSVER*SYSVER);
		TagCng->Config.ini_sign[2]	   =(U8)SYSVER;
//		writeFlashParameter((PcontrolerConfig)TagCng);//写数据到eep，保留默认参数
		
		SysParaInt(TagCng);//系统参数初始化
	}
	nop();
}

/*
 *@Functionname:TagParamInit
 *@Author: HL
 *@Date:2018-01-23
 */
void TagParamInit(Pcontroler_Symple TagCng)
{
	DelayOneMs(200);
	ReadTagParam(TagCng);
	if(TagParam.ParaIntFlag[0] == 0x5A
	   && TagParam.ParaIntFlag[1] == 0xA5
		   && TagParam.ParaIntFlag[2] == 0x65)
	{
		nop();
	}
	else
	{
		TagCng->Config.Tag_status = 0x01;					//ACTIVATE;
		TagCng->Config.Tag_initNB = 0x01;
		TagCng->Config.TagNode.Tag_id_nub[0] = TagID&0XFF;			//TagID
		TagCng->Config.TagNode.Tag_id_nub[1] = (TagID>>8)&0xff;                                                         
		TagCng->Config.TagNode.Tag_id_nub[2] = (TagID>>16)&0xff;
		TagCng->Config.TagNode.UserCode = UserCode;  				//UserCode
		TagCng->Config.TagPara.AlarmValid = 0x4C00;		//有效标志,第10位有车，第11位无车，第14位频繁快速唤醒报警
		TagCng->Config.TagNode.LEDFlag = 0;				//led开启状态
		TagCng->Config.TagPara.GetEMBottom_RFFlag = 0x01;	//0x01为打开地磁检测，
		TagCng->Config.TagPara.GetEMBottom_RFModThreshold = 10;	//磁场强度灵敏度
		TagCng->Config.TagPara.GetEMBottom_RFAngleThreshold = 10;	//磁场偏转角度灵敏度,10度
		TagCng->Config.TagPara.FastRouse = 0;  					//快速唤醒标志    
		TagCng->Config.TagPara.FastRouseAlarm = 0;  				//多次快速唤醒报警标志
		TagCng->Config.TagPara.WdtInterval = 5;					//WDT固定睡眠时间 unit:s
		TagCng->Config.TagPara.HeartbeatInterval = 5; 			//发送心跳间隔 unit:min 默认5分钟
		TagParam.ParaIntFlag[0] = 0x5A;
		TagParam.ParaIntFlag[1] = 0xA5;
		TagParam.ParaIntFlag[2] = 0x65;
		uint16_t tmp[4] = {0};
		sscanf(DEFAULT_SERVER_ADDRESS, "%d.%d.%d.%d:%d", &tmp[0], &tmp[1], &tmp[2], &tmp[3], &TagParam.DefaultPort);
		TagParam.DefaultIPAddr[0] = tmp[0];TagParam.DefaultIPAddr[1] = tmp[1];
		TagParam.DefaultIPAddr[2] = tmp[2];TagParam.DefaultIPAddr[3] = tmp[3];
		WriteTagParam(TagCng);
		ReadTagParam(TagCng);
	}	
}
/*
 *@Functionname:WriteTagParam
 *@Author: HL
 *@Date:2018-01-23
 */
void WriteTagParam(Pcontroler_Symple TagCng)
{
	TagParam.Tag_status = TagCng->Config.Tag_status;
	TagParam.Tag_initNB = TagCng->Config.Tag_initNB;
	TagParam.UserCode = TagCng->Config.TagNode.UserCode;	//保存客户码
	TagParam.TagID[0] = TagCng->Config.TagNode.Tag_id_nub[0];	//保存ID
	TagParam.TagID[1] = TagCng->Config.TagNode.Tag_id_nub[1];
	TagParam.TagID[2] = TagCng->Config.TagNode.Tag_id_nub[2];
//	TagParam.UpdateFlag
//	TagParam.Version 
//	TagParam.ParaIntFlag
	TagParam.LEDFlag  = TagCng->Config.TagNode.LEDFlag;
	TagParam.WdtInterval = TagCng->Config.TagPara.WdtInterval;
	TagParam.HeartbeatInterval = TagCng->Config.TagPara.HeartbeatInterval;
	TagParam.AlarmValid = TagCng->Config.TagPara.AlarmValid;
	TagParam.GetEMBottom_RFFlag = TagCng->Config.TagPara.GetEMBottom_RFFlag;
	TagParam.GetEMBottom_RFModThreshold = TagCng->Config.TagPara.GetEMBottom_RFModThreshold;
	TagParam.GetEMBottom_RFAngleThreshold = TagCng->Config.TagPara.GetEMBottom_RFAngleThreshold;
	TagParam.EMData_xBottom = DynamicBottom.EMData_xBottom;
	TagParam.EMData_yBottom = DynamicBottom.EMData_yBottom;
	TagParam.EMData_zBottom = DynamicBottom.EMData_zBottom;
	TagParam.FastRouse = TagCng->Config.TagPara.FastRouse;
	TagParam.FastRouseAlarm = TagCng->Config.TagPara.FastRouseAlarm;
	WriteBuffToFlashEepRom(FLASH_MemType_Data, TAG_PARAM_START_ADDRESS, (uint8_t *)&TagParam, sizeof(TagParam));
}

/*
 *@Functionname:ReadTagParam
 *@Author: HL
 *@Date:2018-01-23
 */
void ReadTagParam(Pcontroler_Symple TagCng)
{
	ReadFlashEepRomToBuff(FLASH_MemType_Data, TAG_PARAM_START_ADDRESS, (uint8_t *)&TagParam, sizeof(TagParam));
	TagCng->Config.Tag_status = TagParam.Tag_status;
	TagCng->Config.Tag_initNB = TagParam.Tag_initNB;
	TagCng->Config.TagNode.UserCode = TagParam.UserCode;
	TagCng->Config.TagNode.Tag_id_nub[0] = TagParam.TagID[0];
	TagCng->Config.TagNode.Tag_id_nub[1] = TagParam.TagID[1];
	TagCng->Config.TagNode.Tag_id_nub[2] = TagParam.TagID[2];
	TagCng->Config.TagNode.LEDFlag = TagParam.LEDFlag;
	TagCng->Config.TagPara.WdtInterval = TagParam.WdtInterval;
	TagCng->Config.TagPara.HeartbeatInterval = TagParam.HeartbeatInterval;
	TagCng->Config.TagPara.AlarmValid = TagParam.AlarmValid;
	TagCng->Config.TagPara.GetEMBottom_RFFlag = TagParam.GetEMBottom_RFFlag;
	TagCng->Config.TagPara.GetEMBottom_RFModThreshold = TagParam.GetEMBottom_RFModThreshold;
	TagCng->Config.TagPara.GetEMBottom_RFAngleThreshold = TagParam.GetEMBottom_RFAngleThreshold;
	DynamicBottom.EMData_xBottom = TagParam.EMData_xBottom;
	DynamicBottom.EMData_yBottom = TagParam.EMData_yBottom;
	DynamicBottom.EMData_zBottom = TagParam.EMData_zBottom;
	TagCng->Config.TagPara.FastRouse = TagParam.FastRouse;
	TagCng->Config.TagPara.FastRouseAlarm = TagParam.FastRouseAlarm;
	
	SendPackCount = (TagCng_symple.Config.TagPara.HeartbeatInterval * 60)/TagCng_symple.Config.TagPara.WdtInterval;
}

/*
 *@FunctionName:WriteBuffToFlashEepRom
 *@Author: HL
 *@Date:2018-01-23
 */
void WriteBuffToFlashEepRom(FLASH_MemType_TypeDef FLASH_MemType, uint32_t cAddr, uint8_t *pFromBuff, uint32_t cLen)
{
	FLASH_Unlock(FLASH_MemType);
	if(FLASH_MemType == FLASH_MemType_Data)
	{
		while(RESET == FLASH_GetFlagStatus(FLASH_FLAG_DUL));	// unlock eeprom
		for(uint32_t i = 0; i < cLen; i++)
		{
			FLASH_ProgramByte(EEPROM_START_ADDRESS + cAddr + i, pFromBuff[i]);
		}
		
	}
	else
	{
		while(RESET == FLASH_GetFlagStatus(FLASH_FLAG_PUL));	// unlock flash
		for(uint32_t i = 0; i < cLen; i++)
		{
			FLASH_ProgramByte(FLASH_START_ADDRESS + cAddr + i, pFromBuff[i]);
		}
	}
	FLASH_Lock(FLASH_MemType);
}

/*
 *@FunctionName:ReadFlashEepRomToBuff
 *@Author: HL
 *@Date:2018-01-23
 */
void ReadFlashEepRomToBuff(FLASH_MemType_TypeDef FLASH_MemType, uint32_t cAddr, uint8_t *pToBuff, uint32_t cLen)
{
	if(FLASH_MemType == FLASH_MemType_Program)
	{
		for(uint32_t i = 0; i < cLen; i++)
		{
			pToBuff[i] = FLASH_ReadByte(FLASH_START_ADDRESS + cAddr + i);
		}
	}
	else
	{
		for(uint32_t i = 0; i < cLen; i++)
		{
			pToBuff[i] = FLASH_ReadByte(EEPROM_START_ADDRESS + cAddr + i);
		}
	}
}