/*
 * @file: 	MyNBIOT.c
 *@author:	HL
 *@date:	2017-12-18
 *@version:	V1.0
 */

/* includes */
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "stm8l15x_conf.h"
#include "struct.h"
#include "UART.h"
#include "MyCommon.h"
#include "McuTarger.h"
#include "Bluetooth.h"
#include "MyNBIOT.h"
#include "Common.h"
#include "CRC.h"
/* defines */
#define ATTACH_NETWORK_MAX_TRY		120

/* private variables */
extern Sontroler_Symple TagCng_symple;
extern sBT bluetooth_data;
extern char ATCmdFromBTData[30];
/* global variables */
/***********鎸囦护璇存槑***************
*AT+
*NRB:妯″潡閲嶅惎                                  CMEE=1:鎶ュ憡绉诲姩缁堢閿欒            CFUN?:璁剧疆缁堢鍔熻兘 
*NBAND?:璁剧疆棰戞                               NCCID?:寮�鐗囪瘑鍒?               CGSN=1:鏌ヨ妯″潡搴忓垪鍙?
*CIMI:鏌ヨ鍥介檯绉诲姩璁惧韬唤鐮?                         CSQ:鑾峰彇淇″彿寮哄害                 CEREG?:鏌ヨ缃戠粶娉ㄥ唽鐘舵�?
*CGPADDR:鏄剧ずPDP鍦板潃                           CCLK?:杩斿洖褰撳墠鏃ユ湡鍜屾椂闂?           CSCON?:鏌ヨ淇″彿杩炴帴鐘舵�?
*CGATT?:PS杩炴帴鎴栧垎绂?                           NUESTATS:鑾峰彇鐨勬搷浣滅粺璁?          CGMR:鏌ヨ鍥轰欢鐗堟湰
*NSOCR=DGRAM,17,8888,1:鍒涘缓Socket,17-udp     NSOST:鍙戦�佹暟鎹?                NSORF=0,256:鎺ユ敹鍛戒护 0-NSOCR杩斿洖鍊?256-max recive data len
*NSOCL=0:鍏抽棴Socket                          NSOCR=DGRAM,17,8990,1/AT+NSOCR=DGRAM,17,8992,1-鍒涘缓涓嶅悓绔彛鐨剆ocket
*/ 
const sNB_CMD MyNBIOT_CMD_data[30] = 
{
	// 	指令内容									最大尝试次数		超时时间ms		  		正确返回值			初始状态		执行指令间隔ms
	{"",												3,					1000,				"",				NO_BACK,			100},
	{"AT\r\n",											5,					30,					"OK",			NO_BACK,			0},
	{"AT+NRB\r\n",										3,					7000,				"OK",			NO_BACK,			0},
	{"AT+CMEE=1\r\n",									3,					100,				"OK",			NO_BACK,			0},
	{"AT+CFUN?\r\n",									5,					30,					"OK",			NO_BACK,			100},
	{"AT+NBAND?\r\n",									3,					100,				"OK",			NO_BACK,			0},
	{"AT+NCCID?\r\n",									3,					100,				"OK",			NO_BACK,			0},
	{"AT+CGSN=1\r\n",									3,					100,				"OK",			NO_BACK,			0},	//
	{"AT+CIMI\r\n",										3,					100,				"OK",			NO_BACK,			0},
	{"AT+CSQ\r\n",										10,					100,				"OK",			NO_BACK,			1000},
	{"AT+CEREG?\r\n",									3,					30,					"OK",			NO_BACK,			1000},
	{"AT+CGPADDR\r\n",									3,					40,					"OK",			NO_BACK,			0},
	{"AT+CCLK?\r\n",									3,					100,				"OK",			NO_BACK,			0},
	{"AT+CSCON?\r\n",									3,					100,				"OK",			NO_BACK,			0},
	{"AT+CGATT?\r\n",									3,					30,					"OK",			NO_BACK,			1000},
	{"AT+NUESTATS\r\n",									3,					100,				"OK",			NO_BACK,			0},
	{"AT+CGMR\r\n",										3,					100,				"OK",			NO_BACK,			0},
	{"AT+NSOCR=DGRAM,17,8888,1\r\n",					3,					30,					"OK",			NO_BACK,			10},
	{"AT+NSOST\r\n",									1,					30,					"OK",			NO_BACK,			0},
	{"AT+NSORF=0,256\r\n",								3,					100,				"OK",			NO_BACK,			0},
	{"AT+NSOCL=0\r\n",									3,					100,				"OK",			NO_BACK,			0},
	{"AT+NSOCR=DGRAM,17,8990,1\r\n",					3,					500,				"OK",			NO_BACK,			10},
	{"AT+NSOCR=DGRAM,17,8991,1\r\n",					3,					500,				"OK",			NO_BACK,			10},
	{"AT+NCDP=180.101.147.115,5683\r\n",					3,					500,				"OK",			NO_BACK,			10},
};
/* private functions declaration */

/* functions */
/*
 *@name:	send_data_to_NB
 *@author:	HL
 *@date:	2017-12-18
 *@brief:	
 *@return:	成功或者错误，如果错误，错误类型在pMyCMD->NB_Return
 */
RETURN_STATUS SendDataToNB(pNBiotAT pMyNBAT)//, uint8_t cmd_num, char *fmt,...)
{
	uint32_t CurrentCmdTime = 0;
	CurrentCmdTime = pMyNBAT->TryMaxTimeMs;
	pMyNBAT->NBReturn = NO_BACK;
	memset(pMyNBAT->ATReturn, 0, NB_RECEIVE_BUFFER_SIZE);
	memset(pMyNBAT->ErrorCode, 0, NB_ERROR_CODE_SIZE);
	pMyNBAT->RxCnt = 0;
	printf("Send to NB:%s\n", pMyNBAT->ATCMDData);
	sendUsartData(USART_FOR_NB, pMyNBAT->ATCMDData, pMyNBAT->TxCnt);
	while(pMyNBAT->NBReturn == NO_BACK)
	{
		DelayOneMs(1);
		CurrentCmdTime--;
		if(!CurrentCmdTime)
		{
			pMyNBAT->NBReturn = BACK_TIMEOUT;
		}
	}
	pMyNBAT->CurrentCmdTime = CurrentCmdTime;
	return pMyNBAT->NBReturn;
}

/*
 *@name:	CopyString
 *@author:	HL
 *@date:	2018-02-01
 *@brief:	拷贝字符串
 *@return:	返回字符串长度
 */
uint8_t CopyString(char *pTo, char *pFrom)
{
	uint32_t i = 0;
	while(pFrom[i] != '\0')
	{
		pTo[i] = pFrom[i];
		i++;
	}
	return i;
}

ErrorStatus DoNBiotATCmd(pNBiotAT pMyNBAT, uint8_t cmd_num, JUDGE_OPTION option, char *ptr, char *fmt,...)
{
	RETURN_STATUS AtReturn = NO_BACK;
	uint16_t doCmdCnt = 0;
	uint8_t len = 0;
	memset(pMyNBAT->ATCMDData, 0, NB_SEND_BUFFER_SIZE);
	if(cmd_num == 0)
	{
		char *ptr = pMyNBAT->ATCMDData;
		va_list ap;
		va_start(ap, fmt);
		len = (uint8_t)vsprintf(ptr, fmt, ap);
		pMyNBAT->CurrentCmdTime = MyNBIOT_CMD_data[cmd_num].timeout_s;
	}
	else
	{
		len = CopyString(pMyNBAT->ATCMDData, MyNBIOT_CMD_data[cmd_num].pCMD_Data);
		pMyNBAT->TryMaxTimeMs = MyNBIOT_CMD_data[cmd_num].timeout_s;// * 1000;
//		pMyNBAT->CurrentCmdTime = pMyNBAT->TryMaxTimeMs;
		pMyNBAT->MaxTryCnt = MyNBIOT_CMD_data[cmd_num].MaxTry;
		pMyNBAT->IntervalMs = MyNBIOT_CMD_data[cmd_num].interval_ms;
		CopyString(pMyNBAT->ATTrueBack,MyNBIOT_CMD_data[cmd_num].pTrueBack);
	}
	pMyNBAT->CurrentCmdNum = cmd_num;
	pMyNBAT->TxCnt = len;
	while(doCmdCnt < pMyNBAT->MaxTryCnt && pMyNBAT->NBEnableFlag == 0x01)
	{
		AtReturn = SendDataToNB(pMyNBAT);
		if(AtReturn == BACK_OK)
		{
			if(option == JUDGE_NONE)
			{
				return SUCCESS;
			}
			else if(option == JUDGE_AND)
			{
				if(strstr(pMyNBAT->ATReturn, ptr) != NULL)
				{
					return SUCCESS;
				}
				else
				{
//					return ERROR;
					if(pMyNBAT->IntervalMs != 0)
					{
						DelayOneMs(pMyNBAT->IntervalMs);
					}
					doCmdCnt++;
				}
			}
			else if(option == JUDGE_NOT)
			{
				if(strstr(pMyNBAT->ATReturn, ptr) != NULL)
				{
//					return ERROR;
					if(pMyNBAT->IntervalMs != 0)
					{
						DelayOneMs(pMyNBAT->IntervalMs);
					}
					doCmdCnt++;
				}
				else
				{
					return SUCCESS;
				}
			}
			else
			{
			}
		}
		else if(AtReturn == BACK_ERROR)
		{
//			return ERROR;
			if(pMyNBAT->IntervalMs != 0)
			{
				DelayOneMs(pMyNBAT->IntervalMs);
			}
			doCmdCnt++;
		}
		else
		{
			if(pMyNBAT->IntervalMs != 0)
			{
				DelayOneMs(pMyNBAT->IntervalMs);
			}
			doCmdCnt++;
		}
	}
	return ERROR;
}

void DoATCmdFromBT(pNBiotAT pMyNBAT)
{
	if(bluetooth_data.BTEnableFlag != 0x01)
	{
		return;
	}
	if(pMyNBAT->ATFromBTFlag != 0x01)
	{
		return;
	}
	pMyNBAT->ATFromBTFlag = 0x00;
	pBT_COMMAND pFeedback = (pBT_COMMAND)&bluetooth_data.BT_TX_BUFF_TMP[0];
	CopyString(pMyNBAT->ATTrueBack, "OK");
	pMyNBAT->MaxTryCnt = 3;
	pMyNBAT->IntervalMs = 30;
	pMyNBAT->NBReturn = NO_BACK;
	pMyNBAT->NBEnableFlag = 0x01;
	DoNBiotATCmd(pMyNBAT, 0, JUDGE_NONE, "", ATCmdFromBTData);
	memcpy(pFeedback->command_data, pMyNBAT->ATReturn, pMyNBAT->RxCnt);
	pFeedback->fram_prefix0 = 0xFF;
	pFeedback->fram_prefix1 = 0x66;
	pFeedback->command_code = 0x0A;
	pFeedback->fram_length = pMyNBAT->RxCnt + 2;
	Calculate_Crc16((uint8_t *)&pFeedback->fram_prefix0, pFeedback->fram_length + 5 - 2);
	sendUsartData(USART_FOR_BT, &bluetooth_data.BT_TX_BUFF_TMP[0], pFeedback->fram_length + 5);
	memset(&bluetooth_data.BT_TX_BUFF_TMP[0], 0, BT_BUFF_SIZE);
}

/*
 *@name:	send_data_to_NB
 *@author:	HL
 *@date:	2017-12-18
 *@brief:	
 *@return:	成功或者错误，如果错误，错误类型在pMyCMD->NB_Return
 */
RETURN_STATUS send_data_to_NB(pNB pMyNB, uint8_t cmd_num, char *fmt,...)
{
	uint8_t len = 0;	// the length of data that sended to NB
//	memset(pMyNB->Tx_buff_tmp, 0, NB_SEND_BUFFER_SIZE);
	memset(pMyNB->Tx_buff, 0, NB_SEND_BUFFER_SIZE);
	char *ptr = pMyNB->Tx_buff;
	enableUsartNBIot();//打开NB串口
	va_list ap;
	va_start(ap, fmt);
	len = (uint8_t)vsprintf(ptr, fmt, ap);
	pMyNB->tx_cnt = len;
//	MyNBIOT_CMD_data[cmd_num].NB_return = NO_BACK;
	
	memset(pMyNB->Rx_buff, 0, NB_RECEIVE_BUFFER_SIZE);
	memset(pMyNB->Rx_buff_tmp, 0, NB_RECEIVE_BUFFER_SIZE);
	
	pMyNB->current_cmd_num = cmd_num;
	pMyNB->current_cmd_time = MyNBIOT_CMD_data[cmd_num].timeout_s;// * 1000;	// unit: ms
	
	pMyNB->rx_cnt = 0;
	sendUsartData(USART_FOR_NB, pMyNB->Tx_buff, len);
	while(MyNBIOT_CMD_data[cmd_num].NB_return == NO_BACK)
	{
		DelayOneMs(1);
		pMyNB->current_cmd_time--;
		if(!pMyNB->current_cmd_time)
		{
//			MyNBIOT_CMD_data[cmd_num].NB_return = BACK_TIMEOUT;
		}
	}
	
	memset(pMyNB->Tx_buff, 0, NB_SEND_BUFFER_SIZE);
	memset(pMyNB->Tx_buff_tmp, 0, NB_SEND_BUFFER_SIZE);
	return MyNBIOT_CMD_data[cmd_num].NB_return;
}

/*
 *@name:	do_NB_CMD
 *@author:	HL
 *@date:	2017-12-18
 *@brief:	
 *@return:	成功或者错误，如果错误，错误类型在pMyCMD->NB_Return
 */
ErrorStatus do_NB_CMD(pNB pMyNB, uint8_t cmd_num){
	uint16_t doNB_CMD_cnt = 0;
	pMyNB->current_cmd_cnt = 0;
	while(doNB_CMD_cnt < MyNBIOT_CMD_data[cmd_num].MaxTry)
	{
		send_data_to_NB(pMyNB, cmd_num, MyNBIOT_CMD_data[cmd_num].pCMD_Data);
		if(MyNBIOT_CMD_data[cmd_num].NB_return == BACK_OK)
		{
			return SUCCESS;
		}
		else if(MyNBIOT_CMD_data[cmd_num].NB_return == BACK_ERROR)
		{
			return ERROR;
		}
		DelayOneMs(MyNBIOT_CMD_data[cmd_num].interval_ms);
		doNB_CMD_cnt++;
	}
	pMyNB->current_cmd_cnt = doNB_CMD_cnt;
	return ERROR;
}

/*
*@FunctionName: checkNBNetwork
*@Brief: 查询自动附着网络状态
*@Author:
*@Date: 2018-02-02
 */
uint32_t AttachNetworkCount = 0;
void checkNBNetwork(pNBiotAT pMyNBAT)
{
	if((pMyNBAT->NetworkStatus & 0x80) == 0x80)   
	{
		return;
	}
	if(bluetooth_data.BTEnableFlag == 0x01)
	{
		return;
	}
	if(TagCng_symple.Config.Tag_initNB != 0x01)
	{
		return;
	}
	LED1_set;
	#if COAP_TEST   //and by yjd   
	if(DoNBiotATCmd(pMyNBAT, AT_CEREG, JUDGE_AND, "CEREG:0,1", pMyNBAT->ATCMDData) == SUCCESS)	//注册网络成功
	{
		//DoNBiotATCmd(pMyNBAT, AT_CDPSERVERCFG, JUDGE_NONE, "", pMyNBAT->ATCMDData);
                char *CDPCntStr = "AT+NCDP=180.101.147.115,5683\r\n";
                printf("CDPCntStr len = %d\n", strlen(CDPCntStr));
				printf("CDPCntStr = %s\n", CDPCntStr);
                sendUsartData(USART_FOR_NB, CDPCntStr, strlen(CDPCntStr));
                sendUsartData(USART_FOR_NB, CDPCntStr, strlen(CDPCntStr));
                sendUsartData(USART_FOR_NB, CDPCntStr, strlen(CDPCntStr));
		pMyNBAT->NetworkStatus |= 0x80;
		pMyNBAT->SleepFlag = 0x00;
		TagCng_symple.Config.TagNode.McuSleepFlag = 0x01;
		LED1_clr;DelayOneMs(300);LED1_set;DelayOneMs(300);
		LED1_clr;DelayOneMs(300);LED1_set;DelayOneMs(300);
		LED1_clr;DelayOneMs(300);LED1_set;DelayOneMs(300);LED1_clr;
		printf("COAP net work registere successful...\n");
	}
	#else
	if(DoNBiotATCmd(pMyNBAT, AT_CEREG, JUDGE_AND, "CEREG:0,1", pMyNBAT->ATCMDData) == SUCCESS)	//注册网络成功
	{
		DoNBiotATCmd(pMyNBAT, AT_CGPADDR, JUDGE_NONE, "", pMyNBAT->ATCMDData);
		DoNBiotATCmd(pMyNBAT, AT_NSOCR1, JUDGE_NONE, "", pMyNBAT->ATCMDData);
		DoNBiotATCmd(pMyNBAT, AT_NSOCR2, JUDGE_NONE, "", pMyNBAT->ATCMDData);
		DoNBiotATCmd(pMyNBAT, AT_NSOCR3, JUDGE_NONE, "", pMyNBAT->ATCMDData);
		pMyNBAT->NetworkStatus |= 0x80;
		pMyNBAT->SleepFlag = 0x00;
		TagCng_symple.Config.TagNode.McuSleepFlag = 0x01;
		LED1_clr;DelayOneMs(300);LED1_set;DelayOneMs(300);
		LED1_clr;DelayOneMs(300);LED1_set;DelayOneMs(300);
		LED1_clr;DelayOneMs(300);LED1_set;DelayOneMs(300);LED1_clr;
		printf("UDP net work registere successful...\n");
	}
	#endif    
	else
	{
		AttachNetworkCount++;
                printf("start to ATTACH_NETWORK_MAX_TRY");
		if(AttachNetworkCount > ATTACH_NETWORK_MAX_TRY)
		{
			AttachNetworkCount = 0;
			DoNBiotATCmd(pMyNBAT, AT_NRB, JUDGE_NONE, "", pMyNBAT->ATCMDData);
			printf("ATTACH_NETWORK_MAX_TRY");
		}
		//未注册到网络
	}
	
}

/*
 *@name:	check_NB_network
 *@author:	HL
 *@date:	2017-12-19
 *@brief:	
 *@return:	成功或者错误，如果错误，错误类型在pMyCMD->NB_Return
 */
uint8_t check_NB_network(pNB pMyNB)
{
	if((pMyNB->network_status & 0x80) == 0x80)
	{
		return pMyNB->network_status;
	}
	// auto attach network
	LED1_set;
	
	if(do_NB_CMD(pMyNB, AT_CMD) == SUCCESS)
	{
		pMyNB->network_status |= 0x01;
	}
	else
	{
		pMyNB->network_status &= (~0x01);
//		return pMyNB->network_status;
	}
	
	do_NB_CMD(pMyNB, AT_NRB);
	
	do_NB_CMD(pMyNB, AT_NCCID);
	
	send_data_to_NB(pMyNB, 0, "AT+CMEE=1\r\n");
	
//	MyNBIOT_CMD_data[AT_CFUN].pTrueBack = "+CFUN:1";
	if(do_NB_CMD(pMyNB, AT_CFUN) == SUCCESS)
	{
		pMyNB->network_status |= 0x02;
	}
	else
	{
		pMyNB->network_status &= (~0x02);	//不能打开射频，可能SIM卡没插好
		return pMyNB->network_status;
	}
	
	// 设置APN
	
	if(getCSQ(pMyNB) == SUCCESS)			// 获取信号强度
	{
		pMyNB->network_status |= 0x04;
	}
	else
	{
		pMyNB->network_status &= (~0x04);	//不能打开射频，可能SIM卡没插好
		return pMyNB->network_status;
	}
	
	
//	MyNBIOT_CMD_data[AT_CGATT].pTrueBack = "+CGATT:1";
	if(do_NB_CMD(pMyNB, AT_CGATT) == SUCCESS)
	{
		pMyNB->network_status |= 0x08;
	}
	else
	{
		pMyNB->network_status &= (~0x08);
		return pMyNB->network_status;
	}
	
//	MyNBIOT_CMD_data[AT_CEREG].pTrueBack = "+CEREG:0,1";
	if(do_NB_CMD(pMyNB, AT_CEREG) == SUCCESS)
	{
		pMyNB->network_status |= 0x10;
	}
	else
	{
		pMyNB->network_status &= (~0x10);
		return pMyNB->network_status;
	}
	
	// 获取本地IP
	if(do_NB_CMD(pMyNB, AT_CGPADDR) == SUCCESS)
	{
		pMyNB->network_status |= 0x20;
	}
	else
	{
		pMyNB->network_status &= (~0x20);
		return pMyNB->network_status;
	}
	
	pMyNB->network_status = 0x80;
	
	pMyNB->sleep_status = 0x01;
	pMyNB->sleep_end_cnt = 429496729;
	pMyNB->sleep_cnt = 0;
	
	LED1_clr;
	
	for(uint8_t i = 0; i < 3; i++)
	{
		LED1_set;
		DelayOneMs(500);
		LED1_clr;
		DelayOneMs(500);
	}
	
	return pMyNB->network_status;
}

/*
 *@name:	check_socket_status
 *@author:	HL
 *@date:	2017-12-19
 *@brief:	创建socket
 *@return:	
 */
void check_socket_status(pNB pMyNB)
{
	if((pMyNB->network_status & 0x80) != 0x80)
	{
		return;
	}
	pMyNB->default_server.status |= 0x80;		//打开默认服务器socket
#ifdef DEFAULT_SERVER_ADDR	//宏定义在MyNBIOT.h 文件中 调试用默认服务器
	uint16_t tmp[5];
	sscanf(DEFAULT_SERVER_ADDR, "%d.%d.%d.%d:%d", &tmp[0], &tmp[1], &tmp[2], &tmp[3], &tmp[4]);
	pMyNB->default_server.ip[0] = tmp[0];
	pMyNB->default_server.ip[1] = tmp[1];
	pMyNB->default_server.ip[2] = tmp[2];
	pMyNB->default_server.ip[3] = tmp[3];
	pMyNB->default_server.port  = tmp[4];
#endif
//	MyNBIOT_CMD_data[AT_NSOCR].pCMD_Data = "AT+NSOCR=DGRAM,17,9990,1\r\n";
	if(((pMyNB->default_server.status & 0x80) == 0x80) && ((pMyNB->default_server.status & 0x40) != 0x40))
	{
		if(do_NB_CMD(pMyNB, AT_NSOCR) == SUCCESS)
		{
			pMyNB->default_server.status |= 0x40;
		}
		else{
			pMyNB->default_server.status &= (~0x40);
		}
	}
	
//	MyNBIOT_CMD_data[AT_NSOCR].pCMD_Data = "AT+NSOCR=DGRAM,17,9991,1\r\n";
	if(((pMyNB->server1.status & 0x80) == 0x80) && ((pMyNB->default_server.status & 0x40) != 0x40))
	{
		
		if(do_NB_CMD(pMyNB, AT_NSOCR) == SUCCESS)
		{
			pMyNB->server1.status |= 0x40;
		}
		else{
			pMyNB->server1.status &= (~0x40);
		}
	}
	
//	MyNBIOT_CMD_data[AT_NSOCR].pCMD_Data = "AT+NSOCR=DGRAM,17,9992,1\r\n";
	if(((pMyNB->server2.status & 0x80) == 0x80) && ((pMyNB->server2.status & 0x40) != 0x40))
	{
		
		if(do_NB_CMD(pMyNB, AT_NSOCR) == SUCCESS)
		{
			pMyNB->server2.status |= 0x40;
		}
		else{
			pMyNB->server2.status &= (~0x40);
		}
	}
}

/*
 *@name:	MyPrintfToString
 *@author:	HL
 *@date:	2017-12-19
 *@brief:	打印字符串
 *@return:	
 */
uint8_t MyPrintfToString(char *ptr, char *fmt,...)
{
	uint8_t len = 0;
	va_list ap;
	va_start(ap, fmt);
	len = (uint8_t)vsprintf(ptr, fmt, ap);
	return len;
}

/*
 *@name:	MyPrintfToString
 *@author:	HL
 *@date:	2017-12-19
 *@brief:	check NB模块接收buffer
 *@return:	
 */
ErrorStatus checkNBRecvBuff(pNB pMyNB)
{
	uint32_t try_cnt = 0;
	char temp[100] = {0};
	char *ptr1 = temp;
	uint8_t len = 0;
	volatile uint16_t tmp = 0;
	len = MyPrintfToString(ptr1, "0,%d.%d.%d.%d,%d,", pMyNB->default_server.ip[0], 
						   pMyNB->default_server.ip[1], pMyNB->default_server.ip[2], 
						   pMyNB->default_server.ip[3], pMyNB->default_server.port);
	while(try_cnt < 1 && (pMyNB->nb_recv_data.receive_status & 0x40) != 0x40)
	{
		if(do_NB_CMD(pMyNB, AT_NSORF) == SUCCESS)
		{	
			if(strstr(pMyNB->Rx_buff, ptr1) != NULL)
			{
				pMyNB->nb_recv_data.receive_status |= 0x40;	//收到数据
				char *pstart = strstr(pMyNB->Rx_buff, ptr1);
				sscanf(pstart+len, "%d", &pMyNB->nb_recv_data.length);
				memcpy(pMyNB->nb_recv_data.receive_buff_tmp, pstart+len+3, 2 * pMyNB->nb_recv_data.length);
				ASCII_to_Hex(pMyNB->nb_recv_data.receive_buff_tmp, pMyNB->nb_recv_data.receive_buff, pMyNB->nb_recv_data.length);
				return SUCCESS;
			}
			else
			{
				DelayOneMs(10);
				try_cnt++;
			}
		}
	}
	return ERROR;
}

/*
 *@name:	char_convert_hex
 *@author:	HL
 *@date:	2017-12-19
 *@brief:	convert char to hex
 *@return:	
 */
char char_convert_hex(char *ascii)
{
	char hex=0;
	if((ascii[0]>='0')&&(ascii[0]<='9'))
	{
		hex = ascii[0]-'0';
	}
	else if(ascii[0]>='A'&&ascii[0]<='F')
	{
		hex = ascii[0]-'A'+0x0a;
	}
	else if(ascii[0]>='a'&&ascii[0]<='f')
	{
		hex = ascii[0]-'a'+0x0a;
	}
	hex = hex<<4;
	
	if((ascii[1]>='0')&&(ascii[1]<='9'))
	{
		hex = hex+(ascii[1]-'0');
	}
	else if(ascii[1]>='A'&&ascii[1]<='F')
	{
		hex = hex+(ascii[1]-'A'+0x0a);
	}
	else if(ascii[1]>='a'&&ascii[1]<='f')
	{
		hex = hex+(ascii[0]-'a'+0x0a);
	} 
	return hex;
}

/*
 *@name:	ASCII_to_Hex
 *@author:	HL
 *@date:	2017-12-19
 *@brief:	convert ascii to hex
 *@return:	
 */
void ASCII_to_Hex(char *ascii,uint8_t *hex,unsigned char len)
{
	unsigned char i=0;
	for(i = 0;i < 2 * len; i += 2)
	{
		*hex++ = char_convert_hex(ascii+i);
	}
}

/*
 *@name:	getCSQ
 *@author:	HL
 *@date:	2017-12-19
 *@brief:	get the singal strenth
 *@return:	
 */
ErrorStatus getCSQ(pNB pMyNB)
{
	uint16_t try_cnt = 0;
	uint16_t tmp[2] = {0};
	while(try_cnt < MyNBIOT_CMD_data[AT_CSQ].MaxTry)
	{
		if(do_NB_CMD(pMyNB, AT_CSQ) == SUCCESS)
		{
			if(strstr(pMyNB->Rx_buff, "+CSQ:99,99") != NULL)
			{
				try_cnt++;
				DelayOneMs(MyNBIOT_CMD_data[AT_CSQ].interval_ms);
			}
			else
			{
				char *ptr = strstr(pMyNB->Rx_buff, "+CSQ:");
				sscanf(ptr, "+CSQ:%d,%d", &tmp[0],&tmp[1]);
				pMyNB->rssi = tmp[0];pMyNB->ber = tmp[1];
				return SUCCESS;
			}
		}
	}
	return ERROR;
}

/*
 *@name:	judge_NBIOT_Rx_Buff
 *@author:	HL
 *@date:	2017-12-18
 *@brief:	判断NB接收buff状态
 *@return:	
 */
void judge_NBIOT_Rx_Buff(pNBiotAT pMyNBAT){
	if(strstr(pMyNBAT->ATReturn, pMyNBAT->ATTrueBack) != NULL)
	{
		pMyNBAT->NBReturn = BACK_OK;
	}
	else if(strstr(pMyNBAT->ATReturn, "ERROR") != NULL)
	{
		char *ptr = strstr(pMyNBAT->ATReturn, "ERROR");
		memcpy(pMyNBAT->ErrorCode, ptr, NB_ERROR_CODE_SIZE);
		pMyNBAT->NBReturn = BACK_ERROR;
	}
}

void PowerConfig(void)
{
	if(TagCng_symple.Config.Tag_initNB == 0x01)
	{
		NBiotPowerSet;
	}
	else
	{
		NBiotPowerClr;
	}
}