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
extern unsigned char SendAlarmPackage(Pcontroler_Symple TagCng,unsigned char RequestAckFlag);	//发送报警包
extern void GoSleep(unsigned int nS);
extern void send_data( char *d,unsigned char length);
//void  writeFlashParameter(PcontrolerConfig TagCng);
//extern unsigned char SendBerthStatusChange(Pcontroler_Symple TagCng,unsigned char CarStatus);
U8 start_get_magnetic_bottom(Pcontroler_Symple TagCng); //初始获取一次本底

void ReadMag(U8* nsMag,U8 RegAdd,U8 Len);

extern Sontroler_Symple TagCng_symple;//全局可调用的参数结构体
extern U8 DebugInfFlag;//调试信息标志

extern unsigned char HandWakeFlag;
U8 ParkState=0;//当前停车状态，用于比较停车状态是否改变
U8 FastGetParkCount=0;//快速获取车辆状态，当这个变量不为零时，为前面车辆刚刚离开，此时快速启动检查防止跟车进车位过快

SSensor3100   Sensor3100L;
SSensorbottom DynamicBottom;

signed int EMData_x_Vary;//x轴变化
signed int EMData_y_Vary;//y轴变化
signed int EMData_z_Vary;//z轴变化

//extern unsigned int second_flag;//从eeprom中读取参数初始化变量组
U8 m_clearBottomBufferFlag = 0;  // 清除本底缓存

extern  U8 GetTimeBottomCount;                     //取得实时本底次数
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
	if (SetMagBase()==1)//设置地磁本底
	{
		//初始化参数
		SendNodeCount=SendPackCount-1;//如果校准成功，则在下一轮返回数据供手持机查看
		return 1;
	}
	return 0;
}


//U8 start_get_magnetic_bottom(Pcontroler_Symple TagCng) //初始获取一次本底
//{
//	if (SetMagBase()==1)//设置地磁本底
//	{
//		writeFlashParameter((PcontrolerConfig)TagCng);//设置本底成功，保存
//		return 1;
//	}
//	return 0;
//}

void Gather_DataOfR3100()//取地磁数据
{ 
	U8 nsBuff[9];
	long reading = 0;
	GetMag(&nsBuff[0],1);//采集地磁传感器数据
	
	GPIOB->ODR&=(~GPIO_Pin_0);//R3100 Power I\O//关闭地磁供电，节能
	GPIOB->DDR &=(~GPIO_Pin_0);//R3100 Power I\O（PA4）MEG_PRW控制线改为输入
	
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
	GPIOB->DDR |=  BITs(0);//R3100 Power （PA4）RM3100供电口线为输出
	GPIOB->ODR |=  BITs(0);// R3100 Power I\O地磁供电
	minidelay(100);//*us//上电延时等待R3100准备，小于30us读出数据错误，典型值200us
	IicInit();//IIC初始化
	IicWrite(0,0x70);//设置为单一测量模式
	nop();//中断等待，时间与查询相等，功耗约为查询的一半
	R3100_IRQ_set;//开地磁芯片中断输入
	//  EXTI_ClearITPendingBit(EXTI_IT_Pin0);//清PD1引脚中断
	enableInterrupts();	//开中断,关中断动作在睡眠被唤醒后执行
	GoSleep(30);		//rtc睡眠时间,15ms            
	inToSleep((Pcontroler_Symple)&TagCng_symple);//20170717
	R3100_IRQ_clr;		//关地磁芯片中断输入
	ReadMag(nsMag,0x24,9);
	nop();
}


unsigned long Getdataa(Pcontroler_Symple TagCng,unsigned char repeat)
{
	Gather_DataOfR3100();												//取地磁数据
	EMDealGeomagneticValue_VectorDifference();							//处理地磁数据
	return 1;
}


U8 SetMagBase()//设置地磁本底
{
	unsigned char i;  	
	unsigned long Rms;
	long data_x=0,data_y=0,data_z=0,data_Count=0;//用以存储磁场当前值累计值
	signed short xBottomtemp=0,yBottomtemp=0,zBottomtemp=0;
	
	TagCng_symple.Config.TagPara.GetEMBottomFlag=0;
	for(i=0;i<4;i++)
	{
		Gather_DataOfR3100();////取地磁数据
		
		data_x+=Sensor3100L.EMData_x; 
		data_y+=Sensor3100L.EMData_y;
		data_z+=Sensor3100L.EMData_z;
		data_Count++;
		delay(10);
	}
	//xyz值四舍五入，4次采样平均，所有+-2,2/4=0.5
	data_x=(data_x>0)?data_x+2:data_x-2;
	data_y=(data_y>0)?data_y+2:data_y-2;
	data_z=(data_z>0)?data_z+2:data_z-2;
	
	xBottomtemp=data_x/data_Count; 
	yBottomtemp=data_y/data_Count; 
	zBottomtemp=data_z/data_Count; 

	//复检地磁本底，此时变化应该趋近于零，否则认为初始化错误，应该返回给上级设备错误信息
	for(i=0;i<10;i++)
	{
		delay(10);
		Gather_DataOfR3100();//取地磁数据
		
		Rms =    (Sensor3100L.EMData_x-xBottomtemp)*(Sensor3100L.EMData_x-xBottomtemp)
			+(Sensor3100L.EMData_y-yBottomtemp)*(Sensor3100L.EMData_y-yBottomtemp)
				+(Sensor3100L.EMData_z-zBottomtemp)*(Sensor3100L.EMData_z-zBottomtemp);
		nop();
		if(Rms<10)//未开根号，减少计算量 原来是4 改为9 by HL
		{
			TagCng_symple.Config.TagPara.GetEMBottomFlag=1;
			
			//固定本底值
			TagCng_symple.Config.TagPara.EMData_xBottom=xBottomtemp;			//MagneticFactor; 
			TagCng_symple.Config.TagPara.EMData_yBottom=yBottomtemp;			//MagneticFactor; 
			TagCng_symple.Config.TagPara.EMData_zBottom=zBottomtemp;			//MagneticFactor; 
			
			//将当前获取的合法本底值作为动态本底值初始值
			DynamicBottom.EMData_xBottom=TagCng_symple.Config.TagPara.EMData_xBottom;
			DynamicBottom.EMData_yBottom=TagCng_symple.Config.TagPara.EMData_yBottom;
			DynamicBottom.EMData_zBottom=TagCng_symple.Config.TagPara.EMData_zBottom;
			GetTimeBottomCount=0;
			m_clearBottomBufferFlag = 1;										//需要初始化，清除本底缓冲区内的数据
			return 1;
		}
	}
	return 0;
}



void EMDealGeomagneticValue_VectorDifference(void)//处理地磁数据
{
	signed long diffOfRMold=Sensor3100L.diffOfRM;
	
	EMData_x_Vary=Sensor3100L.EMData_x-DynamicBottom.EMData_xBottom;//X强度变化矢量差,单位
	EMData_y_Vary=Sensor3100L.EMData_y-DynamicBottom.EMData_yBottom;//y强度变化矢量差,单位
	EMData_z_Vary=Sensor3100L.EMData_z-DynamicBottom.EMData_zBottom;//z强度变化矢量差,单位
	
	Sensor3100L.diffOfRM =  (unsigned long) (sqrtf(   
												   (long)((EMData_x_Vary))*(long)((EMData_x_Vary))
													   +(long)((EMData_y_Vary))*(long)((EMData_y_Vary))
														   +4*(long)((EMData_z_Vary))*(long)((EMData_z_Vary))));
	
	//加重Z轴对模值的贡献度
	Sensor3100L.diffOfRM=(int32_t)(Sensor3100L.diffOfRM*0.8);//Z轴3*3倍--0.66，4*4倍--0.55,2*2倍---0.8倍,2倍---1倍
	
	Sensor3100L.diffOfRM=TYPE_S8(Sensor3100L.diffOfRM);
	
	diffOfRMold=(diffOfRMold>Sensor3100L.diffOfRM)? diffOfRMold-Sensor3100L.diffOfRM:Sensor3100L.diffOfRM-diffOfRMold;
	if (diffOfRMold>=4 && ParkState==1)
	{
		FastGetParkCount=3;	//启动次快速检测
	}
}

//车辆状态反转判断
void judge_changeOfmodule(Pcontroler_Symple TagCng)
{
  
	U8 SendC = 3;												//发送次数 默认发送3次
	enableInterrupts();
											//发送消息前开中断
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
				//打开BT
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
			SendNodeCount=0;									//发送数据成功,重新计算发送间隔
			SendC--;											//发送失败重复3次
			enableInterrupts();									//开中断,关中断动作在睡眠被唤醒后执行
			GoSleep(2048);										//rtc睡眠时间            
			inToSleep((Pcontroler_Symple)&TagCng_symple);		//进入睡眠           
		}
		
                
	}
	else														//如果状态未翻转，则启动心跳检查程序
	{  
        printf("car state Not Reversal\n");
		if ((++SendNodeCount)>=SendPackCount)					//发包控制，达到心跳发包时间         
		{
			enableInterrupts();									//发送消息前开中断
			SendCarStatus(TagCng,ParkState);					//发送状态
			SendNodeCount=0;									//发送数据成功,重新计算发送间隔
			Keep_AwakeCount=0;
			NB_BC95.nb_recv_data.receive_status |= 0x80;
			NB_BC95.nb_recv_data.receive_cnt = 0;
//			checkNBRecvBuff((pNB)&NB_BC95);
		}
	}
}

unsigned char FixedV_Check()					//固定值判断车位状态
{
	U8 ValueWeight=0;							//阀值权重计数
	short Module_T,Module_TB;
	short Module_XT,Module_YT,Module_ZT;
	short Addit_XYZT,Mul_XYZAT=1;
	
	Module_T=abs((short)Sensor3100L.diffOfRM);	//xyz强度变化矢量差的平方和，单位微特
	Module_XT=abs((short)((EMData_x_Vary)));	//X强度变化矢量差,单位微特
	Module_YT=abs((short)((EMData_y_Vary)));	//y强度变化矢量差,单位微特
	Module_ZT=abs((short)((EMData_z_Vary)));	//z强度变化矢量差,单位微特
	
	Module_TB=(short)TagCng_symple.Config.TagPara.GetEMBottom_RFModThreshold;//xyz强度变化矢量差的平方和的判断阀值，单位微特
	//    Angle_XYZTB=(short)TagCng_symple.Config.TagPara.GetEMBottom_RFAngleThreshold;//xyz矢量值与本底的角度差判断阀值,单位度
	//如果已经有车，则按照阈值0.8倍的标准判决，即存在0.8倍的缓冲空间，解决参数临界时反复跳动的问题
	if ( ParkState==1)
	{
		Module_TB=(int16_t)(Module_TB*0.8);
	}
	
	if(Module_XT  >Module_TB*0.6)           ValueWeight=ValueWeight+1;//
	if(Module_YT  >Module_TB*0.6)           ValueWeight=ValueWeight+1;//
	if(Module_ZT  >Module_TB*0.6)           ValueWeight=ValueWeight+1;//
	
	Addit_XYZT=(Module_XT+Module_YT+Module_ZT);//xyz强度变化矢量差累加和，单位微特
	if(Addit_XYZT>=Module_TB*1.5)              ValueWeight=ValueWeight+1;//当xyz各轴强度变化值累加和大于设定强度阀值1.5倍

	//        //经分析发现，邻道有车干扰出现的强度变化
	//当xyz各轴强度变化值乘积再乘以角度变化值，积大于设定角度阀值6倍
	if (Module_XT>=4)                          Mul_XYZAT=Mul_XYZAT*(Module_XT-2);
	if (Module_YT>=4)                          Mul_XYZAT=Mul_XYZAT*(Module_YT-2);
	if (Module_ZT>=4)                          Mul_XYZAT=Mul_XYZAT*(Module_ZT-2);
	if(Mul_XYZAT>=Module_TB*3)                 ValueWeight=ValueWeight+1;
	
	return ValueWeight;
}
