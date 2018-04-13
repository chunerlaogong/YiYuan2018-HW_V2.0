/*
 * @file: 	MyNBIOT.h
 *@author:	HL
 *@date:	2017-12-18
 *@version:	V1.0
 */

#ifndef __My_NBIOT_H
#define __My_NBIOT_H

/* defines */
#define USART_FOR_NB		USART3

#define NB_SEND_BUFFER_SIZE				120
#define NB_RECEIVE_BUFFER_SIZE			150
#define NB_ERROR_CODE_SIZE				15
#define NB_AT_TRUE_BACK_SIZE			20

#define NB_RECV_TIMEOUT						5000

#define DEFAULT_SERVER_ADDR			"120.79.207.24:8888"//"118.122.93.190:30001"
//#define DEFAULT_SERVER_ADDR			"118.122.93.190:7788"

#define NBIOT_CMD				0
#define AT_CMD					1
#define AT_NRB					2
#define AT_CMEE					3
#define AT_CFUN					4
#define AT_NBAND				5
#define	AT_NCCID				6
#define AT_CGSN					7
#define AT_CIMI					8
#define AT_CSQ					9
#define AT_CEREG				10
#define AT_CGPADDR				11
#define AT_CCLK					12
#define AT_CSCON				13
#define AT_CGATT				14
#define AT_NUESTATS				15
#define AT_CGMR					16
#define AT_NSOCR				17
#define AT_NSOST				18
#define AT_NSORF				19
#define AT_NSOCL				20
#define AT_NSOCR1				21
#define AT_NSOCR2				22
#define AT_NSOCR3				23
#define AT_CDPSERVERCFG         24

/* typedefs */
typedef struct NB_CMD_DATA		// NB 指令结构
{
	char *pCMD_Data;
	uint16_t MaxTry;
	uint16_t timeout_s;
	char *pTrueBack;
	RETURN_STATUS NB_return;
//	uint8_t error_num;
	uint16_t interval_ms;
}sNB_CMD,*pNB_CMD;

typedef struct NB_SERVER
{
	uint8_t ip[4];
	uint16_t port;
	uint8_t status;		// bit7:开关	// bit6: status
}sNB_SERVER,*pNB_SERVER;

typedef struct NB_RECEIVE
{
		uint8_t receive_status;	//bit7: 接收开关 bit6:接收状态　bit5~bit3:socket_num
		uint16_t length;
		uint32_t receive_cnt;
		uint8_t receive_buff[NB_RECEIVE_BUFFER_SIZE];
		char receive_buff_tmp[NB_RECEIVE_BUFFER_SIZE];
}sNB_RECEIVE,*pNB_RECEIVE;


typedef struct NB_MODULE
{
	int16_t RSSQ;
	int16_t SNR;
	int16_t singnal_power;
	int16_t PCI;
	char nb_module_ver[8];
}sNB_MODULE,*pNB_MODULE;

typedef struct NB
{
	char imei_code[15];
	char imsi_code[15];
	char iccid[20];
	uint8_t NBAND;
	uint8_t local_IP[4];
	uint8_t network_status;	// bit7: connect status
	uint8_t module_status;
	uint16_t tx_cnt;
	uint16_t rx_cnt;
	uint8_t current_cmd_num;
	uint32_t current_cmd_cnt;	//该条指令执行次数
	uint32_t current_cmd_time;
	uint8_t error_status;
	uint8_t error_cmd_num;
	char error_info[20];
	uint8_t rssi;			//
	uint8_t ber;			//
	uint8_t sleep_status;
	uint32_t sleep_cnt;
	uint32_t sleep_end_cnt;
	char Rx_buff[NB_RECEIVE_BUFFER_SIZE];
	char Rx_buff_tmp[NB_RECEIVE_BUFFER_SIZE];
	char Tx_buff[NB_SEND_BUFFER_SIZE];
	char Tx_buff_tmp[NB_SEND_BUFFER_SIZE];
	sNB_RECEIVE nb_recv_data;
	sNB_SERVER default_server;
	sNB_SERVER server1;
	sNB_SERVER server2;
}sNB,*pNB;

typedef struct
{
	char ATCMDData[NB_SEND_BUFFER_SIZE];
	char ATReturn[NB_RECEIVE_BUFFER_SIZE];
	uint16_t MaxTryCnt;
	uint32_t TryMaxTimeMs;
	char ATTrueBack[NB_AT_TRUE_BACK_SIZE];
	RETURN_STATUS NBReturn;
	uint32_t IntervalMs;
	char ErrorCode[NB_ERROR_CODE_SIZE];
	uint8_t TxCnt;
	uint8_t RxCnt;
	uint8_t CurrentCmdNum;
	uint32_t CurrentCmdTime;
	uint8_t NetworkStatus;
	uint8_t SleepFlag;
	uint8_t NBEnableFlag;
	uint8_t ATFromBTFlag;
}sNBiotAT,*pNBiotAT;

typedef enum
{
	JUDGE_NONE = 0,
	JUDGE_AND = 1,
	JUDGE_NOT = 2,
	JUDGE_OR = 3,
	JUDGE_XOR = 4
} JUDGE_OPTION;

/* functions declaration */
//void judge_NBIOT_Rx_Buff(pNB pMyNB);

void judge_NBIOT_Rx_Buff(pNBiotAT pMyNBAT);
ErrorStatus do_NB_CMD(pNB pMyNB, uint8_t cmd_num);
RETURN_STATUS send_data_to_NB(pNB pMyNB, uint8_t cmd_num, char *fmt,...);
ErrorStatus getCSQ(pNB pMyNB);
uint8_t check_NB_network(pNB pMyNB);
void check_socket_status(pNB pMyNB);
ErrorStatus checkNBRecvBuff(pNB pMyNB);
void ASCII_to_Hex(char *ascii,uint8_t *hex,unsigned char len);
uint8_t MyPrintfToString(char *ptr, char *fmt,...);
uint8_t CopyString(char *pTo,char *pFrom);
RETURN_STATUS SendDataToNB(pNBiotAT pMyNBAT);
ErrorStatus DoNBiotATCmd(pNBiotAT pMyNBAT, uint8_t cmd_num, JUDGE_OPTION option, char *ptr, char *fmt,...);
void checkNBNetwork(pNBiotAT pMyNBAT);
void DoATCmdFromBT(pNBiotAT pMyNBAT);
void PowerConfig(void);
#endif