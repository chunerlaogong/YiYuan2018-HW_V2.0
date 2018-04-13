#ifndef __NB_H_
#define __NB_H_
#include "Type.h"

#define OK                   "\r\nOK\r\n"
#define ERROR                "\r\nERROR\r\n"
#define ERROR_CODE           "\r\n+CME ERROR:"
#define END                  "\r\n"
#define NSONMI               "+NSONMI:"                             //新消息指示

#define NET_TYPE             "UDP"


//#define NOT_ME
//#ifdef  NOT_ME
#define SERVER_IP            "118.122.93.190"  //  "120.77.159.60"//                   //UDP发送数据远端IP
#define SERVER_PORT          "7788"//   "13880" //                           //UDP发送数据端口
//#else
#define SERVER_IP_1            "118.122.93.190"  // 
#define SERVER_PORT_1          "30001" // 
//#endif


//************************************************************未用到部分********************************************************************
#define Config_CDP           "AT+NCDP=192.168.0.7,5683\r\n"         //配置CDP服务器IP和端口
#define Config_APN           "AT+CGDCONT=1,\"IP\",\"ctnet\"\n"    /*配置APN（Access Point Name）,ctnet为中国电信，cmnet为中国移动
                                                                    AT+CGDCONT=<cid>,<PDP_type>,<APN>
                                            Only <PDP_type>=“IP” is supported. <PDP_type>=“IPV6” will be supported in a future release.
                                            1)  Neul supports +CGDCONT=<cid>,<PDP_type>,<APN> only.
                                            2)  <cid> values of 0-10 are supported.
                                            3)  <cid>=0 is read only and is only defined when AUTOCONNECT is enabled.*/
                                            

#define Config_PLMN          "AT+COPS=0\r\n"                              /*配置PLMN
                                            1)  <Act>, if provided, must be set to 7.
                                            2)  Only <format>=2 is supported.
                                            3)  Only <mode>=0, <mode>=1 & <mode>=2 are supported.
                                            4)  When <mode>=1, the PLMN setting will not be retained after the UE is rebooted.
                                            5)  <mode>=1 is only for development use and <mode>=0 should be used in production when
                                                AUTOCONNECT is enabled.
                                            6)  The test command currently returns the configured values rather than 
                                               performs a PLMN search.*/ 
//******************************************************************************************************************************************

#define STEP_AT_CFUN0         0
#define STEP_AT_CGSN          1
#define STEP_AT_NBR           2
#define STEP_AT_CFUN1         3
#define STEP_AT_CIMI          4
#define STEP_AT_CMEE          5
#define STEP_AT_CGDCONT       6
//#define STEP_AT_CEREG         6
#define STEP_AT_NNMI          7
#define STEP_AT_CEREG         8
#define STEP_AT_CSCON         9
#define STEP_AT_CGATT         10
#define STEP_AT_CEREG1        11
//#define STEP_AT_CSCON1        10                                                 
#define STEP_AT_CGPADDR       11+1
#define STEP_AT_NUESTATS      12+1
#define STEP_AT_NSOCL         21
#define STEP_AT_NSOCR         21
#define STEP_AT_NSOST         15+1
#define STEP_AT_NSONMI        15
#define STEP_AT_NSORF         21//17+1
#define STEP_AT_CSQ           8   //获取信号强度
#define STEP_AT_CLEAN         19+1
#define STEP_AT_QUERY_CSCON   20+1
#if COAP_TEST
//#define 
#define STEP_AT_SET_CDPSERVER 20+2  //配置和查询CDP服务器设置


#define NO_BACK               0
#define BACK_OK               1
#define BACK_ERROR            2
#define BACK_TIMEOUT          3
#define NEW_MESSAGE           4

#define QueryCGATT				10

											   
// the following lines created by HL

#define 	INIT_NB_START		0
#define 	INIT_NB_END			12
#define		GET_CFUN			2
#define		GET_NBAND			4
#define		GET_CGSN			6			
#define		GET_CIMI			7
#define 	GET_CSQ				8
#define		GET_CEREG			9
#define		GET_CGPADDR			10	
#define		GET_CGATT			12
#define		GET_INTERNETTIME	15

typedef enum
{
  Net_type_UDP,
  Net_type_TCP,
  Net_type_COAP
}Net_type;                                                 

//typedef struct NB_CMD_DATA
//{
//  char *cmd;
//  char revtimeout;
//  char *cmdtrueback;
//  char revresult;
////  char rty_max;
//  unsigned int rty_max;
//}NB_CMD_DATA;

typedef struct NB_parameter
{
             char  net_type[4];
             char  server_ip[15];
             char  server_port[5];
   unsigned char  socket[1];                   //创建的网络接口
   unsigned char  current_cmd;
   unsigned char  cmd_retry_max;
   unsigned short cmd_time_cnt;
}*pNB_parameter,sNB_parameter;

typedef struct From_NB
{
  unsigned char NBreci_newMsg;
  unsigned char NBreci_pointer;
  //unsigned char NBresp_result;
  unsigned char NBsignal_strength;
  unsigned char NBtime_cnt;
            char NB_Msg_indicate[7];
            char NB_RxTemp[120];
            char NB_RxBuffer[120];
            char NB_TxTemp[120];
            char NB_TxBuffer[120];                //发送到NB模块的数据，包括socket，IP，port，length，data
  sNB_parameter   NB_param;
}*pFrom_NB,sFrom_NB;

//extern sFrom_NB NB_BC95;
extern void send_data(char *d,unsigned char length);
//extern unsigned char send_data_to_NB(pFrom_NB NB_module,unsigned char CMD_num,char *fmt,...);
extern void delay(U32);
#endif