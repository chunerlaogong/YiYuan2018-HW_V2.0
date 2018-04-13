/*
 * @file: 	Bluetooth.c
 * @author:	HL
 * @date:	2017-11-23
 * @version:	V1.0
 */

/* includes */
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "stm8l15x_conf.h"
#include "struct.h"
#include "McuTarger.h"
#include "Common.h"
#include "CRC.h"
#include "UART.h"
#include "MyCommon.h"
#include "R3100.h"
#include "MyNBIOT.h"
#include "eemFlash.h"
#include "parkStateJudge.h"
#include "Bluetooth.h"

#define TAG_PARAM_START_ADDRESS		(0x50)
#define EEPROM_START_ADDRESS 		(0x1000)
#define BOOTLOADER_START_ADDRESS	(0x8000)
#define FLASH_START_ADDRESS			(0xD000)

/* defines */

/* private variables */

/* extern */
extern SSensorbottom DynamicBottom;                // 动态本底
extern Sontroler_Symple TagCng_symple;
extern SSensor3100  Sensor3100L;
extern volatile sNB_CMD MyNBIOT_CMD_data[30];
extern sNBiotAT NBAtCommand;
extern sParamOfNBTag TagParam;
extern Sontroler_Symple TagCng_symple;
extern uint32_t TIM4_Count;
extern uint32_t SendNodeCount;
extern uint32_t SendPackCount;
extern U8 ParkState;
extern Sontroler_Symple TagCng_symple;
extern char ATCmdFromBTData[30];
/* BT cmd set */
sBTCMD MyBT_CMD_Data[2] = {
	//指令内容				最大尝试次数    超时时间	正确返回值				是否返回		初始状态	返回内容
	{"AT+B GLDN\r",		3,				1,			"\r",				ENABLE,			NO_BACK,		""},
};
	
/* global variable */

/* private functions declaration */

/* functions */

/*
 *@name:	send_data_to_BT
 *@author:	HL
 *@date:	2017-11-30
 *@brief:	通过串口1，利用AT指令判断是否OK
 *@return:	成功或者错误，如果错误，错误类型在pMyCMD->BT_Return
 */
RETURN_STATUS send_data_to_BT(pBT MyBT, uint8_t CMD_NUM, char *fmt,...){
	uint8_t len = 0;
	memset(MyBT->BT_TX_BUFF, 0, BT_BUFF_SIZE);
//	memset(MyBT->BT_TX_BUFF_TMP, 0, BT_BUFF_SIZE);
	char *ptr = MyBT->BT_TX_BUFF;
	enableUsartBT();
	va_list ap;
	va_start(ap,fmt);
	len = (uint8_t)vsprintf(ptr,fmt,ap);
	va_end(ap);
	
	MyBT->BT_TX_CNT = len;
	MyBT_CMD_Data[CMD_NUM].BT_Return = NO_BACK;
	
	memset(MyBT->BT_RX_BUF_TMP, 0, BT_BUFF_SIZE);
	memset(MyBT->BT_RX_BUF, 0, BT_BUFF_SIZE);
	
	MyBT->current_cmd_num = CMD_NUM;
	MyBT->current_cmd_time = MyBT_CMD_Data[CMD_NUM].timeout * 1000;
	
	MyBT->BT_RX_CNT = 0;
	sendUsartData(USART_FOR_BT, MyBT->BT_TX_BUFF, len);
	while(MyBT_CMD_Data[CMD_NUM].BT_Return == NO_BACK){
		DelayOneMs(1);
		MyBT->current_cmd_time--;
		if(!MyBT->current_cmd_time)
		{
			MyBT_CMD_Data[CMD_NUM].BT_Return = BACK_TIMEOUT;
		}
	}
	return MyBT_CMD_Data[CMD_NUM].BT_Return;
}

/*
 *@name:	doBT_CMD
 *@author:	HL
 *@date:	2017-12-01
 *@brief:	通过串口1，利用AT指令判断是否OK
 *@return:	成功或者错误，如果错误，错误类型在pMyCMD->NB_Return
 */
ErrorStatus doBT_CMD(pBT MyBT, uint8_t CMD_NUM){
	uint16_t doCMD_cnt = 0;
	while((doCMD_cnt < MyBT_CMD_Data[CMD_NUM].MaxTry)){
		send_data_to_BT(MyBT, CMD_NUM, MyBT_CMD_Data[CMD_NUM].pCMD_Data);
		if(MyBT_CMD_Data[CMD_NUM].BT_Return == BACK_OK){
			return SUCCESS;
		}
		doCMD_cnt++;
	}
	return ERROR;
}

/*
 *@name:	deal_BT_data
 *@author:	HL
 *@date:	2018-01-18
 *@brief:	处理指令
 *@return:	
 */
ErrorStatus deal_BT_data(pBT MyBT)
{
        
	if(MyBT->BT_rx_mode != DATA_MODE)
	{
		return ERROR;
	}
	if(MyBT->BT_RX_FLAG != 0x01)
	{
		return ERROR;
	}
	MyBT->BT_RX_FLAG = 0x00;
	pBT_COMMAND pMy_BT_command;
	pMy_BT_command = (pBT_COMMAND)&MyBT->BT_RX_BUF[0];
	if(pMy_BT_command->fram_prefix0 != 0xFF || pMy_BT_command->fram_prefix1 != 0x66)
	{
		return ERROR;	// 帧头错误
	}
	uint16_t local_crc = 0, remote_crc = 1;
	local_crc = CRC_16((uint8_t *)&pMy_BT_command->fram_prefix0, pMy_BT_command->fram_length + 5 - 2);	//计算CRC待确定
	remote_crc = pMy_BT_command->command_data[pMy_BT_command->fram_length - 2] * 256 + pMy_BT_command->command_data[pMy_BT_command->fram_length - 1];
	if(local_crc != remote_crc)
	{
		return ERROR;	// CRC错误
	}
	pBT_COMMAND pFeedback = (pBT_COMMAND)&MyBT->BT_TX_BUFF_TMP[0];
	memset(MyBT->BT_TX_BUFF_TMP, 0, BT_BUFF_SIZE);
	pFeedback->fram_prefix0 = pMy_BT_command->fram_prefix0;
	pFeedback->fram_prefix1 = pMy_BT_command->fram_prefix1;
	pFeedback->command_code = pMy_BT_command->command_code;
	pFeedback->protocol_version = pMy_BT_command->protocol_version;
	
	TIM4_Count = 0;
	
	switch (pMy_BT_command->command_code)
	{
	  case 0x01:	//升级
		pFeedback->command_data[0] = 0x01;
		pFeedback->fram_length = 1 + 2;
		Calculate_Crc16((uint8_t *)&pFeedback->fram_prefix0, pFeedback->fram_length + 5 - 2);
		sendUsartData(USART_FOR_BT, MyBT->BT_TX_BUFF_TMP, pFeedback->fram_length + 5);
		enter_Bootloader();
		nop();
		break;
	  case 0x02:	//读IP
		pFeedback->command_data[0] = 	TagParam.DefaultIPAddr[0];
		pFeedback->command_data[1] =  	TagParam.DefaultIPAddr[1];
		pFeedback->command_data[2] =  	TagParam.DefaultIPAddr[2];
		pFeedback->command_data[3] =  	TagParam.DefaultIPAddr[3];
		pFeedback->command_data[4] =	TagParam.DefaultPort % 256;
		pFeedback->command_data[5] =	TagParam.DefaultPort / 256;
		pFeedback->fram_length = 6 + 2;
		Calculate_Crc16((uint8_t *)&pFeedback->fram_prefix0, pFeedback->fram_length + 5 - 2);
		sendUsartData(USART_FOR_BT, MyBT->BT_TX_BUFF_TMP, pFeedback->fram_length + 5);
		break;
	  case 0x03:	//写IP
		TagParam.DefaultIPAddr[0] = pMy_BT_command->command_data[0];
		TagParam.DefaultIPAddr[1] = pMy_BT_command->command_data[1];
		TagParam.DefaultIPAddr[2] = pMy_BT_command->command_data[2];
		TagParam.DefaultIPAddr[3] = pMy_BT_command->command_data[3];
		TagParam.DefaultPort = pMy_BT_command->command_data[4] + (pMy_BT_command->command_data[5] * 256);
		pFeedback->command_data[0] = 0x01;
		pFeedback->fram_length = 1 + 2;
		Calculate_Crc16((uint8_t *)&pFeedback->fram_prefix0, pFeedback->fram_length + 5 - 2);
		sendUsartData(USART_FOR_BT, &MyBT->BT_TX_BUFF_TMP[0], pFeedback->fram_length + 5);
		WriteTagParam((Pcontroler_Symple)&TagCng_symple);
		break;
	  case 0x04:	//获取本底
		pFeedback->command_data[0] = DynamicBottom.EMData_xBottom;
		pFeedback->command_data[1] = DynamicBottom.EMData_yBottom;
		pFeedback->command_data[2] = DynamicBottom.EMData_zBottom;
		pFeedback->fram_length = 3 + 2;
		Calculate_Crc16((uint8_t *)&pFeedback->fram_prefix0, pFeedback->fram_length + 5 - 2);
		sendUsartData(USART_FOR_BT, &MyBT->BT_TX_BUFF_TMP[0], pFeedback->fram_length + 5);
		break;
	  case 0x05:	//设置本底
		pFeedback->fram_length = 1 + 2;
		if(SetMagBase() == 1)
		{
			pFeedback->command_data[0] = 1;
			SendNodeCount = SendPackCount - 1;
		}
		else
		{
			pFeedback->command_data[0] = 0;
		}
		Calculate_Crc16((uint8_t *)&pFeedback->fram_prefix0, pFeedback->fram_length + 5 - 2);
		sendUsartData(USART_FOR_BT, &MyBT->BT_TX_BUFF_TMP[0], pFeedback->fram_length + 5);            		//获取本底
		break;
	  case 0x06:	//获取group ID
		pFeedback->command_data[0] = TagCng_symple.Config.TagNode.UserCode%256;
		pFeedback->command_data[1] = TagCng_symple.Config.TagNode.UserCode/256;
		pFeedback->fram_length = 2 + 2;
		Calculate_Crc16((uint8_t *)&pFeedback->fram_prefix0, pFeedback->fram_length + 5 - 2);
		sendUsartData(USART_FOR_BT, &MyBT->BT_TX_BUFF_TMP[0], pFeedback->fram_length + 5);
		nop();
		break;
	  case 0x07:	//设置group ID
		TagCng_symple.Config.TagNode.UserCode = pMy_BT_command->command_data[0] + pMy_BT_command->command_data[1] * 256;
		pFeedback->command_data[0] = 0x01;
		pFeedback->fram_length = 1 + 2;
		Calculate_Crc16((uint8_t *)&pFeedback->fram_prefix0, pFeedback->fram_length + 5 - 2);
		sendUsartData(USART_FOR_BT, &MyBT->BT_TX_BUFF_TMP[0], pFeedback->fram_length + 5);
		WriteTagParam((Pcontroler_Symple)&TagCng_symple);
		break;
	  case 0x08:	//获取ID
		pFeedback->command_data[0] = TagCng_symple.Config.TagNode.Tag_id_nub[0];
		pFeedback->command_data[1] = TagCng_symple.Config.TagNode.Tag_id_nub[1];
		pFeedback->command_data[2] = TagCng_symple.Config.TagNode.Tag_id_nub[2];
		pFeedback->fram_length = 3 + 2;
		Calculate_Crc16((uint8_t *)&pFeedback->fram_prefix0, pFeedback->fram_length + 5 - 2);
		sendUsartData(USART_FOR_BT, &MyBT->BT_TX_BUFF_TMP[0], pFeedback->fram_length + 5);
		break;
	  case 0x09:	//设置ID
		TagCng_symple.Config.TagNode.Tag_id_nub[0] = pMy_BT_command->command_data[0];
		TagCng_symple.Config.TagNode.Tag_id_nub[1] = pMy_BT_command->command_data[1];
		TagCng_symple.Config.TagNode.Tag_id_nub[2] = pMy_BT_command->command_data[2];
		pFeedback->command_data[0] = 0x01;
		pFeedback->fram_length = 1 + 2;
		Calculate_Crc16((uint8_t *)&pFeedback->fram_prefix0, pFeedback->fram_length + 5 - 2);
		sendUsartData(USART_FOR_BT, &MyBT->BT_TX_BUFF_TMP[0], pFeedback->fram_length + 5);
		WriteTagParam((Pcontroler_Symple)&TagCng_symple);
		break;
	  case 0x0A:	//AT指令
		memset(ATCmdFromBTData, 0, 30);
		memcpy(ATCmdFromBTData, (const void *)&pMy_BT_command->command_data[0], pMy_BT_command->fram_length - 2);
		ATCmdFromBTData[pMy_BT_command->fram_length - 2] = '\r';
		ATCmdFromBTData[pMy_BT_command->fram_length - 1] = '\n';
		NBAtCommand.ATFromBTFlag = 0x01;
		break;
	  case 0x0B:	//获取地磁
		Getdataa((Pcontroler_Symple)&TagCng_symple,1);
		berthStateSwithProcess(&ParkState, (Pcontroler_Symple)&TagCng_symple);
		pFeedback->command_data[0] = Sensor3100L.EMData_x;
		pFeedback->command_data[1] = Sensor3100L.EMData_y;
		pFeedback->command_data[2] = Sensor3100L.EMData_z;
		if(ParkState == 0x01)	//有车
		{
			pFeedback->command_data[3] = 0x01;
		}
		else
		{
			pFeedback->command_data[3] = 0x00;
		}
		pFeedback->command_data[4] = Sensor3100L.diffOfRM;
		pFeedback->fram_length = 5 + 2;
		Calculate_Crc16((uint8_t *)&pFeedback->fram_prefix0, pFeedback->fram_length + 5 - 2);
		sendUsartData(USART_FOR_BT, &MyBT->BT_TX_BUFF_TMP[0], pFeedback->fram_length + 5);
		nop();
		break;
	  case 0x0C:	//升级数据帧操作码
		break;
	  case 0x0D:	//获取设备激活状态
		pFeedback->command_data[0] = TagCng_symple.Config.Tag_initNB;
		pFeedback->fram_length = 1 + 2;
		Calculate_Crc16((uint8_t *)&pFeedback->fram_prefix0, pFeedback->fram_length + 5 - 2);
		sendUsartData(USART_FOR_BT, &MyBT->BT_TX_BUFF_TMP[0], pFeedback->fram_length + 5);
		break;
	  case 0x0E:	//设置设备激活状态
		if(pMy_BT_command->command_data[0] == 0x01)
		{
			if(TagCng_symple.Config.Tag_initNB != 0x01)
			{
				TagCng_symple.Config.Tag_initNB = 0x01;
				NBiotPowerSet;
			}
		}
		else if(pMy_BT_command->command_data[0] == 0x00)
		{
			if(TagCng_symple.Config.Tag_initNB != 0x00)
			{
				TagCng_symple.Config.Tag_initNB = 0x00;
				NBiotPowerClr;
				NBAtCommand.NetworkStatus = 0;
			}
		}
		else
		{}
		pFeedback->command_data[0] = 0x01;
		pFeedback->fram_length = 1 + 2;
		Calculate_Crc16((uint8_t *)&pFeedback->fram_prefix0, pFeedback->fram_length + 5 - 2);
		sendUsartData(USART_FOR_BT, &MyBT->BT_TX_BUFF_TMP[0], pFeedback->fram_length + 5);
		break;
	  case 0x0F:	//获取心跳时间
		pFeedback->command_data[0] = TagParam.HeartbeatInterval % 256;
		pFeedback->command_data[1] = TagParam.HeartbeatInterval / 256;
		pFeedback->fram_length = 2 + 2;
		Calculate_Crc16((uint8_t *)&pFeedback->fram_prefix0, pFeedback->fram_length + 5 - 2);
		sendUsartData(USART_FOR_BT, &MyBT->BT_TX_BUFF_TMP[0], pFeedback->fram_length + 5);
		break;
	  case 0x10:	//设置心跳时间
		TagCng_symple.Config.TagPara.HeartbeatInterval = pMy_BT_command->command_data[0] + pMy_BT_command->command_data[1] * 256;
		pFeedback->command_data[0] = 0x01;
		pFeedback->fram_length = 1 + 2;
		Calculate_Crc16((uint8_t *)&pFeedback->fram_prefix0, pFeedback->fram_length + 5 - 2);
		sendUsartData(USART_FOR_BT, &MyBT->BT_TX_BUFF_TMP[0], pFeedback->fram_length + 5);
		WriteTagParam((Pcontroler_Symple)&TagCng_symple);
		SendPackCount = (TagCng_symple.Config.TagPara.HeartbeatInterval * 60)/TagCng_symple.Config.TagPara.WdtInterval;
		break;
	  default:
		break;
	}
	memset(MyBT->BT_RX_BUF_TMP, 0, BT_BUFF_SIZE);
	MyBT->BT_RX_CNT = 0;
	return ERROR;
}

/*
 *@name:	judge_BT_buffer
 *@author:	HL
 *@date:	2017-12-04
 *@brief:	判断蓝牙模块串口接收BUFFER
 *@return:	
 */
void judge_BT_buffer(pBT MyBT){
	if(strstr(MyBT->BT_RX_BUF_TMP, "AT-B STATE 03") != NULL)
	{
		MyBT->BT_RX_CNT = 0;
		MyBT->BT_mode_status = CONNECTED;
		memset(MyBT->BT_RX_BUF_TMP, 0, BT_BUFF_SIZE);
	}
	if(strstr(MyBT->BT_RX_BUF_TMP, "AT-B STATE 01") != NULL)
	{
		MyBT->BT_RX_CNT = 0;
		MyBT->BT_mode_status = ADVERTISING;
		memset(MyBT->BT_RX_BUF_TMP, 0, BT_BUFF_SIZE);
	}
	if(strstr(MyBT->BT_RX_BUF_TMP, "AT-B STATE 00") != NULL)
	{
		MyBT->BT_RX_CNT = 0;
		MyBT->BT_mode_status = INIT_READY;
		memset(MyBT->BT_RX_BUF_TMP, 0, BT_BUFF_SIZE);
	}
	if(strstr(MyBT->BT_RX_BUF_TMP, "AT-B STATE 04") != NULL)
	{
		MyBT->BT_RX_CNT = 0;
		MyBT->BT_mode_status = DISCONNECTED;
		memset(MyBT->BT_RX_BUF_TMP, 0, BT_BUFF_SIZE);
	}
	if(strstr(MyBT->BT_RX_BUF_TMP, "AT-B STATE 05") != NULL)
	{
		MyBT->BT_RX_CNT = 0;
		MyBT->BT_mode_status = STANDBY;
		memset(MyBT->BT_RX_BUF_TMP, 0, BT_BUFF_SIZE);
	}
	if(strstr(MyBT->BT_RX_BUF_TMP, "AT-B STATE 11") != NULL)
	{
		MyBT->BT_RX_CNT = 0;
		MyBT->BT_mode_status = CENTRAL_SCAN;
		memset(MyBT->BT_RX_BUF_TMP, 0, BT_BUFF_SIZE);
	}
	if(strstr(MyBT->BT_RX_BUF_TMP, "AT-B STATE 13") != NULL)
	{
		MyBT->BT_RX_CNT = 0;
		MyBT->BT_mode_status = CENTRAL_CON;
		memset(MyBT->BT_RX_BUF_TMP, 0, BT_BUFF_SIZE);
	}
	if(strstr(MyBT->BT_RX_BUF_TMP, "AT-B STATE 13") != NULL)
	{
		MyBT->BT_RX_CNT = 0;
		MyBT->BT_mode_status = CENTRAL_DISCON;
		memset(MyBT->BT_RX_BUF_TMP, 0, BT_BUFF_SIZE);
	}
	if(MyBT->BT_RX_BUF_TMP[0] == 0xFF && MyBT->BT_RX_BUF_TMP[1] == 0x66)
	{
		MyBT->BT_rx_mode = DATA_MODE;
		memcpy(MyBT->BT_RX_BUF, MyBT->BT_RX_BUF_TMP, BT_BUFF_SIZE);
		MyBT->BT_RX_CNT = 0;
		MyBT->BT_RX_FLAG = 0x01;
	}
	else if(MyBT->BT_RX_BUF_TMP[0] == 'A' && MyBT->BT_RX_BUF_TMP[1] == 'T')
	{
		MyBT->BT_rx_mode = AT_MODE;
		if(strstr(MyBT->BT_RX_BUF_TMP, MyBT_CMD_Data[MyBT->current_cmd_num].pTrueBack) != NULL)
		{
			memcpy(MyBT->BT_RX_BUF, MyBT->BT_RX_BUF_TMP, BT_BUFF_SIZE);
			memset(MyBT->BT_RX_BUF_TMP, 0, BT_BUFF_SIZE);
			MyBT_CMD_Data[MyBT->current_cmd_num].BT_Return = BACK_OK;
		}
		else if(strstr(MyBT->BT_RX_BUF_TMP, "ERROR") != NULL)
		{
			memcpy(MyBT->BT_RX_BUF, MyBT->BT_RX_BUF_TMP, BT_BUFF_SIZE);
			memset(MyBT->BT_RX_BUF_TMP, 0, BT_BUFF_SIZE);
			MyBT_CMD_Data[MyBT->current_cmd_num].BT_Return = BACK_ERROR;
		}
		else
		{}
	}
	else
	{
		memset(MyBT->BT_RX_BUF_TMP, 0, BT_BUFF_SIZE);
		MyBT->BT_RX_CNT = 0;
	}
}

/*******************************************
*Name:enter_MainApp
*Description:跳转到main app函数中去
*Input:NULL
*Output:NULL
*Author:COPY
*Date:2017/12/14
********************************************/
void enter_Bootloader(void)
{
	void (*theBootloader)(void);
	TagParam.UpdateFlag = 0x01;
	WriteTagParam((Pcontroler_Symple)&TagCng_symple);
	_DINT();
	theBootloader = (void (*)(void))(BOOTLOADER_START_ADDRESS);
	asm("nop");
	asm("nop");
	(*theBootloader)();
}