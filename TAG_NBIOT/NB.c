/********************************************
*Copyright(C),2017,XGSG,Company
*Filename:NB.c
*Author: CODER_GJL
*Version:V1.0
*Date:2017/06/19
*Description:
********************************************/
//#include <stdint.h>
#include <string.h>             //
#include <stdio.h>              //
#include <stdarg.h>             //va_list,va_start,va_end
#include <stdbool.h>            //bool变量在此库中
#include <stdlib.h>             //atoi函数在此库中(ASCII TO Integer)
#include "NB.h"
#include "struct.h"
#include "UART.h"
//#include "stm8l15x_conf.h"

void NB_init(pFrom_NB NB_module);
void copy_data(unsigned char *sour,unsigned char *dest,unsigned char length);
void NB_intiparameter(pFrom_NB NB_module);
void init_UDP(pFrom_NB NB_module,unsigned short port);
void send_data( char *d,unsigned char length);
void creat_UDPsocket(pFrom_NB NB_module,unsigned short port);
void Hex_to_ASCII(char *hex,char *ascii,unsigned char len);
char char_convert_hex(char *ascii);
void ASCII_to_Hex(char *ascii,char *hex,unsigned char len);
void Get_signal_strength(pFrom_NB NB_module);
void clean_NBbuffer(pFrom_NB NB_module);
void read_NB_message(pFrom_NB NB_module,Pcontroler_Symple TagCng);
unsigned char query_receive_data(pFrom_NB NB_module,Pcontroler_Symple TagCng);
bool find_imsi(unsigned char len);
bool find_imei(unsigned char len);
unsigned char send_data_to_NB(pFrom_NB NB_module,unsigned char CMD_num,char *fmt,...);
unsigned char judge_response(char *n);
Net_type Get_net_type(pFrom_NB NB_module);
Net_type net_type;

unsigned char IMEI_code[16]={0};
unsigned char IMSI_code[16]={0};

volatile NB_CMD_DATA nbiot_cmd_data[23]=
{
    //cmd,     							timeout(s), 	  trueback,		revresult, 		rty_max
  {"AT+CFUN=0\r\n",  						5,       		"OK",		NO_BACK,			3},
  {"AT+CGSN=1\r\n",  						3,       		"OK",		NO_BACK,			3}, 
  {"AT+NRB\r\n",     						10,      		"OK",		NO_BACK,			1},
  {"AT+CFUN=1\r\n",  						8,      		"OK",		NO_BACK,			3}, 
  {"AT+CIMI\r\n",    						3,        		"OK",		NO_BACK,			3}, 
  {"AT+CMEE=1\r\n",  						2,        		"OK",		NO_BACK,			3},
  {"AT+CGDCONT=1,\"IP\",\"ctnb\"\r\n",  	2,  			"OK",		NO_BACK,			3},
  {"AT+NNMI=1\r\n",  						3,        		"OK",		NO_BACK,			3},
  {"AT+CEREG=1\r\n",  						2,       		"OK",		NO_BACK,			3},
  {"AT+CSCON=1\r\n",  						3,        		"OK",		NO_BACK,			3},
  {"AT+CEREG?\r\n",  						2,       	"+CEREG:1,1",	NO_BACK,			15},
  {"AT+CGPADDR=1\r\n", 						4,   		"+CGPADDR:1," ,	NO_BACK,			30}, 
  {"AT+NUESTATS\r\n" ,  					3,        		"OK",		NO_BACK,			1},
  {"AT+CCLK?\r\n",  						2,        		"OK",		NO_BACK,			3}, 
 // {"AT+NSOCL=1",  						2,        		"OK",		NO_BACK,			1}, 
  {"CMDSTR_NOUSE",  						2,         		"OK",		NO_BACK,			1}, //AT+NSOCR  CMD
  {"CMDSTR_NOUSE",  						2,        		"OK",		NO_BACK,			1},//NB_SEND_DATA  CMD
  {"CMDSTR_NOUSE",  						8,       		"NSONMI",	NO_BACK,			1},//NB_SEND_DATA with ack  CMD
  {"CMDSTR_NOUSE",  						3,       		"OK",		NO_BACK,			1},//REV_NB_DATA CMD  NB_AT_NSORF
  {"CMDSTR_NOUSE",  						3,       		"+CSQ:",	NO_BACK,			1},//get signal strength
  {"CMDSTR_NOUSE",  						2,       		"OK",   	NO_BACK,			1},
  {"CMDSTR_NOUSE",  						2,       		"OK",   	NO_BACK,			1}
};

/**
  * init the NB module set
  * By HL at Sep.21th 2017
  */
volatile NB_CMD_DATA nbiot_cmd_data1[22]=
{
	// cmd 										timeout(s)			trueback			recvresult		try_max_times
	{"AT\r\n",										3,				"OK",				NO_BACK,		3},
//		{"AT+CGMR\r\n",								3,				"OK",				NO_BACK,		3},
	{"AT+NRB\r\n",									10,				"OK",				NO_BACK,		3},
	{"AT+CFUN?\r\n",								3,				"OK",				NO_BACK,		20},
	{"AT+CFUN=1\r\n",								5,				"OK",				NO_BACK,		3},
	{"AT+NBAND?\r\n",								3,				"OK",				NO_BACK,		3},
	{"AT+NBAND=5\r\n",								3,				"OK",				NO_BACK,		3},
//	{"AT+NBAND=900\r\n",							3,				"OK",				NO_BACK,		3},
//	{"AT+NTSETID=1,863703032302486\r\n",			3,				"OK",				NO_BACK,		3},
	{"AT+CGSN=1\r\n",								3,				"OK",				NO_BACK,		3},
	{"AT+CIMI\r\n",									3,				"OK",				NO_BACK,		3},
//	{"AT+NCCID\r\n",								3,				"OK",				NO_BACK,		3},
	{"AT+CSQ\r\n",									5,				"OK",				NO_BACK,		100},
	{"AT+CEREG?\r\n",								5,				"OK",				NO_BACK,		80},
	{"AT+CGPADDR\r\n",								5,				"OK",				NO_BACK,		10},
	{"AT+NSOCR=DGRAM,17,8888,1\r\n",				3,				"OK",				NO_BACK,		3},
	{"AT+NSOCR=DGRAM,17,9999,1\r\n",				3,				"OK",				NO_BACK,		3},
	
	{"AT+CGDCONT=1,\"IP\",\"ctnb\"\r\n",			5,				"OK",				NO_BACK,		3},
	{"AT+CGATT?\r\n",								3,				"OK",				NO_BACK,		10},
	
	{"AT+CCLK?\r\n",  								3,         		"OK",				NO_BACK,		1}, //AT+NSOCR  CMD
	{"CMDSTR_NOUSE",  								2,        		"OK",				NO_BACK,		1},//NB_SEND_DATA  CMD
	{"CMDSTR_NOUSE",  								8,       		"NSONMI",			NO_BACK,		1},//NB_SEND_DATA with ack  CMD
	{"CMDSTR_NOUSE",  								3,     			"OK",				NO_BACK,		1},//REV_NB_DATA CMD  NB_AT_NSORF
	{"CMDSTR_NOUSE",  								3,       		"+CSQ:",			NO_BACK,		1},//get signal strength
	{"CMDSTR_NOUSE",  								2,       		"OK",   			NO_BACK,		1},
	{"CMDSTR_NOUSE",  								2,       		"OK",   			NO_BACK,		1},
//	{"CMDSTR_NOUSE",  								2,       		"OK",   			NO_BACK,		1},
//	{"CMDSTR_NOUSE",  								2,       		"OK",   			NO_BACK,		1}	// common AT command No. 22
};

bool is_connected(pFrom_NB NB_module){
	unsigned char l_try_times = 0;
	while(l_try_times++ < 3){
		send_data_to_NB(NB_module, QueryCGATT, nbiot_cmd_data1[QueryCGATT].cmd);
		if(nbiot_cmd_data1[QueryCGATT].revresult == BACK_OK){
			if(nbiot_cmd_data1[QueryCGATT].cmdtrueback == "+CGATT:1"){
				return true;
			}
			else{
				// deattach net,so reconnect to net
			}
		}
	}
	return false;
}

/********************************************
*Name:NB_init
*Description:NB模块初始化NB模块配置
*Input：NULL
*Output：NULL
*Author：GJL
*Date:2017/06/19
********************************************/
void NB_init(pFrom_NB NB_module)
{
	Net_type type;
	NB_intiparameter(NB_module);
	type = Get_net_type(NB_module); 
	if(type == Net_type_UDP)
	{
		init_UDP(NB_module,atoi(NB_module->NB_param.server_port));  //atoi函数在stdlib.h库中(ASCII TO Integer),字符串转化为整型数
	}
	else if(type == Net_type_TCP)
	{
		
	}else if( type == Net_type_COAP)
	{
		
	}
}


/********************************************
*Name:copy_data
*Description:数据拷贝
*Input：unsigned char *sour, unsigned char *dest, unsigned char length
*Output：NULL
*Author：GJL
*Date:2017/06/19
********************************************/
void copy_data(unsigned char *sour,unsigned char *dest,unsigned char length)
{
	unsigned char i;
	for(i=0;i<length;i++)
	{
		*dest++=*sour++;
	}
}

/********************************************
*Name:NB_intiparameter
*Description:初始化网络参数
*Input：NULL
*Output：NULL
*Author：GJL
*Date:2017/06/20
********************************************/
void NB_intiparameter(pFrom_NB NB_module)
{
	strcpy(NB_module->NB_param.net_type,NET_TYPE);
	strcpy(NB_module->NB_param.server_ip,SERVER_IP);
	strcpy(NB_module->NB_param.server_port,SERVER_PORT);
}

/********************************************
*Name:Get_net_type
*Description:获取网络类型
*Input：NULL
*Output：net_type
*Author：GJL
*Date:2017/06/20
********************************************/
Net_type Get_net_type(pFrom_NB NB_module)
{
	char *type = NB_module->NB_param.net_type;
	if(strstr(type,"UDP")!=NULL)
	{
		net_type = Net_type_UDP;
	}
	else if(strstr(type,"TCP")!=NULL)
	{
		net_type = Net_type_TCP;
	}
	else if(strstr(type,"COAP")!=NULL)
	{
		net_type = Net_type_COAP;
	}
	return net_type;
}

/********************************************
*Name:creat_UDPsocket
*Description:创建UDPsocket
*Input：十进制端口号
*Output：NULL
*Author：GJL
*Date:2017/06/20
********************************************/
void creat_UDPsocket(pFrom_NB NB_module,unsigned short port)
{
 //send_data_to_NB(NB_module,STEP_AT_NSOCL,"AT+NSOCL=%d\r\n",socket_num);
 while(1)
 {
  //send_data_to_NB(NB_module,STEP_AT_NSOCR,"AT+NSOCR=DGRAM,17,%u,1\r\n",port);
  send_data_to_NB(NB_module,STEP_AT_NSOCR,"AT+NSOCR=DGRAM,17,%s,1\r\n",SERVER_PORT);
  if(nbiot_cmd_data1[STEP_AT_NSOCR].revresult == BACK_OK)//(NB_module->NBresp_result == BACK_OK)
  {
   break;
  }
 }
}

/********************************************
*Name:UDP_config
*Description:UDP配置、连网
*Input：pFrom_NB NB_module（NB结构体指针）
*Output：NULL
*Author：GJL
*Date:2017/06/20
********************************************/
/**
  * modified by HL on Sep.21th 2017
  */
void UDP_config(pFrom_NB NB_module)
{
	unsigned char i;
	unsigned int retry_cnt=0;
	for(i=INIT_NB_START;i<=INIT_NB_END;)//STEP_AT_CSCON1;)//
	{
		send_data_to_NB(NB_module,i,nbiot_cmd_data1[i].cmd);
		if(nbiot_cmd_data1[i].revresult != BACK_OK)//(NB_module->NBresp_result != BACK_OK)
		{
			retry_cnt++;
			if(retry_cnt == nbiot_cmd_data1[i].rty_max)
			{
				retry_cnt = 0;
				i = 0;
			} 
		}
		else if(nbiot_cmd_data1[i].revresult == BACK_OK)//(NB_module->NBresp_result == BACK_OK)
		{
			if(i == GET_CGSN)
			{
				if(find_imei((unsigned char)strlen(NB_module->NB_RxBuffer)) == true)
				{
					i++;
					retry_cnt = 0;
				}
				else
				{
					retry_cnt++;
					if(retry_cnt == nbiot_cmd_data1[i].rty_max)
					{
						retry_cnt = 0;
						i++;
					} 
				}
			}
			else if(i == GET_CIMI)
			{
				if(find_imsi((unsigned char)strlen(NB_module->NB_RxBuffer)) == true)
				{
					i++;
					retry_cnt = 0;
				}
				else
				{
					retry_cnt++;
					if(retry_cnt == nbiot_cmd_data1[i].rty_max)
					{
						retry_cnt=0;
						i++;
					}     
				}
			}
			else if(i == GET_CFUN){
				if(strstr(NB_module->NB_RxBuffer,"+CFUN:1") != NULL){
					i = i + 2;
					retry_cnt=0;
				}
				else{
					retry_cnt++;
					delay(1000);
				}
			}
			else if(i == GET_NBAND){
				if(strstr(NB_module->NB_RxBuffer,"+NBAND:5") != NULL){
					i = i + 2;
					retry_cnt=0;
				}
				else{
					retry_cnt++;
				}
			}
			else if(i == GET_CSQ){
				if(strstr(NB_module->NB_RxBuffer,"+CSQ:99,99") != NULL){
					retry_cnt++;
					delay(1000);
				}
				else{
					retry_cnt=0;
					i++;
				}
				if(retry_cnt == nbiot_cmd_data1[i].rty_max){
					i = 0;
					retry_cnt = 0;
				}
			}
			else if(i == GET_CEREG){
				if(strstr(NB_module->NB_RxBuffer,"+CEREG:0,1") != NULL){
					i++;
					retry_cnt=0;
				}
				else{
					retry_cnt++;
					delay(2000);
				}
				if(retry_cnt == nbiot_cmd_data1[i].rty_max){
					i = 0;
					retry_cnt = 0;
				}
			}
			else
			{
				if(strstr(NB_module->NB_RxBuffer,"ERROR") != NULL){
					i = 0;
					retry_cnt = 0;
				}
				retry_cnt = 0;
				i++;
			}
		}
	} 
}


/********************************************
*Name:init_UDP
*Description:初始化UDP配置、连网
*Input：NULL
*Output：NULL
*Author：GJL
*Date:2017/06/20
********************************************/
void init_UDP(pFrom_NB NB_module,unsigned short port)
{
	UDP_config(NB_module);
}   

/********************************************
*Name:send_data_to_NB
*Description:发送数据并等待返回结果
*Input：要发送的字符串
*Output：NB模块返回的结果
*Author：GJL
*Date:2017/06/20
********************************************/
unsigned char send_data_to_NB(pFrom_NB NB_module,unsigned char CMD_num,char *fmt,...)
{
	UART_enable();
	unsigned char back_result;
	unsigned char len = 0;
	char *ptr=NB_module->NB_TxBuffer;
	memset(NB_BC95.NB_TxBuffer,0,sizeof(NB_BC95.NB_TxBuffer));
	NB_module->NB_param.current_cmd = CMD_num;
	va_list ap;
	va_start(ap,fmt); 
	len = (unsigned char)vsprintf(ptr,fmt,ap);
	va_end(ap);
	nbiot_cmd_data1[CMD_num].revresult = NO_BACK;// NB_BC95.NBresp_result = NO_BACK; 
	NB_BC95.NBreci_pointer = 0;
	memset(NB_BC95.NB_RxBuffer,0,sizeof(NB_BC95.NB_RxBuffer));
	memset(NB_BC95.NB_RxTemp,0,sizeof(NB_BC95.NB_RxTemp));
	NB_module->NB_param.cmd_time_cnt = nbiot_cmd_data1[CMD_num].revtimeout*1000;
	send_data(NB_module->NB_TxBuffer,len);
	while(nbiot_cmd_data1[CMD_num].revresult == NO_BACK)//(NB_BC95.NBresp_result == NO_BACK)
	{
		delay(1);
		NB_module->NB_param.cmd_time_cnt--;
		if(!NB_module->NB_param.cmd_time_cnt)
		{
			nbiot_cmd_data1[CMD_num].revresult = BACK_TIMEOUT;//NB_module->NBresp_result = BACK_TIMEOUT;
		}    
	}
	
	if((strstr(NB_module->NB_RxBuffer,"ERROR") != NULL && (CMD_num > 1) && (CMD_num != STEP_AT_CLEAN)) 
	   || ((strstr(NB_module->NB_RxBuffer,"REBOOT_CAUSE") != NULL) && CMD_num != 1)){
		   UDP_config(NB_module);
	   }
	
	NB_module->NB_param.cmd_time_cnt = 0;
	back_result = nbiot_cmd_data1[CMD_num].revresult;//NB_module->NBresp_result;
	return back_result;
}       

/********************************************
*Name:find_imei
*Description:查找、保存IMEI
*Input：长度len
*Output：bool变量（true/false）
*Author：GJL
*Date:2017/06/20
********************************************/
bool find_imei(unsigned char len)
{
	unsigned char j=0,m=0;
	if(len>=15)
	{
		for(j=0;j<len;j++)
		{
			if((NB_BC95.NB_RxBuffer[j]>=0x30)&&(NB_BC95.NB_RxBuffer[j]<=0x39))
			{
				IMEI_code[m]=NB_BC95.NB_RxBuffer[j];
				m++;
				if(m==15)
				{
					return true;
				}
			}
			else
			{
				m=0;
			}
		}
		return false;
	}
	else
	{
		return false;
	}
}
/********************************************
*Name:find_imsi
*Description:查找、保存IMSI
*Input：长度冷
*Output：bool逻辑变量（true or false）
*Author：GJL
*Date:2017/06/20
********************************************/ 
bool find_imsi(unsigned char len)
{
  unsigned char j=0,m=0;
  if(len>=15)
  {
   for(j=0;j<len;j++)
   {
    if((NB_BC95.NB_RxBuffer[j]>=0x30)&&(NB_BC95.NB_RxBuffer[j]<=0x39))
    {
     IMSI_code[m]=NB_BC95.NB_RxBuffer[j];
     m++;
     if(m==15)
     {
      return true;
     }
    }
    else
    {
     m=0;
    }
   }
    return false;
  }
  else
  {
   return false;
  }
}

/********************************************
*Name:Hex_to_ASCII(char *hex,char *ascii,unsigned char len)
*Description:十六进制转化为ASCII码
*Input：char *hex,char *ascii,unsigned char len
*Output：NULL
*Author：GJL
*Date:2017/06/20
********************************************/ 
void Hex_to_ASCII(char *hex,char *ascii,unsigned char len)
{
  unsigned char i=0,j=0;
  char str[100];
  for(i=0;i<len;i++)
  {
    str[j] = (*hex)>>4;
    str[j+1] = (*hex)&0x0f;
    hex++;
    j+=2;
  }
  for(i=0;i<j;i++)
  {
   sprintf((char *)ascii,"%X",str[i]);
   ascii++;
  }
}

/********************************************
*Name:char_convert_hex(char *ascii)
*Description:两个ASCII码转化为一个十六进制数
*Input：char *ascii
*Output：hex（转换好的单个十六进制数）
*Author：GJL
*Date:2017/07/22
********************************************/ 
char char_convert_hex(char *ascii)
{
	char hex=0;
	if((ascii[0]>='0')&&(ascii[0]<='9'))
	{
		hex = ascii[0]-0x30;
	}
	else if(ascii[0]>='A'&&ascii[0]<='F')
	{
		hex = ascii[0]-0x41+0x0a;
	}
	else if(ascii[0]>='a'&&ascii[0]<='f')
	{
		hex = ascii[0]-0x61+0x0a;
	}
	hex = hex<<4;
	
	if((ascii[1]>='0')&&(ascii[1]<='9'))
	{
		hex = hex+(ascii[1]-0x30);
	}
	else if(ascii[1]>='A'&&ascii[1]<='F')
	{
		hex = hex+(ascii[1]-0x41+0x0a);
	}
	else if(ascii[1]>='a'&&ascii[1]<='f')
	{
		hex = hex+(ascii[0]-0x61+0x0a);
	} 
	return hex;
}

/********************************************
*Name:ASCII_to_Hex(char *ascii,char *hex,unsigned char len)
*Description:ASCII码转化为十六进制
*Input：char *ascii,char *hex,unsigned char len
*Output：NULL
*Author：GJL
*Date:2017/06/20
********************************************/ 
void ASCII_to_Hex(char *ascii,char *hex,unsigned char len)
{
 unsigned char i=0;
  for(i=0;i<len;i+=2)
  {
   *hex++ = char_convert_hex(ascii+i);
  }
}

/********************************************
*Name:Get_signal_strength(pFrom_NB NB_module)
*Description:获取信号强度
*Input：pFrom_NB NB_module
*Output：NULL
*Author：GJL
*Date:2017/06/20
********************************************/ 
void Get_signal_strength(pFrom_NB NB_module)
{
  unsigned char result=0,i=0,j=0,cnt=0;
  char rssi[2]={0};
  cnt = 5;
  while(result!=BACK_OK)
  {
    if(cnt==0)//5次读取强度不成功则返回，信号强度赋值为0
    {
     NB_module->NBsignal_strength=0;
     return;
    }
    result = send_data_to_NB(NB_module,STEP_AT_CSQ,"AT+CSQ\r\n");
    cnt--;
  }
  for(i=0;i<strlen(NB_module->NB_RxBuffer);i++)
  {
   if(NB_module->NB_RxBuffer[i]==':')
   {
    while(NB_module->NB_RxBuffer[++i]!=',')
    {
     rssi[j] = NB_module->NB_RxBuffer[i];
     j++;
    }
    break;
   }
  }
  NB_module->NBsignal_strength = atoi(rssi); 
}



/********************************************
*Name:clean_NBbuffer(pFrom_NB NB_module)
*Description:清除NB接收Buffer
*Input：pFrom_NB NB_module
*Output：NULL
*Author：GJL
*Date:2017/07/25
********************************************/ 
void clean_NBbuffer(pFrom_NB NB_module)
{
	unsigned char j=0,comma_cnt=0,complete;
	complete = 0;
	do
	{
		j = 0;
		send_data_to_NB(NB_module,STEP_AT_CLEAN,"AT+NSORF=1,256\r\n");//此处为防止网络延迟而参生NB接收BUFFER中有多条消息时，\
		在接收到新消息时不会提示NSONMI,因此不管buff中有无数据，\
			都再全部读取一次
				if(NB_BC95.NB_RxBuffer[3]==',')//字符为OK中的字母O
				{
					while(1)
					{
						if(NB_module->NB_RxBuffer[j]==',')
						{
							comma_cnt++;
							if(comma_cnt==5)
							{
								if(NB_module->NB_RxBuffer[j+1]=='0')
								{
									complete = 1; 
								}
								break;
							}    
						}
						j++;
					} 
				}
				else 
				{
					complete = 1;
				}
	}while(complete<1);
}

/********************************************
*Name:read_NB_message(pFrom_NB NB_module,Pcontroler_Symple TagCng)
*Description:读出NB接收的消息
*Input：pFrom_NB NB_module,Pcontroler_Symple TagCng
*Output：NULL
*Author：GJL
*Date:2017/07/27
********************************************/ 
void read_NB_message(pFrom_NB NB_module,Pcontroler_Symple TagCng)
{
	unsigned char times=0,i=0,j=0,count=0;
	char len[2]={0};
	char msg[80]={0};                             //数据长度为36字节，此数组占用72字节
	//***************消息长度************************************
	for(i=0;i<strlen(NB_module->NB_Msg_indicate);i++)//获取数据长度字符串
	{
		if(NB_module->NB_Msg_indicate[i] == ',')
		{
			while(NB_module->NB_Msg_indicate[++i] != '\0')
			{
				len[j] = NB_module->NB_Msg_indicate[i];
				j++;
			}
			break;
		}
	}
	j = 0;
	memset(NB_module->NB_Msg_indicate,0,sizeof(NB_module->NB_Msg_indicate));
	TagCng->reciLength = (unsigned char )atoi(len);//字符串长度转换为十六进制长度
	//************************************************************
	
	//**************从NB模块读取数据，3次读取不成功则返回*********
	nbiot_cmd_data1[STEP_AT_NSORF].revresult = NO_BACK;
	while(nbiot_cmd_data1[STEP_AT_NSORF].revresult != BACK_OK)
	{
		if(times > 3)
		{
			memset(NB_module->NB_Msg_indicate,0,sizeof(NB_module->NB_Msg_indicate));
			return;
		}
		send_data_to_NB(NB_module,STEP_AT_NSORF,"AT+NSORF=1,256\r\n");//读出数据
		times++;
	}
	//***************************************************************
	
	//********************提取出消息数据***********************************
	for(i=0;i<strlen(NB_module->NB_RxBuffer);i++)
	{
		if(NB_module->NB_RxBuffer[i] == ',')
		{
			count++;
			if(count == 4)                           //    例如"0,192.168.5.1,1024,2,ABAB,0" NB上传的第四个逗号和第五个逗号之间为数据
			{
				while(NB_module->NB_RxBuffer[++i] != ',')
				{
					msg[j] = NB_module->NB_RxBuffer[i];
					j++;
				}
				break;
			}
		}
	}
	ASCII_to_Hex(msg,(char *)TagCng->rfReci,TagCng->reciLength*2);
	//***************************************************************************
}

/********************************************
*Name:query_receive_data(pFrom_NB NB_module,Pcontroler_Symple TagCng)
*Description:查询NB接收的消息
*Input：pFrom_NB NB_module,Pcontroler_Symple TagCng
*Output：NULL
*Author：GJL
*Date:2017/08/23
********************************************/ 
unsigned char query_receive_data(pFrom_NB NB_module,Pcontroler_Symple TagCng)
{
	unsigned char times=0,i=0,j=0,count=0;
	char len[2]={0};
	char msg[80]={0};                             //数据长度为36字节，此数组占用72字节
	//**************从NB模块读取数据，3次读取不成功则返回*********
	nbiot_cmd_data1[STEP_AT_NSORF].revresult = NO_BACK;
	while(nbiot_cmd_data[STEP_AT_NSORF].revresult != BACK_OK)
	{
		if(times > 3)
		{
			memset(NB_module->NB_Msg_indicate,0,sizeof(NB_module->NB_Msg_indicate));
			return 0;
		}
		send_data_to_NB(NB_module,STEP_AT_NSORF,"AT+NSORF=0,256\r\n");//读出数据
		times++;
	}
	//***************************************************************
	if((NB_module->NB_RxBuffer[2]=='O')||(NB_module->NB_RxBuffer[2]=='E'))//此处为英文字母O和E
	{
		return 0;
	}
	//********************提取出消息数据和长度***********************************
	for(i=0;i<strlen(NB_module->NB_RxBuffer);i++)
	{
		if(NB_module->NB_RxBuffer[i] == ',')
		{
			count++;
			if(count ==3 )
			{
				while(NB_module->NB_RxBuffer[++i] != ',')
				{
					len[j] = NB_module->NB_RxBuffer[i];
					j++;
				}
				count+=1;
			} 
			if(count == 4)                           //    例如"0,192.168.5.1,1024,2,ABAB,0" NB上传的第四个逗号和第五个逗号之间为数据
			{
				j=0;
				while(NB_module->NB_RxBuffer[++i] != ',')
				{
					msg[j] = NB_module->NB_RxBuffer[i];
					j++;
				}
				break;
			}
		}
	}
	TagCng->reciLength = (unsigned char )atoi(len);//字符串长度转换为十六进制长度
	ASCII_to_Hex(msg,(char *)TagCng->rfReci,TagCng->reciLength*2);
	return 1;
}
