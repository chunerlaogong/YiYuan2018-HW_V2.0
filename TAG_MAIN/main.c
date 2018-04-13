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
U16 DelayCount;//˯��ʱ��
long second_flag=0;
extern unsigned char HandWakeFlag;
extern unsigned int UserCode;//�ͻ���
extern unsigned long TagID;    // ID,3�ֽڣ����16777215��

void sleep(void);
extern uint16_t ADCdata; //adc�ɼ�����
extern void testRomOperate(Pcontroler_Symple TagCng);
extern void Gather_DataOfR3100();//ȡ�ش�����
extern void EMDealGeomagneticValue_VectorDifference(void);//����ش�����
extern U8 start_get_magnetic_bottom(Pcontroler_Symple TagCng);
extern signed int EMData_x_Vary;//x��仯�н�
extern signed int EMData_y_Vary;//y��仯�н�
extern signed int EMData_z_Vary;//z��仯�н�

extern SSensor3100  Sensor3100L;

extern U8 FastGetParkCount;//���ٻ�ȡ����״̬�������������Ϊ��ʱ��Ϊǰ�泵���ո��뿪����ʱ������������ֹ��������λ����
sNBiotAT NBAtCommand;
sNB	NB_BC95;
sBT bluetooth_data;
char ATCmdFromBTData[30] = {0};
unsigned char Startup_get_bottom = 0;
uint32_t TIM4_Count = 0;
/************************************************************************************************
Function: void inimcu(void) 
Description: ��ʼ��MCU
note��
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
  CLK_RTC_Ini();//RTCʱ�ӳ�ʼ��
  CLK->ICKCR = 0x15;//Internal clock register,MVR regulator OFF in HALT/Active-halt mode
  PWR->CSR2 = 0x2;
  CLK->CKDIVR = 0; //System clock source   16M
  
  GPIOA->DDR = 0xFF;//����ģʽ
  GPIOA->CR1 = 0x00;//����ģʽ�£���������������
  GPIOA->CR2 = 0x00;//����ģʽ�£���ֹ�ж�
  GPIOA->ODR = 0x00;
  
  GPIOB->DDR = 0xFF;//����ģʽ
  GPIOB->CR1 = 0x00;//����ģʽ�£���������������
  GPIOB->CR2 = 0x00;//����ģʽ�£���ֹ�ж�
  GPIOB->ODR = 0x00;
  
  
  GPIOC->DDR = 0xFF;//����ģʽ
  GPIOC->CR1 = 0x00;//����ģʽ�£���������������
  GPIOC->CR2 = 0x00;//����ģʽ�£���ֹ�ж� 
  GPIOC->ODR = 0x00;
  
  GPIOD->DDR = 0xFF;//����ģʽ
  GPIOD->CR1 = 0x00;//����ģʽ�£���������������
  GPIOD->CR2 = 0x00;//����ģʽ�£���ֹ�ж�  
  GPIOD->ODR = 0x00;
  
  GPIOE->DDR = 0xFF;//����ģʽ
  GPIOE->CR1 = 0x00;//����ģʽ�£���������������
  GPIOE->CR2 = 0x00;//����ģʽ�£���ֹ�ж�
  GPIOE->ODR = 0x00;
  
  GPIOF->DDR = 0xFF;//����ģʽ
  GPIOF->CR1 = 0x00;//����ģʽ�£���������������
  GPIOF->CR2 = 0x00;//����ģʽ�£���ֹ�ж� 
  GPIOF->ODR = 0x00;
  
  /**********����ռ���嵥*********************/
  
  //****************����UART3--NB�ӿ�************
  GPIOE->DDR|=GPIO_Pin_6;//UART3-TX���
  GPIOE->DDR&=(~GPIO_Pin_7);//UART3-RX����
  //*********************************************

  //*******************BLE***************** 
  GPIOA->DDR|=GPIO_Pin_2;//UART1-TX���
  GPIOA->DDR&=(~GPIO_Pin_3);//UART1-RX����
  //*******************************************/ 
  
  //**********IIC0--RM3100--�شż��**************
  GPIOE->DDR |=  GPIO_Pin_0 ;//SDA-0��PE0��Ϊ���
  GPIOE->DDR |=  GPIO_Pin_1; //SCL-0��PE1��Ϊ��� 
  GPIOB->DDR |=  GPIO_Pin_0; //MEG_PRW��RM3100������ߵش�оƬ�����,���
  GPIOD->DDR &=(~GPIO_Pin_0);//PD0   Ϊ���룬3100�ش�оƬ�ж�״̬����2017.324
  GPIOD->CR1 &=(~GPIO_Pin_0);//3100�ش�оƬ �жϣ���������  2017.324
  GPIOB->CR1|= GPIO_Pin_0;   //MCU��RM3100������ߣ���Ϊ�������
  //*********************************************
  
  //***************LED--ָʾ��*****************************
  GPIOD->DDR |=  GPIO_Pin_6;//PD6Ϊ�����LED1
  GPIOD->DDR |=  GPIO_Pin_5;//PD7Ϊ�����LED2
  GPIOD->CR1 |=  GPIO_Pin_6;//PD6Ϊ���������LED1
  GPIOD->CR1 |=  GPIO_Pin_5;//PD7Ϊ���������LED2
  //*****************************************************
  
  //********************��ѹģ������*****************************
  GPIOD->DDR |= GPIO_Pin_7;		//BTPower
  GPIOD->DDR |= GPIO_Pin_4;		//NBiotPower	
  GPIOD->CR1 |= GPIO_Pin_7;
  GPIOD->CR1 |= GPIO_Pin_4;
  //*********************************************************

  //************NBģ�鸴λ����******************************
  GPIOA->DDR |=  GPIO_Pin_1; //RESET��PD7��Ϊ��� ��NB��λ��
  GPIOA->CR1 |=  GPIO_Pin_1; //RESET��PD7��Ϊ���������NB��λ��
  //*********************************************************
  
  //******************NFC�ж�����************************
  GPIOD->DDR &=  ~GPIO_Pin_3;
  GPIOD->CR1 &=  ~GPIO_Pin_3;//�������루ǰ���Ѿ���GPIODȫ��Ϊ���룩
  //*******************************************************
   
  KEY_EXTI_config();//����PB0�ж����������жϻ���cpu  
  
}

/************************************************************************************************
Function: void sleep(void)
Description: 
note��
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
	ROUSE_IRQ_set;													//���ⲿ�����ж�����
	enableInterrupts();												//���ж�,���ж϶�����˯�߱����Ѻ�ִ��
	GoSleep(DelayCount);											//rtc˯��ʱ��
	inToSleep((Pcontroler_Symple)&TagCng_symple);					//����˯��
	ROUSE_IRQ_clr;													//���ⲿ�����ж�����
	UART_enable();
}

/********************************************
*Name:void deep_sleep()
*Description:˯�ߵȴ�����
*Input��NULL
*Output��NULL
*Author��GJL
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
note��
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
	TagID = 1006;    						// ID,3�ֽ�,16777200��0xFFFFF0ΪĬ��Ŀ��id����id�����Ϊδ��ʼ��
	UserCode =10034;						//�ͻ��룬0Ϊ��Ч�ͻ��룬�˿ͻ��뱻���Ϊδ��ʼ��
	disableInterrupts();                   	//��ȫ���ж�
	inimcu();
        LED1_set;
        LED2_clr;
	init_UART();
	MyUsart_InitforBT();
        initUsartDebug();   //��ʼ�����Դ���
	TIM4_config();							
	TagParamInit((Pcontroler_Symple)&TagCng_symple);							//��ʼ���ڴ����
	PowerConfig();
	Startup_get_bottom = 1;                           							//������ʼ��NB����ȡ�شű��ױ�־
	test_Product((Pcontroler_Symple)&TagCng_symple, (pNBiotAT)&NBAtCommand);	// ����NB���ڡ��شŹ���
	ROUSE_IRQ_set;
	MEG_PRW_set;
	enableInterrupts();
	NBAtCommand.NBEnableFlag = 0x01;
	TagCng_symple.Config.TagNode.McuSleepFlag = 0x01;
        //����BT Name
        unsigned char setBtNameCmd[32];
        unsigned char tagIDStr[8];
        itoa(TagID, tagIDStr, 10);
        strcpy(setBtNameCmd, "AT+B SLDN ");
        strcat(setBtNameCmd, tagIDStr);
        strcat(setBtNameCmd, "\r\n");
        sendUsartData(USART_FOR_BT, setBtNameCmd, strlen(setBtNameCmd));
	DoNBiotATCmd((pNBiotAT)&NBAtCommand, AT_CMD, JUDGE_NONE, "", NBAtCommand.ATCMDData);	//ִ��NBָ��
	DoNBiotATCmd((pNBiotAT)&NBAtCommand, AT_NRB, JUDGE_NONE, "", NBAtCommand.ATCMDData);
	DoNBiotATCmd((pNBiotAT)&NBAtCommand, AT_CMEE, JUDGE_NONE, "", NBAtCommand.ATCMDData);
        BTPowerClr;
        USART_Cmd(USART_FOR_BT, DISABLE);   //and by yjd
	while(1)
	{
		while(TagCng_symple.Config.Tag_status == ACTIVATE)
		{
			checkNBNetwork((pNBiotAT)&NBAtCommand);											//��ѯ���總��״̬
			OutFromSleep((Pcontroler_Symple)&TagCng_symple);								//��λ״̬�ж�
			DoATCmdFromBT((pNBiotAT)&NBAtCommand);											//ִ����������ָ��
			//			deal_BT_data((pBT)&bluetooth_data);
                        sleep();
		}
		ROUSE_IRQ_set;											//���ⲿ�����ж�����
                deep_sleep();															//����״̬,˯�ߵȴ�����
                ROUSE_IRQ_clr;
	}
}
