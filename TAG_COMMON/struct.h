#ifndef struct_h 
#define struct_h 1

#define MaxheartBeatCnt     	4
#define RfReciBufferSize		50
#define TxReciBufferSize		50

#define LastCmdTempBufferSize	50

#include <math.h>

#define  GetSignChangTolong(x,n) (((x&0x8000)!=0)?(-(((long)(-x))/n)):(((long)x)/n))
#define  GetSignSub(x1,x2) ((((x1-x2)&0x8000)!=0)?((0x10000-(x1-x2))):(x1-x2))
#define  GetAbsValue(x1,x2) ((x1>x2)?(x1-x2):(x2-x1))
#define  GetSubValue(x1,x2) ((x1>x2)?(x1-x2):(10-x2))

typedef struct EMData
{
  signed short  data_x;  
  signed short  data_y; 
  signed short  data_z; 
}*PEMData,SEMData;

//typedef struct SetTagcodeVoidTimer
//{ 
//	unsigned char  CodeSet;
//	unsigned int   codeVoidTimer;
//}*PsetcodeVoidTimer,SsetcodeVoidTimer;
//typedef struct OpenTagWCM
//{ 
//	unsigned char codeCodeOpenFlag;
//	unsigned char CodeOpen_start;
//	unsigned int  CodeOpenTimer;
//}*PopenCode,SopenCode;
//typedef struct Mtstbuff
//{ 
//	unsigned long Rstrength;
//	float angle_z;
//}*Ptstbuff,Ststbuff;

//typedef struct Marrow
//{ 
//	short data_x;
//	short data_y;
//	short data_z;
//}*Parrow,Sarrow;

typedef struct Controler_cnfig
{
	unsigned char Tag_id_nub[3];	 
	unsigned int  UserCode;	   
	unsigned char  IntFlag;
	unsigned char  LEDFlag;//led开关标志，取系统灯配置
	unsigned char  McuSleepFlag;
}*PnodeConfig,SnodeConfig;

typedef struct Sensor3100
{
	signed short  EMData_x;
	signed short  EMData_y;
	signed short  EMData_z;
	signed long diffOfRM;
}*PSensor3100,SSensor3100;

typedef struct Sensorbottom
{
	signed short EMData_xBottom;							//x轴强度本底
	signed short EMData_yBottom;							//y轴强度本底
	signed short EMData_zBottom;							//z轴强度本底
}*PSensorbottom,SSensorbottom;
typedef struct  paraConfig
{     
	unsigned char ParaIntFlag;							//参数初始化标志
	unsigned char WdtInterval; 							//wdt固定睡眠时间，
	unsigned char FastRouseAlarm; 						//多次快速唤醒报警标志
	unsigned char FastRouse; 							//wdt快速唤醒标志
	//  unsigned char FastRouCount; 						//wdt快速唤醒标志
	unsigned char HeartbeatInterval; 					//心跳间隔时间
	unsigned int AlarmValid;
	unsigned int AlarmStatus;
	unsigned char HMCOutFlag;							//等待地磁操作退出标志，0--退出，1--继续等待
	unsigned char GetEMBottomFlag;						//取本底成功标志
	unsigned char GetEMBottom_RFFlag;					//地磁工作标志
	signed int GetEMBottom_RFModThreshold;				//磁场强度灵敏度
	signed int GetEMBottom_RFAngleThreshold;				//磁场偏转角度灵敏度
	signed short EMData_xBottom;							//x轴强度本底
	signed short EMData_yBottom;							//y轴强度本底
	signed short EMData_zBottom;							//z轴强度本底
	signed int EMData_xyz_AbsValue_Module_Bottom;		//xyz轴变化模值本底
	signed int EMData_RMS;
	signed int EMData_xRMS;
	signed int EMData_zRMS;
} *PparaConfig,SparaConfig;

//升级参数结构
typedef  struct AttrOfTag
{
		unsigned char        flag;      		//flag 置1
		unsigned char        channel;  		//RF 通道
		unsigned char        type;   			//类型 0-读写器，1-基站，2-中继，3-标签
		unsigned char        power;    		//发射功率
		unsigned char        pn;       		//pn码
		unsigned char        mode;     		//工作模式 （ 0-主动，1-被动，2-升级）
		unsigned char        reserved[2];
		unsigned long        paramAddr;    	//设备存储区地址
		unsigned long        jumpAddr;     	//app区地址
		unsigned long        groupId;  		//组ID
		unsigned long        deviceId; 		//设备ID
		unsigned long        ver;      		//版本号
		unsigned long        updateGroupId;    //对应升级手持机的
		unsigned long        updateId;      	//对应升级手持机的
}*pAttrOfTag,sAttrOfTag;

typedef struct Controler_config
{
  unsigned char ini_sign[3];
  unsigned char Tag_status;  //20170727出厂状态标志，ACTIVATE为激活状态，LEAVE_FACTORY为出厂状态
  unsigned char Tag_initNB;
  SnodeConfig   TagNode;           //共7字节
  SparaConfig   TagPara;           //共32字节
} *PcontrolerConfig,ScontrolerConfig;

typedef struct Controler_Symple
{
	ScontrolerConfig  Config;
	unsigned char txReci[RfReciBufferSize];		//RfSendPackage
} *Pcontroler_Symple,Sontroler_Symple;

#endif
