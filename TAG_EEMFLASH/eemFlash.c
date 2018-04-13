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

#define MASS_KEY1	0XAE		//д������Կ,ע��FLASH��EEPROM����Կ�෴
#define MASS_KEY2	0X56
#define TAG_PARAM_START_ADDRESS		(0x50)
#define EEPROM_START_ADDRESS 		(0x1000)
#define BOOTLOADER_START_ADDRESS	(0x8000)
#define FLASH_START_ADDRESS			(0xD000)

void SysParaInt();//ϵͳ������ʼ��

unsigned int UserCode;//�ͻ���
unsigned int TagID;    // ID,3�ֽڣ����16777215��

extern SSensorbottom DynamicBottom;//��̬����

extern uint32_t SendPackCount;//,SendPackCount_backup;//�������������������ڿ��Ʒ�����ʱ����
sParamOfNBTag TagParam;

/************************************************************************************************
Function: void ReadParamFrmFlash(Pcontroler_Symple TagCng)
Description: ��ʼ��������д��E2Rom
note��
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
	//����Ĭ��id����Ϊ�㲥��ַ��������Ϊ��ʼ����־id
	if(TagCng->Config.TagNode.Tag_id_nub[0] ==0 && TagCng->Config.TagNode.Tag_id_nub[1]==0 && TagCng->Config.TagNode.Tag_id_nub[2] ==0) 
	{
		TagCng->Config.TagNode.Tag_id_nub[0] =TagID&0XFF;	//����Ĭ�ϱ�ǩID
		TagCng->Config.TagNode.Tag_id_nub[1] =(TagID>>8)&0xff;                                                         
		TagCng->Config.TagNode.Tag_id_nub[2] =(TagID>>16)&0xff; 
	}
}

void SysParaInt(Pcontroler_Symple TagCng)//ϵͳ������ʼ��
{
  ReadParamFrmFlash(TagCng);
  //����ǰ��ȡ�ĺϷ�����ֵ��Ϊ��̬����ֵ��ʼֵ
  DynamicBottom.EMData_xBottom=TagCng_symple.Config.TagPara.EMData_xBottom;
  DynamicBottom.EMData_yBottom=TagCng_symple.Config.TagPara.EMData_yBottom;
  DynamicBottom.EMData_zBottom=TagCng_symple.Config.TagPara.EMData_zBottom;
}

/************************************************************************************************
Function: void testRomOperate(Pcontroler_Symple TagCng) 
Description: ��ʼ��������д��E2Rom
note��
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
	SysParaInt(TagCng);							//ϵͳ������ʼ��
	//  TagCng->Config.TagNode.LEDFlag=1;		//led����״̬
	if (TagCng->Config.TagPara.ParaIntFlag==0)	//������ʼ����־
	{
		TagCng->Config.ini_sign[0] =0;			//�ƻ�flash����������־��ǿ�Ƴ�ʼ��
	} 
	TagCng->Config.ini_sign[0] =0;				//�ƻ�flash����������־��ǿ�Ƴ�ʼ��
	//ʹ�ð汾������Ϊeep�����Ϸ���֤
	if   ((TagCng->Config.ini_sign[0] ==(U8)SYSVER)
		  &&(TagCng->Config.ini_sign[1] ==(U8)(SYSVER*SYSVER))
			  &&(TagCng->Config.ini_sign[2] ==(U8)SYSVER))
	{
		nop();
	}
	else           //���flash����������־�Ƿ������ʼ���������ã����Ҹ���flash�е�����
	{
		TagCng->Config.Tag_status = LEAVE_FACTORY;//ACTIVATE;//LEAVE_FACTORY;//
		TagCng->Config.Tag_initNB = LEAVE_FACTORY;
		TagCng->Config.TagPara.ParaIntFlag=1;//������ʼ����־
		
		TagCng->Config.TagNode.Tag_id_nub[0] =TagID&0XFF;	//������ǩID
		TagCng->Config.TagNode.Tag_id_nub[1] =(TagID>>8)&0xff;                                                         
		TagCng->Config.TagNode.Tag_id_nub[2] =(TagID>>16)&0xff; 
		
		TagCng->Config.TagNode.UserCode = UserCode;  	//�ͻ���                                                                                                                                                                                                                                                            
		
		TagCng->Config.TagPara.AlarmValid = 0x4C00;		//��Ч��־,��10λ�г�����11λ�޳�����14λƵ�����ٻ��ѱ���   
		TagCng->Config.TagNode.LEDFlag=0;				//led����״̬
		TagCng->Config.TagPara.GetEMBottom_RFFlag=0x01;	//0x01Ϊ�򿪵شż�⣬
		
		TagCng->Config.TagPara.GetEMBottom_RFModThreshold=10;	//�ų�ǿ��������
		TagCng->Config.TagPara.GetEMBottom_RFAngleThreshold=10;	//�ų�ƫת�Ƕ�������,10��
		
		TagCng->Config.TagPara.FastRouse=0;  					//���ٻ��ѱ�־    
		TagCng->Config.TagPara.FastRouseAlarm=0;  				//��ο��ٻ��ѱ�����־
		TagCng->Config.TagPara.WdtInterval = 5;					//WDT�̶�˯��ʱ�� unit:s
		TagCng->Config.TagPara.HeartbeatInterval= 1; 			//����������� unit:min
		
		//ʹ�ð汾������Ϊeep�����Ϸ���֤
		TagCng->Config.ini_sign[0]	   =(U8)SYSVER; 	//flash��ʼ����־
		TagCng->Config.ini_sign[1]	   =(U8)(SYSVER*SYSVER);
		TagCng->Config.ini_sign[2]	   =(U8)SYSVER;
//		writeFlashParameter((PcontrolerConfig)TagCng);//д���ݵ�eep������Ĭ�ϲ���
		
		SysParaInt(TagCng);//ϵͳ������ʼ��
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
		TagCng->Config.TagPara.AlarmValid = 0x4C00;		//��Ч��־,��10λ�г�����11λ�޳�����14λƵ�����ٻ��ѱ���
		TagCng->Config.TagNode.LEDFlag = 0;				//led����״̬
		TagCng->Config.TagPara.GetEMBottom_RFFlag = 0x01;	//0x01Ϊ�򿪵شż�⣬
		TagCng->Config.TagPara.GetEMBottom_RFModThreshold = 10;	//�ų�ǿ��������
		TagCng->Config.TagPara.GetEMBottom_RFAngleThreshold = 10;	//�ų�ƫת�Ƕ�������,10��
		TagCng->Config.TagPara.FastRouse = 0;  					//���ٻ��ѱ�־    
		TagCng->Config.TagPara.FastRouseAlarm = 0;  				//��ο��ٻ��ѱ�����־
		TagCng->Config.TagPara.WdtInterval = 5;					//WDT�̶�˯��ʱ�� unit:s
		TagCng->Config.TagPara.HeartbeatInterval = 5; 			//����������� unit:min Ĭ��5����
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
	TagParam.UserCode = TagCng->Config.TagNode.UserCode;	//����ͻ���
	TagParam.TagID[0] = TagCng->Config.TagNode.Tag_id_nub[0];	//����ID
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