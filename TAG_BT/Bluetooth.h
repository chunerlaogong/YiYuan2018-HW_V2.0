/*
 * @file:	Bluetooth.h
 * @author:	HL
 * @date:	2017-11-23
 * @version:V1.0	
*/

#ifndef	__Bluetooth_h
#define __Bluetooth_h

/* defines */
#define	USART_FOR_BT	USART1
#define BT_BUFF_SIZE		150		// not more than 255
#define GET_TRY_MAX_CNT		3
#define SET_TRY_MAX_CNT		3

#define TEST_BT				0
#define GET_BT_VERSION		1
#define GET_BT_MAC_ADDR		2
#define GET_BT_NAME			3
#define GET_BT_BAUD			4
#define GET_BT_STOP			5
#define GET_BT_PARI			6
#define GET_BT_MODE			9
#define GET_BT_POWE			13
#define BT_CMD				14

/* typedefs */
typedef enum 
{
	INIT_READY = 0,
	ADVERTISING = 1,
	CONNECTED = 2,
	DISCONNECTED = 3,
	STANDBY = 4,
	CENTRAL_SCAN = 5,
	CENTRAL_CON = 6,
	CENTRAL_DISCON = 7
} BT_MODE_STATUS;

typedef enum
{
	AT_MODE = 0,
	DATA_MODE = 1
} BT_RX_MODE;

typedef struct
{
	uint8_t fram_prefix0;
	uint8_t fram_prefix1;
	uint8_t command_code;
	uint8_t fram_length;
	uint8_t protocol_version;
	uint8_t command_data[150];
}sBT_COMMAND, *pBT_COMMAND;

typedef struct {
	char BT_Version[6];	//BT version default:V1.0
	char MyBT_Address[12];	//BT address MAC 地址
	char MyBT_Name[20];	//BT name default:BRT DATA
//	char OtherBTaddr[17];	//主模式下从机MAC地址
//	char OtherBTname[15];	//主模式下从机name
	uint8_t BT_RX_FLAG;
//	uint8_t BT_RX_MODE;
	BT_RX_MODE BT_rx_mode;
	uint8_t	BT_RX_CNT;
	uint8_t BT_TX_CNT;
	uint8_t BTEnableFlag;
	BT_MODE_STATUS BT_mode_status;
	uint8_t current_cmd_num;
	uint32_t current_cmd_time;
	char BT_TX_BUFF_TMP[BT_BUFF_SIZE];
	char BT_TX_BUFF[BT_BUFF_SIZE];
	char BT_RX_BUF_TMP[BT_BUFF_SIZE];
	char BT_RX_BUF[BT_BUFF_SIZE];
}sBT,*pBT;

typedef struct{
	char *pCMD_Data;				// 指令内容
	uint16_t MaxTry;				// 最大尝试次数
	uint16_t timeout;				// unit: second
	char *pTrueBack;				//指令完成正确返回
	FunctionalState isReturn;		// 是否返回
	RETURN_STATUS BT_Return;
	char *pReturn;
}sBTCMD,*pBTCMD;

/* functions declaration */

ErrorStatus doBT_CMD(pBT MyBT, uint8_t CMD_NUM);
ErrorStatus test_BT(pBT MyBT);
void judge_BT_buffer(pBT MyBT);
ErrorStatus get_BT_Version(pBT MyBT);
ErrorStatus get_BT_Address(pBT MyBT);
ErrorStatus get_BT_Name(pBT MyBT);
ErrorStatus get_BT_Baudrate(pBT MyBT);
ErrorStatus get_BT_Power(pBT MyBT);
ErrorStatus get_BT_Mode(pBT MyBT);
ErrorStatus set_BT_Name(pBT MyBT, char *pName);
//ErrorStatus set_BT_Mode(pBT MyBT, BTModeStatus MyBTmode);
ErrorStatus deal_BT_data(pBT MyBT);
void enter_Bootloader(void);

#endif