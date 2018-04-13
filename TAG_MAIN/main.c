#include <stdlib.h>
#include "stm8l15x_conf.h"
#include "MyNBIOT.h"
#include "Common.h"
#include  "McuTarger.h"
#include "RtcDrv.h"
#include "Spi.h"
#include "zigbee.h"
#include "eemFlash.h"
#include "TIM4Drv.h"
#include "GPIODrv.h"
#include "compilefiles.h"
#include "Type.h"
#include "struct.h"
#include "UART.h"
#include "WDT.h"
#include "Bluetooth.h"
#include "stm8l15x_it.h"
#include "main.h"
#include <stdlib.h>


Sontroler_Symple TagCng_symple ={0x08,0x00,0x00,0x07,0x080,0x04,0x03,0};
void deep_sleep(void);

extern void OutFromSleep(Pcontroler_Symple TagCng);
extern void CLK_RTC_Ini(void);
extern void GoSleep(U16 nS);
U16 DelayCount;//睡眠时间
long second_flag=0;
extern unsigned char HandWakeFlag;
extern unsigned int UserCode;//客户码
extern unsigned long TagID;    // ID,3字节，最大16777215，

void sleep(void);
extern uint16_t ADCdata; //adc采集缓存
extern void testRomOperate(Pcontroler_Symple TagCng);
extern void Gather_DataOfR3100();//取地磁数据
extern void EMDealGeomagneticValue_VectorDifference(void);//处理地磁数据
extern U8 start_get_magnetic_bottom(Pcontroler_Symple TagCng);
extern signed int EMData_x_Vary;//x轴变化夹角
extern signed int EMData_y_Vary;//y轴变化夹角
extern signed int EMData_z_Vary;//z轴变化夹角

extern SSensor3100  Sensor3100L;

extern U8 FastGetParkCount;//快速获取车辆状态，当这个变量不为零时，为前面车辆刚刚离开，此时快速启动检查防止跟车进车位过快
sNBiotAT NBAtCommand;
sNB	NB_BC95;
sBT bluetooth_data;
char ATCmdFromBTData[30] = {0};
unsigned char Startup_get_bottom = 0;
uint32_t TIM4_Count = 0;
/************************************************************************************************
Function: void inimcu(void) 
Description: 初始化MCU
note：
Calls:          
Called By:      
Table Accessed: 
Table Updated:  
Input:               
Output:         
Return:       
Timers:  2015.12.08  
author:  ME
************************************************************************************************/
void inimcu(void)
{
  CLK_RTC_Ini();//RTC时钟初始化
  CLK->ICKCR = 0x15;//Internal clock register,MVR regulator OFF in HALT/Active-halt mode
  PWR->CSR2 = 0x2;
  CLK->CKDIVR = 0; //System clock source   16M
  
  GPIOA->DDR = 0xFF;//输入模式
  GPIOA->CR1 = 0x00;//输入模式下，带上拉电阻输入
  GPIOA->CR2 = 0x00;//输入模式下，禁止中断
  GPIOA->ODR = 0x00;
  
  GPIOB->DDR = 0xFF;//输入模式
  GPIOB->CR1 = 0x00;//输入模式下，带上拉电阻输入
  GPIOB->CR2 = 0x00;//输入模式下，禁止中断
  GPIOB->ODR = 0x00;
  
  
  GPIOC->DDR = 0xFF;//输入模式
  GPIOC->CR1 = 0x00;//输入模式下，带上拉电阻输入
  GPIOC->CR2 = 0x00;//输入模式下，禁止中断 
  GPIOC->ODR = 0x00;
  
  GPIOD->DDR = 0xFF;//输入模式
  GPIOD->CR1 = 0x00;//输入模式下，带上拉电阻输入
  GPIOD->CR2 = 0x00;//输入模式下，禁止中断  
  GPIOD->ODR = 0x00;
  
  GPIOE->DDR = 0xFF;//输入模式
  GPIOE->CR1 = 0x00;//输入模式下，带上拉电阻输入
  GPIOE->CR2 = 0x00;//输入模式下，禁止中断
  GPIOE->ODR = 0x00;
  
  GPIOF->DDR = 0xFF;//输入模式
  GPIOF->CR1 = 0x00;//输入模式下，带上拉电阻输入
  GPIOF->CR2 = 0x00;//输入模式下，禁止中断 
  GPIOF->ODR = 0x00;
  
  /**********口线占用清单*********************/
  
  //****************串口UART3--NB接口************
  GPIOE->DDR|=GPIO_Pin_6;//UART3-TX输出
  GPIOE->DDR&=(~GPIO_Pin_7);//UART3-RX输入
  //*********************************************

  //*******************BLE***************** 
  GPIOA->DDR|=GPIO_Pin_2;//UART1-TX输出
  GPIOA->DDR&=(~GPIO_Pin_3);//UART1-RX输入
  //*******************************************/ 
  
  //**********IIC0--RM3100--地磁检查**************
  GPIOE->DDR |=  GPIO_Pin_0 ;//SDA-0（PE0）为输出
  GPIOE->DDR |=  GPIO_Pin_1; //SCL-0（PE1）为输出 
  GPIOB->DDR |=  GPIO_Pin_0; //MEG_PRW，RM3100供电口线地磁芯片供电口,输出
  GPIOD->DDR &=(~GPIO_Pin_0);//PD0   为输入，3100地磁芯片中断状态输入2017.324
  GPIOD->CR1 &=(~GPIO_Pin_0);//3100地磁芯片 中断，浮空输入  2017.324
  GPIOB->CR1|= GPIO_Pin_0;   //MCU给RM3100供电口线，设为推免输出
  //*********************************************
  
  //***************LED--指示灯*****************************
  GPIOD->DDR |=  GPIO_Pin_6;//PD6为输出，LED1
  GPIOD->DDR |=  GPIO_Pin_5;//PD7为输出，LED2
  GPIOD->CR1 |=  GPIO_Pin_6;//PD6为推挽输出，LED1
  GPIOD->CR1 |=  GPIO_Pin_5;//PD7为推挽输出，LED2
  //*****************************************************
  
  //********************升压模块引脚*****************************
  GPIOD->DDR |= GPIO_Pin_7;		//BTPower
  GPIOD->DDR |= GPIO_Pin_4;		//NBiotPower	
  GPIOD->CR1 |= GPIO_Pin_7;
  GPIOD->CR1 |= GPIO_Pin_4;
  //*********************************************************

  //************NB模块复位引脚******************************
  GPIOA->DDR |=  GPIO_Pin_1; //RESET（PD7）为输出 ，NB复位脚
  GPIOA->CR1 |=  GPIO_Pin_1; //RESET（PD7）为推免输出，NB复位脚
  //*********************************************************
  
  //******************NFC中断引脚************************
  GPIOD->DDR &=  ~GPIO_Pin_3;
  GPIOD->CR1 &=  ~GPIO_Pin_3;//浮空输入（前面已经将GPIOD全设为输入）
  //*******************************************************
   
  KEY_EXTI_config();//配置PB0中断输入引脚中断唤醒cpu  
  
}

/************************************************************************************************
Function: void sleep(void)
Description: 
note：
Calls:          
Called By:      
Table Accessed: 
Table Updated:  
Input:               
Output:         
Return:       
Timers:  2015.12.08  
author:  ME 
************************************************************************************************/
void sleep(void)
{
	if(TagCng_symple.Config.TagNode.McuSleepFlag != 0x01)
	{
		return;
	}
	if(bluetooth_data.BTEnableFlag == 0x01)
	{
		return;
	}
	LED1_clr;LED2_clr;
	UART_disable();
	if (FastGetParkCount>0)
	{
		
		DelayCount=2*2400;
		FastGetParkCount--;
	}
	else
	{
		DelayCount = TagCng_symple.Config.TagPara.WdtInterval*2301;
	}
	MEG_PRW_clr;
	ROUSE_IRQ_set;													//开外部唤醒中断输入
	enableInterrupts();												//开中断,关中断动作在睡眠被唤醒后执行
	GoSleep(DelayCount);											//rtc睡眠时间
	inToSleep((Pcontroler_Symple)&TagCng_symple);					//进入睡眠
	ROUSE_IRQ_clr;													//关外部唤醒中断输入
	UART_enable();
}

/********************************************
*Name:void deep_sleep()
*Description:睡眠等待唤醒
*Input：NULL
*Output：NULL
*Author：GJL
*Date:2017/07/27
********************************************/ 
void deep_sleep(void)
{
	LED1_clr;LED2_clr;
	//BTPowerClr;
	NBiotPowerClr;
	UART_disable();
	disableUsartBT();
	MEG_PRW_clr;
	nop();nop();nop();nop();nop();
	nop();nop();nop();nop();nop();
	_HALT();
	nop();nop();nop();nop();nop();
	nop();nop();nop();nop();nop();
	UART_enable();
	enableUsartBT();
	BTPowerSet;
	NBiotPowerSet;
}

/************************************************************************************************
Function: void main()
Description: 
note：
Calls:          
Called By:      
Table Accessed: 
Table Updated:    
Input:               
Output:         
Return:       
Timers:  2015.12.08  
author:  ME 
********************************************************************************************/
void main()
{  
	TagID = 1006;    						// ID,3字节,16777200，0xFFFFF0为默认目标id，此id被标记为未初始化
	UserCode =10034;						//客户码，0为无效客户码，此客户码被标记为未初始化
	disableInterrupts();                   	//关全局中断
	inimcu();
        LED1_set;
        LED2_clr;
	init_UART();
	MyUsart_InitforBT();
        initUsartDebug();   //初始化调试串口
	TIM4_config();							
	TagParamInit((Pcontroler_Symple)&TagCng_symple);							//初始化内存变量
	PowerConfig();
	Startup_get_bottom = 1;                           							//开机初始化NB、获取地磁本底标志
	test_Product((Pcontroler_Symple)&TagCng_symple, (pNBiotAT)&NBAtCommand);	// 测试NB串口、地磁功能
	ROUSE_IRQ_set;
	MEG_PRW_set;
	enableInterrupts();
	NBAtCommand.NBEnableFlag = 0x01;
	TagCng_symple.Config.TagNode.McuSleepFlag = 0x01;
        //设置BT Name
        unsigned char setBtNameCmd[32];
        unsigned char tagIDStr[8];
        itoa(TagID, tagIDStr, 10);
        strcpy(setBtNameCmd, "AT+B SLDN ");
        strcat(setBtNameCmd, tagIDStr);
        strcat(setBtNameCmd, "\r\n");
        sendUsartData(USART_FOR_BT, setBtNameCmd, strlen(setBtNameCmd));
	DoNBiotATCmd((pNBiotAT)&NBAtCommand, AT_CMD, JUDGE_NONE, "", NBAtCommand.ATCMDData);	//执行NB指令
	DoNBiotATCmd((pNBiotAT)&NBAtCommand, AT_NRB, JUDGE_NONE, "", NBAtCommand.ATCMDData);
	DoNBiotATCmd((pNBiotAT)&NBAtCommand, AT_CMEE, JUDGE_NONE, "", NBAtCommand.ATCMDData);
        BTPowerClr;
        USART_Cmd(USART_FOR_BT, DISABLE);   //and by yjd
	while(1)
	{
		while(TagCng_symple.Config.Tag_status == ACTIVATE)
		{
			checkNBNetwork((pNBiotAT)&NBAtCommand);											//查询网络附着状态
			OutFromSleep((Pcontroler_Symple)&TagCng_symple);								//车位状态判断
			DoATCmdFromBT((pNBiotAT)&NBAtCommand);											//执行蓝牙配置指令
			//			deal_BT_data((pBT)&bluetooth_data);
                        sleep();
		}
		ROUSE_IRQ_set;											//开外部唤醒中断输入
                deep_sleep();															//出厂状态,睡眠等待唤醒
                ROUSE_IRQ_clr;
	}
}
