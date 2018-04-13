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
	unsigned char  LEDFlag;//led���ر�־��ȡϵͳ������
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
	signed short EMData_xBottom;							//x��ǿ�ȱ���
	signed short EMData_yBottom;							//y��ǿ�ȱ���
	signed short EMData_zBottom;							//z��ǿ�ȱ���
}*PSensorbottom,SSensorbottom;
typedef struct  paraConfig
{     
	unsigned char ParaIntFlag;							//������ʼ����־
	unsigned char WdtInterval; 							//wdt�̶�˯��ʱ�䣬
	unsigned char FastRouseAlarm; 						//��ο��ٻ��ѱ�����־
	unsigned char FastRouse; 							//wdt���ٻ��ѱ�־
	//  unsigned char FastRouCount; 						//wdt���ٻ��ѱ�־
	unsigned char HeartbeatInterval; 					//�������ʱ��
	unsigned int AlarmValid;
	unsigned int AlarmStatus;
	unsigned char HMCOutFlag;							//�ȴ��شŲ����˳���־��0--�˳���1--�����ȴ�
	unsigned char GetEMBottomFlag;						//ȡ���׳ɹ���־
	unsigned char GetEMBottom_RFFlag;					//�شŹ�����־
	signed int GetEMBottom_RFModThreshold;				//�ų�ǿ��������
	signed int GetEMBottom_RFAngleThreshold;				//�ų�ƫת�Ƕ�������
	signed short EMData_xBottom;							//x��ǿ�ȱ���
	signed short EMData_yBottom;							//y��ǿ�ȱ���
	signed short EMData_zBottom;							//z��ǿ�ȱ���
	signed int EMData_xyz_AbsValue_Module_Bottom;		//xyz��仯ģֵ����
	signed int EMData_RMS;
	signed int EMData_xRMS;
	signed int EMData_zRMS;
} *PparaConfig,SparaConfig;

//���������ṹ
typedef  struct AttrOfTag
{
		unsigned char        flag;      		//flag ��1
		unsigned char        channel;  		//RF ͨ��
		unsigned char        type;   			//���� 0-��д����1-��վ��2-�м̣�3-��ǩ
		unsigned char        power;    		//���书��
		unsigned char        pn;       		//pn��
		unsigned char        mode;     		//����ģʽ �� 0-������1-������2-������
		unsigned char        reserved[2];
		unsigned long        paramAddr;    	//�豸�洢����ַ
		unsigned long        jumpAddr;     	//app����ַ
		unsigned long        groupId;  		//��ID
		unsigned long        deviceId; 		//�豸ID
		unsigned long        ver;      		//�汾��
		unsigned long        updateGroupId;    //��Ӧ�����ֳֻ���
		unsigned long        updateId;      	//��Ӧ�����ֳֻ���
}*pAttrOfTag,sAttrOfTag;

typedef struct Controler_config
{
  unsigned char ini_sign[3];
  unsigned char Tag_status;  //20170727����״̬��־��ACTIVATEΪ����״̬��LEAVE_FACTORYΪ����״̬
  unsigned char Tag_initNB;
  SnodeConfig   TagNode;           //��7�ֽ�
  SparaConfig   TagPara;           //��32�ֽ�
} *PcontrolerConfig,ScontrolerConfig;

typedef struct Controler_Symple
{
	ScontrolerConfig  Config;
	unsigned char txReci[RfReciBufferSize];		//RfSendPackage
} *Pcontroler_Symple,Sontroler_Symple;

#endif
