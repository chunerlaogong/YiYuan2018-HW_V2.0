#include <stdlib.h>

#include "Common.h"
#include  "McuTarger.h"
#include "Iic.h"
#include "RegPscStruct.h"
#include "struct.h"
#include "compilefiles.h"
#include "stm8l15x_exti.h"
#include "stm8l15x_it.h"
#include "parkStateJudge.h"
#include "CRC.h"
#include "MyNBIOT.h"
#include "RTC_Driver.h"
#include "R3100.h"
#include "UART.H"
#include "Bluetooth.h"
extern unsigned char Test_Product_Flag;
extern sNBiotAT NBAtCommand;
extern sBT bluetooth_data;
//void flashLeds(unsigned char ledNum,unsigned char ledflags);
extern unsigned char SendAlarmPackage(Pcontroler_Symple TagCng,unsigned char RequestAckFlag);	//���ͱ�����
extern void GoSleep(unsigned int nS);
extern void send_data( char *d,unsigned char length);
//void  writeFlashParameter(PcontrolerConfig TagCng);
//extern unsigned char SendBerthStatusChange(Pcontroler_Symple TagCng,unsigned char CarStatus);
U8 start_get_magnetic_bottom(Pcontroler_Symple TagCng); //��ʼ��ȡһ�α���

void ReadMag(U8* nsMag,U8 RegAdd,U8 Len);

extern Sontroler_Symple TagCng_symple;//ȫ�ֿɵ��õĲ����ṹ��
extern U8 DebugInfFlag;//������Ϣ��־

extern unsigned char HandWakeFlag;
U8 ParkState=0;//��ǰͣ��״̬�����ڱȽ�ͣ��״̬�Ƿ�ı�
U8 FastGetParkCount=0;//���ٻ�ȡ����״̬�������������Ϊ��ʱ��Ϊǰ�泵���ո��뿪����ʱ������������ֹ��������λ����

SSensor3100   Sensor3100L;
SSensorbottom DynamicBottom;

signed int EMData_x_Vary;//x��仯
signed int EMData_y_Vary;//y��仯
signed int EMData_z_Vary;//z��仯

//extern unsigned int second_flag;//��eeprom�ж�ȡ������ʼ��������
U8 m_clearBottomBufferFlag = 0;  // ������׻���

extern  U8 GetTimeBottomCount;                     //ȡ��ʵʱ���״���
extern sNB	NB_BC95;
uint32_t SendPackCount;//,SendPackCount_backup;
uint32_t SendNodeCount;
volatile unsigned char Keep_AwakeCount=0;
extern void inToSleep(Pcontroler_Symple TagCng);

#if (_MAG_SENSOR || _TEST_D)
U16 m_nNoReSetPXTImes = 0;
U8 m_bTimerForGet = 0;
S8 m_cPsState = 0;  

static U32 m_lLastMag = 0;

#endif

U8 Send_R3100ToRW_Test(void)
{
	if (SetMagBase()==1)//���õشű���
	{
		//��ʼ������
		SendNodeCount=SendPackCount-1;//���У׼�ɹ���������һ�ַ������ݹ��ֳֻ��鿴
		return 1;
	}
	return 0;
}


//U8 start_get_magnetic_bottom(Pcontroler_Symple TagCng) //��ʼ��ȡһ�α���
//{
//	if (SetMagBase()==1)//���õشű���
//	{
//		writeFlashParameter((PcontrolerConfig)TagCng);//���ñ��׳ɹ�������
//		return 1;
//	}
//	return 0;
//}

void Gather_DataOfR3100()//ȡ�ش�����
{ 
	U8 nsBuff[9];
	long reading = 0;
	GetMag(&nsBuff[0],1);//�ɼ��شŴ���������
	
	GPIOB->ODR&=(~GPIO_Pin_0);//R3100 Power I\O//�رյشŹ��磬����
	GPIOB->DDR &=(~GPIO_Pin_0);//R3100 Power I\O��PA4��MEG_PRW�����߸�Ϊ����
	
	reading  = *nsBuff <<8;
	reading |= *(nsBuff+1);
	reading  = reading <<8;
	reading |= *(nsBuff+2);
	Sensor3100L.EMData_x= (signed short)reading/MagneticFactor;//(reading >>8);///273.0;
	
	reading  = *(nsBuff+3) <<8;
	reading |= *(nsBuff+4);
	reading  = reading <<8;
	reading |= *(nsBuff+5);
	Sensor3100L.EMData_y=(signed short)reading/MagneticFactor;// (reading >>8);///273.0;
	
	reading  = *(nsBuff+6) <<8;
	reading |= *(nsBuff+7);
	reading  = reading <<8;
	reading |= *(nsBuff+8);
	Sensor3100L.EMData_z=(signed short)reading/MagneticFactor;// (reading >>8);///273.0;
	nop();
}


void ReadMag(U8* nsMag,U8 RegAdd,U8 Len)
{
	IicReadBytes(RegAdd,Len,(U8*)nsMag);
}

unsigned long i;
void GetMag(U8* nsMag,U8 cTemp)//GetMag(nsBuff,1);
{
	GPIOB->DDR |=  BITs(0);//R3100 Power ��PA4��RM3100�������Ϊ���
	GPIOB->ODR |=  BITs(0);// R3100 Power I\O�شŹ���
	minidelay(100);//*us//�ϵ���ʱ�ȴ�R3100׼����С��30us�������ݴ��󣬵���ֵ200us
	IicInit();//IIC��ʼ��
	IicWrite(0,0x70);//����Ϊ��һ����ģʽ
	nop();//�жϵȴ���ʱ�����ѯ��ȣ�����ԼΪ��ѯ��һ��
	R3100_IRQ_set;//���ش�оƬ�ж�����
	//  EXTI_ClearITPendingBit(EXTI_IT_Pin0);//��PD1�����ж�
	enableInterrupts();	//���ж�,���ж϶�����˯�߱����Ѻ�ִ��
	GoSleep(30);		//rtc˯��ʱ��,15ms            
	inToSleep((Pcontroler_Symple)&TagCng_symple);//20170717
	R3100_IRQ_clr;		//�صش�оƬ�ж�����
	ReadMag(nsMag,0x24,9);
	nop();
}


unsigned long Getdataa(Pcontroler_Symple TagCng,unsigned char repeat)
{
	Gather_DataOfR3100();												//ȡ�ش�����
	EMDealGeomagneticValue_VectorDifference();							//����ش�����
	return 1;
}


U8 SetMagBase()//���õشű���
{
	unsigned char i;  	
	unsigned long Rms;
	long data_x=0,data_y=0,data_z=0,data_Count=0;//���Դ洢�ų���ǰֵ�ۼ�ֵ
	signed short xBottomtemp=0,yBottomtemp=0,zBottomtemp=0;
	
	TagCng_symple.Config.TagPara.GetEMBottomFlag=0;
	for(i=0;i<4;i++)
	{
		Gather_DataOfR3100();////ȡ�ش�����
		
		data_x+=Sensor3100L.EMData_x; 
		data_y+=Sensor3100L.EMData_y;
		data_z+=Sensor3100L.EMData_z;
		data_Count++;
		delay(10);
	}
	//xyzֵ�������룬4�β���ƽ��������+-2,2/4=0.5
	data_x=(data_x>0)?data_x+2:data_x-2;
	data_y=(data_y>0)?data_y+2:data_y-2;
	data_z=(data_z>0)?data_z+2:data_z-2;
	
	xBottomtemp=data_x/data_Count; 
	yBottomtemp=data_y/data_Count; 
	zBottomtemp=data_z/data_Count; 

	//����شű��ף���ʱ�仯Ӧ���������㣬������Ϊ��ʼ������Ӧ�÷��ظ��ϼ��豸������Ϣ
	for(i=0;i<10;i++)
	{
		delay(10);
		Gather_DataOfR3100();//ȡ�ش�����
		
		Rms =    (Sensor3100L.EMData_x-xBottomtemp)*(Sensor3100L.EMData_x-xBottomtemp)
			+(Sensor3100L.EMData_y-yBottomtemp)*(Sensor3100L.EMData_y-yBottomtemp)
				+(Sensor3100L.EMData_z-zBottomtemp)*(Sensor3100L.EMData_z-zBottomtemp);
		nop();
		if(Rms<10)//δ�����ţ����ټ����� ԭ����4 ��Ϊ9 by HL
		{
			TagCng_symple.Config.TagPara.GetEMBottomFlag=1;
			
			//�̶�����ֵ
			TagCng_symple.Config.TagPara.EMData_xBottom=xBottomtemp;			//MagneticFactor; 
			TagCng_symple.Config.TagPara.EMData_yBottom=yBottomtemp;			//MagneticFactor; 
			TagCng_symple.Config.TagPara.EMData_zBottom=zBottomtemp;			//MagneticFactor; 
			
			//����ǰ��ȡ�ĺϷ�����ֵ��Ϊ��̬����ֵ��ʼֵ
			DynamicBottom.EMData_xBottom=TagCng_symple.Config.TagPara.EMData_xBottom;
			DynamicBottom.EMData_yBottom=TagCng_symple.Config.TagPara.EMData_yBottom;
			DynamicBottom.EMData_zBottom=TagCng_symple.Config.TagPara.EMData_zBottom;
			GetTimeBottomCount=0;
			m_clearBottomBufferFlag = 1;										//��Ҫ��ʼ����������׻������ڵ�����
			return 1;
		}
	}
	return 0;
}



void EMDealGeomagneticValue_VectorDifference(void)//����ش�����
{
	signed long diffOfRMold=Sensor3100L.diffOfRM;
	
	EMData_x_Vary=Sensor3100L.EMData_x-DynamicBottom.EMData_xBottom;//Xǿ�ȱ仯ʸ����,��λ
	EMData_y_Vary=Sensor3100L.EMData_y-DynamicBottom.EMData_yBottom;//yǿ�ȱ仯ʸ����,��λ
	EMData_z_Vary=Sensor3100L.EMData_z-DynamicBottom.EMData_zBottom;//zǿ�ȱ仯ʸ����,��λ
	
	Sensor3100L.diffOfRM =  (unsigned long) (sqrtf(   
												   (long)((EMData_x_Vary))*(long)((EMData_x_Vary))
													   +(long)((EMData_y_Vary))*(long)((EMData_y_Vary))
														   +4*(long)((EMData_z_Vary))*(long)((EMData_z_Vary))));
	
	//����Z���ģֵ�Ĺ��׶�
	Sensor3100L.diffOfRM=(int32_t)(Sensor3100L.diffOfRM*0.8);//Z��3*3��--0.66��4*4��--0.55,2*2��---0.8��,2��---1��
	
	Sensor3100L.diffOfRM=TYPE_S8(Sensor3100L.diffOfRM);
	
	diffOfRMold=(diffOfRMold>Sensor3100L.diffOfRM)? diffOfRMold-Sensor3100L.diffOfRM:Sensor3100L.diffOfRM-diffOfRMold;
	if (diffOfRMold>=4 && ParkState==1)
	{
		FastGetParkCount=3;	//�����ο��ټ��
	}
}

//����״̬��ת�ж�
void judge_changeOfmodule(Pcontroler_Symple TagCng)
{
  
	U8 SendC = 3;												//���ʹ��� Ĭ�Ϸ���3��
	enableInterrupts();
											//������Ϣǰ���ж�
	if (berthStateSwithProcess(&ParkState, TagCng) == STATE_REVERSAL)
	{
                printf("car state Reversal\n");
	 	DebugInfFlag=0;
		while(SendC>0)
		{
			printf("SendC = %d\n", SendC);
			SendCarStatus(TagCng,ParkState);
			if(ParkState == 1)
		    {
				//��BT
				printf("yjd-BTPowerSet\n");
		        BTPowerSet;
		        USART_Cmd(USART_FOR_BT, ENABLE);
				Test_Product_Flag = 1;
				//NBAtCommand.NBEnableFlag = 0x00;
				TagCng_symple.Config.TagNode.McuSleepFlag = 0x00;
				TIM4_Cmd(ENABLE);
				//bluetooth_data.BTEnableFlag = 0x01;
				LED1_set;
			}
			SendNodeCount=0;									//�������ݳɹ�,���¼��㷢�ͼ��
			SendC--;											//����ʧ���ظ�3��
			enableInterrupts();									//���ж�,���ж϶�����˯�߱����Ѻ�ִ��
			GoSleep(2048);										//rtc˯��ʱ��            
			inToSleep((Pcontroler_Symple)&TagCng_symple);		//����˯��           
		}
		
                
	}
	else														//���״̬δ��ת������������������
	{  
        printf("car state Not Reversal\n");
		if ((++SendNodeCount)>=SendPackCount)					//�������ƣ��ﵽ��������ʱ��         
		{
			enableInterrupts();									//������Ϣǰ���ж�
			SendCarStatus(TagCng,ParkState);					//����״̬
			SendNodeCount=0;									//�������ݳɹ�,���¼��㷢�ͼ��
			Keep_AwakeCount=0;
			NB_BC95.nb_recv_data.receive_status |= 0x80;
			NB_BC95.nb_recv_data.receive_cnt = 0;
//			checkNBRecvBuff((pNB)&NB_BC95);
		}
	}
}

unsigned char FixedV_Check()					//�̶�ֵ�жϳ�λ״̬
{
	U8 ValueWeight=0;							//��ֵȨ�ؼ���
	short Module_T,Module_TB;
	short Module_XT,Module_YT,Module_ZT;
	short Addit_XYZT,Mul_XYZAT=1;
	
	Module_T=abs((short)Sensor3100L.diffOfRM);	//xyzǿ�ȱ仯ʸ�����ƽ���ͣ���λ΢��
	Module_XT=abs((short)((EMData_x_Vary)));	//Xǿ�ȱ仯ʸ����,��λ΢��
	Module_YT=abs((short)((EMData_y_Vary)));	//yǿ�ȱ仯ʸ����,��λ΢��
	Module_ZT=abs((short)((EMData_z_Vary)));	//zǿ�ȱ仯ʸ����,��λ΢��
	
	Module_TB=(short)TagCng_symple.Config.TagPara.GetEMBottom_RFModThreshold;//xyzǿ�ȱ仯ʸ�����ƽ���͵��жϷ�ֵ����λ΢��
	//    Angle_XYZTB=(short)TagCng_symple.Config.TagPara.GetEMBottom_RFAngleThreshold;//xyzʸ��ֵ�뱾�׵ĽǶȲ��жϷ�ֵ,��λ��
	//����Ѿ��г���������ֵ0.8���ı�׼�о���������0.8���Ļ���ռ䣬��������ٽ�ʱ��������������
	if ( ParkState==1)
	{
		Module_TB=(int16_t)(Module_TB*0.8);
	}
	
	if(Module_XT  >Module_TB*0.6)           ValueWeight=ValueWeight+1;//
	if(Module_YT  >Module_TB*0.6)           ValueWeight=ValueWeight+1;//
	if(Module_ZT  >Module_TB*0.6)           ValueWeight=ValueWeight+1;//
	
	Addit_XYZT=(Module_XT+Module_YT+Module_ZT);//xyzǿ�ȱ仯ʸ�����ۼӺͣ���λ΢��
	if(Addit_XYZT>=Module_TB*1.5)              ValueWeight=ValueWeight+1;//��xyz����ǿ�ȱ仯ֵ�ۼӺʹ����趨ǿ�ȷ�ֵ1.5��

	//        //���������֣��ڵ��г����ų��ֵ�ǿ�ȱ仯
	//��xyz����ǿ�ȱ仯ֵ�˻��ٳ��ԽǶȱ仯ֵ���������趨�Ƕȷ�ֵ6��
	if (Module_XT>=4)                          Mul_XYZAT=Mul_XYZAT*(Module_XT-2);
	if (Module_YT>=4)                          Mul_XYZAT=Mul_XYZAT*(Module_YT-2);
	if (Module_ZT>=4)                          Mul_XYZAT=Mul_XYZAT*(Module_ZT-2);
	if(Mul_XYZAT>=Module_TB*3)                 ValueWeight=ValueWeight+1;
	
	return ValueWeight;
}
