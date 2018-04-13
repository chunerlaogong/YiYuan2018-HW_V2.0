/*
 * @file: 	NBIot.h
 *@author:	HL
 *@date:		2017-11-23
 *@version:	V1.0
 */

#ifndef __NBIot_h
#define __NBIot_h

/* defines */
#define NB_BUFFER_SIZE		180
#define USART_FOR_NB	USART3		//NBIot uses USART3

/* server's information */
#define DEFAULT_SERVER		"118.122.93.190"
#define DEFAULT_SERVER_PORT	30003

#define NBIOT_CMD				0
#define AT_CMD					1
#define AT_NRB					2
#define GET_CFUN				3
#define GET_NBAND				4
#define GET_ICCID				5		// get the ICCID of the sim card
#define GET_IMEI				6		// get the IMEI code of NBIOT module
#define GET_IMSI				7		// get the IMSI code
#define GET_CSQ					8		// get the signal strength
#define GET_REG_STATUS			9		// registration status
#define GET_IP_ADDRESS			10		// get the local IP address
#define GET_INTERNET_TIME		11		// get the Internet time
#define GET_CSCON				12		// 0:idle 1:connected
#define GET_CGATT				13		// 0:unattached 1:attached
#define GET_NEUSTATS			14		// get RSRQ and SNR
#define GET_CGMR				15		// get nbiot module's software version
#define CREATE_SOCKET			16		// create socket with IP address and Port
#define SEND_UPD_MESSAGE		17		// send udp message
#define READ_UDP_MESSAGE		18		// receive udp message
#define CLOSE_SOCKET			19		// close socket


/* typedefs */
typedef struct NBIot_RecvStatus{
	NBIotRecvBuffStatus NBIotRecvBuffFlag;
	uint8_t socket_num;
	uint16_t length;	// not more than 512 bytes
}sNBIot_RecvStatus,*pNBIot_RecvStatus;

typedef struct{
	char *pCMD_Data;				// 指令内容
	uint16_t MaxTry;				// 最大尝试次数
	uint16_t timeout;				// unit: second
	char *pTrueBack;				// 指令完成正确返回
	FunctionalState isReturn;		// 是否返回
	RETURN_STATUS NB_Return; 
	char *pReturn;
}sNBCMD,*pNBCMD;

typedef struct {
	uint8_t SERVER_IP_ADDR[4];
	uint16_t SERVER_PORT;
	uint8_t	server_status;	//是否开启 0:关闭 1:开启
	uint8_t socket_status;	//是否创建socket成功
}sServerAddr,*pServerAddr;

typedef struct {
	sServerAddr default_server;
	sServerAddr server1;
	sServerAddr server2;
	char ICCID[20];		//sim card's ICCID
	char IMEI[15];		// AT+CGSN=1
	char IMSI[15];		// AT+CIMI
	uint8_t local_ip[4];
	uint8_t NB_RSSI;	//信号强度
	uint8_t NB_BER;		//误码率
	uint8_t NBAND;			// AT+NBAND? 得到运营商
}sNBIOT_CONFIG,*pNBIOT_CONFIG;


typedef struct {
//	sServerAddr default_server;
//	sServerAddr server1;
//	sServerAddr server2;
//	uint8_t ICCID[20];		//sim card's ICCID
//	uint8_t IMEI[16];		// AT+CGSN=1
//	uint8_t IMSI[16];		// AT+CIMI
//	uint8_t NBAND;			// AT+NBAND? 得到运营商
//	uint8_t local_ip[4];
	int16_t RSSQ;
	int16_t SNR;
	int16_t Singal_power;
	int16_t PCI;
	char nbiot_soft_version[8];
	sNBIOT_CONFIG nbiot_config;
	char NB_TX_BUFF[NB_BUFFER_SIZE];
	char NB_TX_BUFF_TMP[NB_BUFFER_SIZE];
	char NB_RX_BUFF[NB_BUFFER_SIZE];
	char NB_RX_BUFF_TMP[NB_BUFFER_SIZE];
	uint8_t RX_CNT;
	uint8_t TX_CNT;
	uint8_t current_cmd_num;
	uint32_t current_cmd_cnt;
	uint32_t current_cmd_time;
	sNBIot_RecvStatus NBIotRecv;
	
	uint16_t nb_status;
	// 从低位到高位依次表示:串口是否正常、射频是否打开、是否信号正常、附着状态、注册状态、连接状态、
	// 							0x01		  0x02			0x04		0x08	  0x10		0x20
	
}sNB,*pNB;

/* functions declaration */
RETURN_STATUS send_data_to_NB(pNB MyNB, uint8_t CMD_NUM, char *fmt,...);
ErrorStatus doNB_CMD(pNB MyNB, uint8_t CMD_NUM);
void judge_NBIot_buffer(pNB MyNB);
ErrorStatus getCFUN(pNB MyNB);
ErrorStatus getNBAND(pNB MyNB);
ErrorStatus getICCID(pNB MyNB);
ErrorStatus getIMEI(pNB MyNB);
ErrorStatus getIMSI(pNB MyNB);
ErrorStatus getCSQ(pNB MyNB);
uint8_t auto_attach_network(pNB MyNB);
ErrorStatus getLocalIP(pNB MyNB);
ErrorStatus getNEUSTATS(pNB MyNB);
ErrorStatus getCGMR(pNB MyNB);
ErrorStatus create_socket(pNB MyNB);

#endif