/*
 * @file: 	NBIot.c
 * @author:	HL
 * @date:	2017-11-23
 * @version:	V1.0
 */

/* includes */
#include "stm8l15x_conf.h"
#include "Common.h"
#include "NBIot.h"
//#include "USART.h"

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
/* defines */

/* private variables */

/* global variable */

/* private functions declaration */
volatile sNBCMD MyNBIotCMD_Data[30] = {
	//ָ������				����Դ���    ��ʱʱ��	��ȷ����ֵ				�Ƿ񷵻�	��ʼ״̬	��������
	{"",						3,				3,			"",					ENABLE,		NO_BACK,	""},	// 0
	{"AT\r\n",					5,				3,			"OK",				ENABLE,		NO_BACK,	""},	// 1
	{"AT+NRB\r\n",				3,				10,			"OK",				ENABLE,		NO_BACK,	""},	// 2
	{"AT+CFUN?\r\n",			10,				3,			"OK",				ENABLE,		NO_BACK,	""},	// 3
//	{"AT+CFUN=1\r\n",			3,				3,			"OK",				ENABLE,		NO_BACK,	""},
	{"AT+NBAND?\r\n",			3,				3,			"OK",				ENABLE,		NO_BACK,	""},	// 4
//	{"AT+NBAND=5\r\n",			3,				3,			"OK",				ENABLE,		NO_BACK,	""},	
	{"AT+NCCID?\r\n",			3,				3,			"OK",				ENABLE,		NO_BACK,	""},	// 5
	{"AT+CGSN=1\r\n",			3,				3,			"OK",				ENABLE,		NO_BACK,	""},	// 6
	{"AT+CIMI\r\n",				3,				3,			"OK",				ENABLE,		NO_BACK,	""},	// 7
	{"AT+CSQ\r\n",				1000,			3,			"OK",				ENABLE,		NO_BACK,	""},	// 8
	{"AT+CEREG?\r\n",			3,				3,			"OK",				ENABLE,		NO_BACK,	""},	// 9
	{"AT+CGPADDR\r\n",			3,				3,			"OK",				ENABLE,		NO_BACK,	""},	// 10
	{"AT+CCLK?\r\n",			3,				3,			"OK",				ENABLE,		NO_BACK,	""},	// 11
	{"AT+CSCON?\r\n",			3,				3,			"OK",				ENABLE,		NO_BACK,	""},	// 12
	{"AT+CGATT?\r\n",			1000,			3,			"+CGATT:1\r\n\r\nOK",				ENABLE,		NO_BACK,	""},	// 13
	{"AT+NUESTATS\r\n",			3,				3,			"OK",				ENABLE,		NO_BACK,	""},	//14
	{"AT+CGMR\r\n",				3,				3,			"SECURITY",			ENABLE,		NO_BACK,	""},	//15
	{"AT+NSOCR=DGRAM,17,9999,1",3,				3,			"OK",				ENABLE,		NO_BACK,	""},	//16
	{"AT+NSOST\r\n",			3,				3,			"OK",				ENABLE,		NO_BACK,	""},	//17
	{"AT+NSORF=0,256\r\n",		3,				3,			"OK",				ENABLE,		NO_BACK,	""},	//18
	{"AT+NSOCL=0\r\n",			3,				3,			"OK",				ENABLE,		NO_BACK,	""},	//19
	{"AT+NCDP=180.101.147.115,5683\r\n",	    3,				3,			"OK",				ENABLE,		NO_BACK,	""},	//20
};
/* functions */

/*
 *@name:	send_data_to_NB
 *@author:	HL
 *@date:	2017-12-01
 *@brief:	ͨ������3������ATָ���ж��Ƿ�OK
 *@return:	�ɹ����ߴ���������󣬴���������pMyCMD->NB_Return
 */
RETURN_STATUS send_data_to_NB(pNB MyNB, uint8_t CMD_NUM, char *fmt,...){
	uint8_t len = 0;
	memset(MyNB->NB_TX_BUFF, 0, NB_BUFFER_SIZE);
	memset(MyNB->NB_TX_BUFF_TMP, 0, NB_BUFFER_SIZE);
	char *ptr = MyNB->NB_TX_BUFF;
	enableUsartNBIot();
	va_list ap;
	va_start(ap,fmt); 
	len = (uint8_t)vsprintf(ptr,fmt,ap);
	va_end(ap);
	MyNB->TX_CNT = len;
	MyNBIotCMD_Data[CMD_NUM].NB_Return = NO_BACK;
	
	memset(MyNB->NB_RX_BUFF, 0, NB_BUFFER_SIZE);
	memset(MyNB->NB_RX_BUFF_TMP, 0, NB_BUFFER_SIZE);
	
	MyNB->current_cmd_num = CMD_NUM;
	MyNB->current_cmd_time = MyNBIotCMD_Data[CMD_NUM].timeout * 1000;
	
	MyNB->RX_CNT = 0;
	sendUsartData(USART_FOR_NB, MyNB->NB_TX_BUFF, len);
	while(MyNBIotCMD_Data[CMD_NUM].NB_Return == NO_BACK){
		DelayMs(1);
		MyNB->current_cmd_time--;
		if(!MyNB->current_cmd_time){
			MyNBIotCMD_Data[CMD_NUM].NB_Return = BACK_TIMEOUT;
		}
	}
	memset(MyNB->NB_TX_BUFF, 0, NB_BUFFER_SIZE);
	memset(MyNB->NB_TX_BUFF_TMP, 0, NB_BUFFER_SIZE);
	return MyNBIotCMD_Data[CMD_NUM].NB_Return;
}

/*
 *@name:	doNB_CMD
 *@author:	HL
 *@date:	2017-12-01
 *@brief:	ͨ������3������ATָ���ж��Ƿ�OK
 *@return:	�ɹ����ߴ���������󣬴���������pMyCMD->NB_Return
 */
ErrorStatus doNB_CMD(pNB MyNB, uint8_t CMD_NUM){
	uint16_t doCMD_cnt = 0;
	while((doCMD_cnt < MyNBIotCMD_Data[CMD_NUM].MaxTry)){
		send_data_to_NB(MyNB, CMD_NUM, MyNBIotCMD_Data[CMD_NUM].pCMD_Data);
		if(MyNBIotCMD_Data[CMD_NUM].NB_Return == BACK_OK){
			return SUCCESS;
		}
		else if(MyNBIotCMD_Data[CMD_NUM].NB_Return == BACK_TIMEOUT){
			DelaySecond(1);
		}
		doCMD_cnt++;
	}
	return ERROR;
}

/*
 *@name:	getCFUN
 *@author:	HL
 *@date:	2017-12-01
 *@brief:	ͨ������3������ATָ���ж��Ƿ�OK
 *@return:	�ɹ����ߴ���������󣬴���������pMyCMD->NB_Return
 */
ErrorStatus getCFUN(pNB MyNB){
	uint16_t try_cnt = 0;
	while(try_cnt < MyNBIotCMD_Data[GET_CFUN].MaxTry){
		if(doNB_CMD(MyNB, GET_CFUN) == SUCCESS){
			if(strstr(MyNB->NB_RX_BUFF, "+CFUN:1") != NULL){
				MyNB->nb_status |= 0x02;
			}
			else{
				MyNB->nb_status &= (~0x02);
			}
			return SUCCESS;
		}
		try_cnt++;
	}
	return ERROR;
}

/*
 *@name:	getNBAND
 *@author:	HL
 *@date:	2017-12-05
 *@brief:	ͨ������3������ATָ���ж��Ƿ�OK
 *@return:	�ɹ����ߴ���������󣬴���������pMyCMD->NB_Return
 */
ErrorStatus getNBAND(pNB MyNB){
	uint16_t try_cnt = 0;
	while(try_cnt < MyNBIotCMD_Data[GET_NBAND].MaxTry){
		if(doNB_CMD(MyNB, GET_NBAND) == SUCCESS){
			char *pStart = strstr(MyNB->NB_RX_BUFF, "+NBAND:");
			MyNB->nbiot_config.NBAND = pStart[7] - '0';
			return SUCCESS;
		}
		try_cnt++;
	}
	return ERROR;
}

/*
 *@name:	getICCID
 *@author:	HL
 *@date:	2017-12-05
 *@brief:	ͨ������3������ATָ���ж��Ƿ�OK
 *@return:	�ɹ����ߴ���������󣬴���������pMyCMD->NB_Return
 */
ErrorStatus getICCID(pNB MyNB){
	uint16_t try_cnt = 0;
	while(try_cnt < MyNBIotCMD_Data[GET_ICCID].MaxTry){
		if(doNB_CMD(MyNB, GET_ICCID) == SUCCESS){
			char *pStart = strstr(MyNB->NB_RX_BUFF, "+NCCID:");
			for(uint8_t i = 0; i < 20; i++){
				MyNB->nbiot_config.ICCID[i] = pStart[7+i];
			}
			return SUCCESS;
		}
		try_cnt++;
	}
	return ERROR;
}

/*
 *@name:	getIMEI
 *@author:	HL
 *@date:	2017-12-05
 *@brief:	ͨ������3������ATָ���ж��Ƿ�OK
 *@return:	�ɹ����ߴ���������󣬴���������pMyCMD->NB_Return
 */
ErrorStatus getIMEI(pNB MyNB){
	uint16_t try_cnt = 0;
	while(try_cnt < MyNBIotCMD_Data[GET_IMEI].MaxTry){
		if(doNB_CMD(MyNB, GET_IMEI) == SUCCESS){
			char *pStart = strstr(MyNB->NB_RX_BUFF, "+CGSN:");
			for(uint8_t i = 0; i < 15; i++){
				MyNB->nbiot_config.IMEI[i] = pStart[6+i];
			}
			return SUCCESS;
		}
		try_cnt++;
	}
	return ERROR;
}

/*
 *@name:	getIMSI
 *@author:	HL
 *@date:	2017-12-05
 *@brief:	ͨ������3������ATָ���ж��Ƿ�OK
 *@return:	�ɹ����ߴ���������󣬴���������pMyCMD->NB_Return
 */
ErrorStatus getIMSI(pNB MyNB){
	uint16_t try_cnt = 0;
	while(try_cnt < MyNBIotCMD_Data[GET_IMSI].MaxTry){
		if(doNB_CMD(MyNB, GET_IMSI) == SUCCESS){
			char *pStart = strstr(MyNB->NB_RX_BUFF, "\r\n\r\nOK\r\n");
			for(uint8_t i = 0; i < 15; i++){
				MyNB->nbiot_config.IMSI[i] = pStart[i-15];
			}
			return SUCCESS;
		}
		try_cnt = 0;
	}
	return ERROR;
}

/*
 *@name:	getCSQ
 *@author:	HL
 *@date:	2017-12-05
 *@brief:	ͨ������3������ATָ���ж��Ƿ�OK
 *@return:	�ɹ����ߴ���������󣬴���������pMyCMD->NB_Return
 */
ErrorStatus getCSQ(pNB MyNB){
	uint16_t try_cnt = 0;
	uint16_t tmp[2] = {0};
	while(try_cnt < MyNBIotCMD_Data[GET_CSQ].MaxTry){
		if(doNB_CMD(MyNB, GET_CSQ) == SUCCESS){
			if(strstr(MyNB->NB_RX_BUFF, "+CSQ:99,99") != NULL){
				try_cnt++;
			}
			else{
				char *pStart = strstr(MyNB->NB_RX_BUFF, "+CSQ:");
				sscanf(pStart, "+CSQ:%d,%d", &tmp[0],&tmp[1]);
				MyNB->nbiot_config.NB_RSSI = (uint8_t)tmp[0];
				MyNB->nbiot_config.NB_BER = (uint8_t)tmp[1];
				return SUCCESS;
			}
		}
		DelayMs(10);
	}
	return ERROR;
}

/*
 *@name:	auto_connect_UDP
 *@author:	HL
 *@date:	2017-12-05
 *@brief:	�ο�Quectel_BC95_AT_Commands_Manual_V1.8.pdf
 *	�������̣�
 *	AT+NBAND? 	+NBAND:8\r\n OK
 *	AT+CFUN?	+CFUN:1\r\n OK
 *	AT+CIMI		IMSI number
 * 	AT+CSGN=1	IMEI number
 *	AT+CSQ		+CSQ:21,99\r\n	OK
 *	AT+UESTATS	Signal power, SNR, PCI, RSRQ
 *	AT+CGATT?	+CGATT:1\r\n	OK
 *	AT+CEREG?	+CEREG:0,1\r\n	OK
 *	AT+CSCON?	+CSCON:0,1\r\n	OK	//��ִ�д�ָ��
 *@return:	�ɹ����ߴ���������󣬴���������	pMyCMD->NB_Return
 */
uint8_t auto_attach_network(pNB MyNB){
	uint8_t attatch_status = 0;
	if(doNB_CMD(MyNB, AT_CMD) == SUCCESS){
		attatch_status |= 0x01;	// ͨѶ����
	}
	else{
		// NBiot ģ��ͨѶ����
		attatch_status &= (~0x01);	// ������NBģ��ͨѶ
		return attatch_status;
	}
	
	doNB_CMD(MyNB, AT_NRB);
	
	MyNBIotCMD_Data[GET_CFUN].pTrueBack = "+CFUN:1";
	if(doNB_CMD(MyNB, GET_CFUN) == SUCCESS){
		attatch_status |= 0x02;	// ��Ƶ�򿪳ɹ�
	}
	else{
		// ���ܴ�ģ����Ƶ������SIM��װ����
		attatch_status &= (~0x02);	// ����Ƶʧ��
		return attatch_status;
	}
	MyNBIotCMD_Data[GET_CFUN].pTrueBack = "OK";
	
	// ����APN?
	
	// ��ȡ�ź�ǿ��
	if(getCSQ(MyNB) == SUCCESS){
		attatch_status |= 0x04;	//��վ�ź�����
	}
	else{
		// ��ʱ���ȡ������վ�ź�
		attatch_status &= (~0x04);	//��վ�ź��쳣
		return attatch_status;
	}
	
	if(doNB_CMD(MyNB, GET_CGATT) == SUCCESS){
		attatch_status |= 0x08;	//attached
	}
	else{
		// û�и��ŵ�����
		attatch_status &= (~0x08); // unattached
		return attatch_status;
	}
	
	MyNBIotCMD_Data[GET_REG_STATUS].pTrueBack = "+CEREG:0,1";
	if(doNB_CMD(MyNB, GET_REG_STATUS) == SUCCESS){
		attatch_status |= 0x10;	//register success
	}
	else{
		// ��ʱ�䲻��ע�ᵽ�ƶ�����
		attatch_status &= (~0x10); //register fail
		return attatch_status;
	}
	MyNBIotCMD_Data[GET_REG_STATUS].pTrueBack = "OK";
	
	return attatch_status;
//	if(doNB_CMD(MyNB, GET_CSCON) == SUCCESS){
//		if(strstr(MyNB->NB_RX_BUFF, "+CSCON:0,1")){
//			attatch_status |= 0x20;	// connected
//		}
//		else{
//			attatch_status &= (~0x20);	//idle
//		}
//		return attatch_status;
//	}
//	else{
//		// ����״̬
//		return attatch_status;
//	}
}

/*
 *@name:	getLocalIP
 *@author:	HL
 *@date:	2017-12-05
 *@brief:	ͨ������3������ATָ���ж��Ƿ�OK
 *@return:	�ɹ����ߴ���������󣬴���������pMyCMD->NB_Return
 */
ErrorStatus getLocalIP(pNB MyNB){
	uint8_t try_cnt = 0;
	uint16_t tmp[4] = {0};
	while(try_cnt < MyNBIotCMD_Data[GET_IP_ADDRESS].MaxTry){
		if(doNB_CMD(MyNB, GET_IP_ADDRESS) == SUCCESS){
			// successfully get the local IP address
			char *pStart = strstr(MyNB->NB_RX_BUFF, "+CGPADDR:");
			sscanf(pStart, "+CGPADDR:0,%d.%d.%d.%d",&tmp[0],&tmp[1],&tmp[2],&tmp[3]);
			MyNB->nbiot_config.local_ip[0] = (uint8_t)tmp[0];
			MyNB->nbiot_config.local_ip[1] = (uint8_t)tmp[1];
			MyNB->nbiot_config.local_ip[2] = (uint8_t)tmp[2];
			MyNB->nbiot_config.local_ip[3] = (uint8_t)tmp[3];
			return SUCCESS;
		}
		try_cnt++;
	}
	return ERROR;
}

/*
 *@name:	getNEUSTATS
 *@author:	HL
 *@date:	2017-12-05
 *@brief:	ͨ������3������ATָ���ж��Ƿ�OK
 *@return:	�ɹ����ߴ���������󣬴���������pMyCMD->NB_Return
 */
ErrorStatus getNEUSTATS(pNB MyNB){
	uint8_t try_cnt = 0;
	while(try_cnt < MyNBIotCMD_Data[GET_NEUSTATS].MaxTry){
		if(doNB_CMD(MyNB, GET_NEUSTATS) == SUCCESS){
			char *pStart = strstr(MyNB->NB_RX_BUFF, "Signal power:");
			sscanf(pStart, "Signal power:%d",&MyNB->Singal_power);
			pStart = strstr(MyNB->NB_RX_BUFF, "SNR:");
			sscanf(pStart, "SNR:%d",&MyNB->SNR);
			pStart = strstr(MyNB->NB_RX_BUFF, "RSRQ:");
			sscanf(pStart, "RSRQ:%d",&MyNB->RSSQ);
			pStart = strstr(MyNB->NB_RX_BUFF, "PCI:");
			sscanf(pStart, "PCI:%d",&MyNB->PCI);
			return SUCCESS;
		}
		try_cnt++;
	}
	return ERROR;
}

/*
 *@name:	getCGMR
 *@author:	HL
 *@date:	2017-12-05
 *@brief:	��ȡ�̼�NBiotģ��Ĺ̼��汾
 *@return:	�ɹ����ߴ���������󣬴���������pMyCMD->NB_Return
 */
ErrorStatus getCGMR(pNB MyNB){
	uint8_t try_cnt = 0;
	while(try_cnt < MyNBIotCMD_Data[GET_CGMR].MaxTry){
		if(doNB_CMD(MyNB, GET_CGMR) == SUCCESS){
			char *pStart = strstr(MyNB->NB_RX_BUFF, "SECURITY");
			for(uint8_t i = 0; i < 7; i++){
				MyNB->nbiot_soft_version[i] = pStart[20 + i];
			}
			return SUCCESS;
		}
		try_cnt++;
	}
	return ERROR;
}
/*
 *@name:	create_socket
 *@author:	HL
 *@date:	2017-12-07
 *@brief:	�ж�NB����buff״̬
 *@return:	�ɹ����ߴ���������󣬴���������pMyCMD->NB_Return
 */
ErrorStatus create_socket(pNB MyNB){
	uint8_t l_attach_status = 0;
	do{
		l_attach_status = auto_attach_network(MyNB);
	}while((l_attach_status & 0x1F) != 0x1F);
	if((l_attach_status & 0x1F) != 0x1F){
		return ERROR;
	}
	getLocalIP(MyNB);
	MyNB->nbiot_config.default_server.server_status = 1;
	if((MyNB->nbiot_config.default_server.server_status == 1) && (MyNB->nbiot_config.default_server.socket_status == 0)){
		MyNBIotCMD_Data[CREATE_SOCKET].pCMD_Data = "AT+NSOCR=DGRAM,17,9990,1\r\n";
		if(doNB_CMD(MyNB, CREATE_SOCKET) == SUCCESS){
			MyNB->nbiot_config.default_server.socket_status = 1;	// successfully create socket for default server
		}
		else{
			MyNBIotCMD_Data[CLOSE_SOCKET].pCMD_Data = "AT+NSOCL=0\r\n";
			doNB_CMD(MyNB, CLOSE_SOCKET);
			return ERROR;
		}
	}
	if((MyNB->nbiot_config.server1.server_status == 1) && (MyNB->nbiot_config.server1.socket_status == 0)){
		MyNBIotCMD_Data[CREATE_SOCKET].pCMD_Data = "AT+NSOCR=DGRAM,17,9991,1\r\n";
		if(doNB_CMD(MyNB, CREATE_SOCKET) == SUCCESS){
			MyNB->nbiot_config.server1.socket_status = 1;	// successfully create socket for default server
		}
		else{
			MyNBIotCMD_Data[CLOSE_SOCKET].pCMD_Data = "AT+NSOCL=1\r\n";
			doNB_CMD(MyNB, CLOSE_SOCKET);
			return ERROR;
		}
	}
	if((MyNB->nbiot_config.server2.server_status == 1) && (MyNB->nbiot_config.server2.socket_status == 0)){
		MyNBIotCMD_Data[CREATE_SOCKET].pCMD_Data = "AT+NSOCR=DGRAM,17,9992,1\r\n";
		if(doNB_CMD(MyNB, CREATE_SOCKET) == SUCCESS){
			MyNB->nbiot_config.server2.socket_status = 1;	// successfully create socket for default server
		}
		else{
			MyNBIotCMD_Data[CLOSE_SOCKET].pCMD_Data = "AT+NSOCL=2\r\n";
			doNB_CMD(MyNB, CLOSE_SOCKET);
			return ERROR;
		}
	}
	return SUCCESS;
}
/*
 *@name:	judge_NBIot_buffer
 *@author:	HL
 *@date:	2017-12-05
 *@brief:	�ж�NB����buff״̬
 *@return:	�ɹ����ߴ���������󣬴���������pMyCMD->NB_Return
 */
void judge_NBIot_buffer(pNB MyNB){
	if(strstr(MyNB->NB_RX_BUFF_TMP, MyNBIotCMD_Data[MyNB->current_cmd_num].pTrueBack) != NULL){
		memcpy(MyNB->NB_RX_BUFF, MyNB->NB_RX_BUFF_TMP, NB_BUFFER_SIZE);
		memset(MyNB->NB_RX_BUFF_TMP, 0, NB_BUFFER_SIZE);
		MyNBIotCMD_Data[MyNB->current_cmd_num].NB_Return = BACK_OK;
	}
	else if(strstr(MyNB->NB_RX_BUFF_TMP, "+NSONMI:") != NULL){
		// �з�������
		memcpy(MyNB->NB_RX_BUFF, MyNB->NB_RX_BUFF_TMP, NB_BUFFER_SIZE);
		memset(MyNB->NB_RX_BUFF_TMP, 0, NB_BUFFER_SIZE);
		MyNB->NBIotRecv.NBIotRecvBuffFlag = NBIotRecvBuffNotEmpty;
		char *pStart = strstr(MyNB->NB_RX_BUFF, "+NSONMI:");
		MyNB->NBIotRecv.socket_num = pStart[8];
		MyNB->NBIotRecv.length = pStart[10];
	}
	else if(strstr(MyNB->NB_RX_BUFF_TMP, "ERROR") != NULL){
		MyNBIotCMD_Data[MyNB->current_cmd_num].NB_Return = BACK_ERROR;
	}
}