#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "stm8l15x_conf.h"
#include "struct.h"
#include "MyCommon.h"
#include "UART.h"

/********************************************
*Name:init_UART()
*Description:���ڳ�ʼ��
*Input��NULL
*Output��NULL
*Author��GJL
*Date:2017/07/3
********************************************/
void init_UART(void)
{
	GPIO_ExternalPullUpConfig(GPIOE,GPIO_Pin_7, ENABLE);//RX
	GPIO_ExternalPullUpConfig(GPIOE,GPIO_Pin_6, ENABLE);//TX         ����3  NBģ��
	USART_DeInit(UART_NB);
	USART_Init(UART_NB, (uint32_t)9600, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No,(USART_Mode_TypeDef)(USART_Mode_Rx|USART_Mode_Tx));//���ô��ڲ����ʡ��ֳ���ֹͣλ��У��λ���շ�ģʽ
	USART_ITConfig(UART_NB, USART_IT_RXNE, ENABLE);//ʹ�ܴ���3�����ж�
	USART_ITConfig(UART_NB, USART_IT_IDLE, ENABLE);//ʹ�ܴ��ڿ����жϣ�����һ֡���ʱ�����ж�
	USART_ITConfig(UART_NB, USART_IT_TC, DISABLE);//�رմ���3��������ж�
	USART_Cmd(UART_NB, ENABLE);//����3ʹ��
}

/**
  * disable uart NB
  */
void UART_disable(void)
{
	USART_Cmd(UART_NB, DISABLE);//����3�ر�
	GPIOE->CR1 &=  ~GPIO_Pin_7;//��������
}

/**
  * enable uart NB
  */
void UART_enable(void)
{
	GPIO_ExternalPullUpConfig(GPIOE,GPIO_Pin_7, ENABLE);//RX
	USART_Cmd(UART_NB, ENABLE);//����3ʹ��
}

/*
 * @name: 	MyUsart_InitforBT
 * @author:	HL
 * @date:	2017-11-24
 */
void MyUsart_InitforBT(void){
//	BTPortConfig();
	GPIO_ExternalPullUpConfig(GPIOA,GPIO_Pin_2, ENABLE);//Tx
	GPIO_ExternalPullUpConfig(GPIOA,GPIO_Pin_3, ENABLE);//Rx         ����3  NBģ��
	SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortA, ENABLE);		// USART1 routing to PA2 PA3
	CLK_PeripheralClockConfig(CLK_Peripheral_USART1, ENABLE);		// ��USART3 ����ʱ��
	USART_DeInit(USART_FOR_BT);
	USART_Init(USART_FOR_BT, (uint32_t)115200, USART_WordLength_8b, USART_StopBits_1, 
			   USART_Parity_No, (USART_Mode_TypeDef)(USART_Mode_Rx|USART_Mode_Tx));
	USART_ITConfig(USART_FOR_BT, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART_FOR_BT, USART_IT_IDLE, ENABLE);
//	USART_ITConfig(USART_FOR_BT, USART_IT_TC, DISABLE);
	USART_Cmd(USART_FOR_BT, ENABLE);
}

/*
 *@name: 	MyUsart_InitforNBIot
 *@author:	HL
 *@date:	2017-12-01
 */
void MyUsart_InitforNBIot(void){
	NBIotPortConfig();
	CLK_PeripheralClockConfig(CLK_Peripheral_USART3, ENABLE);	//��USART1 ����ʱ��
	USART_DeInit(USART_FOR_NB);
	USART_Init(USART_FOR_NB, (uint32_t)9600, USART_WordLength_8b, USART_StopBits_1, 
			   USART_Parity_No, (USART_Mode_TypeDef)(USART_Mode_Rx|USART_Mode_Tx));
	USART_ITConfig(USART_FOR_NB, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART_FOR_NB, USART_IT_IDLE, ENABLE);
	USART_Cmd(USART_FOR_NB, ENABLE);
}

/*
 * @name: 	disableUsartNBIot
 * @author:	HL
 * @date:	2017-11-24
 */
void disableUsartNBIot(void){
	CLK_PeripheralClockConfig(CLK_Peripheral_USART3, DISABLE);
	USART_Cmd(USART_FOR_NB, DISABLE);
	GPIO_Init(GPIOE, GPIO_Pin_7, GPIO_Mode_In_FL_No_IT);		//RX floating input
}

/*
 * @name: 	enableUsartNBIot
 * @author:	HL
 * @date:	2017-11-24
 */
void enableUsartNBIot(void){
	CLK_PeripheralClockConfig(CLK_Peripheral_USART3, ENABLE);
	USART_Cmd(USART_FOR_NB, ENABLE);
	GPIO_Init(GPIOE, GPIO_Pin_7, GPIO_Mode_In_PU_No_IT);
}

/*
 * @name: 	disableUsartBT
 * @author:	HL
 * @date:	2017-11-24
 */
void disableUsartBT(void){
	CLK_PeripheralClockConfig(CLK_Peripheral_USART1, DISABLE);
	USART_Cmd(USART_FOR_BT, DISABLE);
	GPIOA->CR1 &=  ~GPIO_Pin_3;//��������
}

/*
 * @name: 	enableUsartBT
 * @author:	HL
 * @date:	2017-11-24
 */
void enableUsartBT(void){
	CLK_PeripheralClockConfig(CLK_Peripheral_USART1, ENABLE);
	USART_Cmd(USART_FOR_BT, ENABLE);
	GPIO_ExternalPullUpConfig(GPIOA, GPIO_Pin_3, ENABLE);//Rx
}

/*
 * @name: 	sendUsartData
 * @author:	HL
 * @date:	2017-11-24
 */
void sendUsartData(USART_TypeDef* USARTx,char *pSendBuf, uint16_t len){
	uint16_t i = 0;
	for(i = 0; i < len; i++){
		while(!(USARTx->SR&USART_SR_TXE));
		USART_SendData8(USARTx,*pSendBuf++);
	}
}


/*
* @name: 	initUsartDebug
* @author:	YJD
* @date:	2018-03-22
*/
void initUsartDebug()
{ 
 CLK_PeripheralClockConfig(CLK_Peripheral_USART2, ENABLE);
 //GPIO_ExternalPullUpConfig(GPIOE,GPIO_Pin_3, ENABLE);//RX
 GPIO_Init(GPIOE,GPIO_Pin_3,GPIO_Mode_In_PU_IT);
 GPIO_ExternalPullUpConfig(GPIOE,GPIO_Pin_4, ENABLE);//TX         ����2   DEBUG
 USART_DeInit(UART_DEBUG);
 USART_Init(UART_DEBUG, (uint32_t)9600, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No,(USART_Mode_TypeDef)(USART_Mode_Rx|USART_Mode_Tx));//���ô��ڲ����ʡ��ֳ���ֹͣλ��У��λ���շ�ģʽ
 USART_ITConfig(UART_DEBUG, USART_IT_RXNE, ENABLE);//ʹ�ܴ���2�����ж�
 USART_ITConfig(UART_DEBUG, USART_IT_IDLE, ENABLE);//ʹ�ܴ��ڿ����жϣ�����һ֡���ʱ�����ж�
 //USART_ITConfig(USART2, USART_IT_TC, ENABLE);//�رմ���2��������ж�
 USART_Cmd(UART_DEBUG , ENABLE);//���Դ���ʹ��
  /*���ڼ�鴮��UART2�Ƿ������,���ʱ��TC�жϱ�־��λ���˳���ѯ�ȴ�*/
  while (USART_GetFlagStatus(UART_DEBUG, USART_FLAG_TC) == RESET);
}
/*ϵͳprintf����ʵ��*/
int putchar(int c)
{
  if('\n' == (char)c)
  {
    USART_SendData8(USART2, '\r');
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
  }
  USART_SendData8(USART2, c);
  while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);

  return (c);
}


void print(char* fmt, ...)
{
  double vargflt = 0;
  int  vargint = 0;
  char* vargpch = NULL;
  char vargch = 0;
  char* pfmt = NULL;
  va_list vp;
  va_start(vp, fmt);
  pfmt = fmt;
  while(*pfmt)
  {
    if(*pfmt == '%')
    {
      switch(*(++pfmt))
      {
        
      case 'c':
        vargch = va_arg(vp, int); 
        /*    va_arg(ap, type), if type is narrow type (char, short, float) an error is given in strict ANSI
        mode, or a warning otherwise.In non-strict ANSI mode, 'type' is allowed to be any expression. */
        printch(vargch);
        break;
      case 'd':
      case 'i':
        vargint = va_arg(vp, int);
        printdec(vargint);
        break;
      case 'f':
        vargflt = va_arg(vp, double);
        /*    va_arg(ap, type), if type is narrow type (char, short, float) an error is given in strict ANSI
        mode, or a warning otherwise.In non-strict ANSI mode, 'type' is allowed to be any expression. */
        printflt(vargflt);
        break;
      case 's':
        vargpch = va_arg(vp, char*);
        printstr(vargpch);
        break;
      case 'b':
      case 'B':
        vargint = va_arg(vp, int);
        printbin(vargint);
        break;
      case 'x':
      case 'X':
        vargint = va_arg(vp, int);
        printhex(vargint);
        break;
      case '%':
        printch('%');
        break;
      default:
        break;
      }
      pfmt++;
    }
    else
    {
      printch(*pfmt++);
    }
  }
  va_end(vp);
}
void printch(char ch)
{
  while(!(USART2->SR&USART_SR_TXE));
  USART_SendData8(USART2, ch);
}
void printdec(int dec)
{
  if(dec==0)
  {
    return;
  }
  printdec(dec/10);
  printch( (char)(dec%10 + '0'));
}
void printflt(double flt)
{
  int icnt = 0;
  int tmpint = 0;
  
  tmpint = (int)flt;
  printdec(tmpint);
  printch('.');
  flt = flt - tmpint;
  tmpint = (int)(flt * 1000000);
  printdec(tmpint);
}

void printstr(char* str)
{
  while(*str)
  {
    printch(*str++);
  }
}

void printbin(int bin)
{
  if(bin == 0)
  {
    printstr("0b");
    return;
  }
  printbin(bin/2);
  printch( (char)(bin%2 + '0'));
}
void printhex(int hex)
{
  if(hex==0)
  {
    printstr("0x");
    return;
  }
  printhex(hex/16);
  if(hex < 10)
  {
    printch((char)(hex%16 + '0'));
  }
  else
  {
    printch((char)(hex%16 - 10 + 'a' ));
  }
}
