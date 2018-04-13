#ifndef _McuTarger__H__
#define _McuTarger__H__

//#define NB05_01													//������NBIOTģ��
#define BC95_B5														//�Ϻ���ԶNBIOTģ��

#ifdef  NB05_01

#define LED1_clr       GPIOB->ODR&=(~GPIO_Pin_3)					//LED1�ر�
#define LED1_set       GPIOB->ODR|=(GPIO_Pin_3)						//LED1����
#define LED1_tgl       GPIOB->ODR^=(GPIO_Pin_3)						//LED1ȡ��

#define LED2_clr       GPIOB->ODR&=(~GPIO_Pin_2)					//LED2�ر�
#define LED2_set       GPIOB->ODR|=(GPIO_Pin_2)						//LED2����
#define LED2_tgl       GPIOB->ODR^=(GPIO_Pin_2)						//LED2ȡ��

#define ROUSE_IRQ_set       GPIOB->CR2|=GPIO_Pin_0;					//���ⲿ�����ж�����
#define ROUSE_IRQ_clr       GPIOB->CR2&=(~GPIO_Pin_0);				//���ⲿ�����ж�����

#define R3100_power_set   GPIOB->ODR|=(GPIO_Pin_1)					//�شŹ���ʹ��(MCP1824T�ߵ�ƽʹ��)
#define R3100_power_clr   GPIOB->ODR&=(~GPIO_Pin_1)					//�شŹ���ر�

#define BOOST_EN_clr		GPIOB->ODR&=(~GPIO_Pin_4)				//��ѹоƬ��ֹ
#define BOOST_EN_set		GPIOB->ODR|=GPIO_Pin_4      			//��ѹоƬʹ��

#define MEG_PRW_set    GPIOE->ODR|=GPIO_Pin_4       				//ʹ��MCU��IO�ڶԵش�RM3100����
#define MEG_PRW_clr    GPIOE->ODR &= (~GPIO_Pin_4)  				//��ֹMCU��IO�ڶԵش�RM3100����

#define ILIM0_set     GPIOB->ODR|=GPIO_Pin_5
#define ILIM0_clr     GPIOB->ODR &= (~GPIO_Pin_5) 

#elif defined (BC95_B5)

#define LED1_clr       GPIOD->ODR&=(~GPIO_Pin_6)					//LED1�ر�
#define LED1_set       GPIOD->ODR|=(GPIO_Pin_6)						//LED1����
#define LED1_tgl       GPIOD->ODR^=(GPIO_Pin_6)						//LED1ȡ��

#define LED2_clr       GPIOD->ODR&=(~GPIO_Pin_5)					//LED2�ر�
#define LED2_set       GPIOD->ODR|=(GPIO_Pin_5)						//LED2����
#define LED2_tgl       GPIOD->ODR^=(GPIO_Pin_5)						//LED2ȡ��

#define ROUSE_IRQ_set       GPIOD->CR2|=GPIO_Pin_3;					//���ⲿ�����ж�����
#define ROUSE_IRQ_clr       GPIOD->CR2&=(~GPIO_Pin_3);				//���ⲿ�����ж�����

#define BTPowerSet		GPIOD->ODR |= GPIO_Pin_7					// ������Դʹ��PD7
#define BTPowerClr		GPIOD->ODR &= (~GPIO_Pin_7)

#define NBiotPowerSet	GPIOD->ODR |= GPIO_Pin_4					// NBiot��Դʹ��PD4
#define	NBiotPowerClr	GPIOD->ODR &= (~GPIO_Pin_4)		

#define RESET_NB_set    GPIOA->ODR |= GPIO_Pin_4       				//��λNBģ��ʹ��  
#define RESET_NB_clr    GPIOA->ODR &= (~GPIO_Pin_4)    				//��λNBģ��ʧ��

#define MEG_PRW_set    GPIOB->ODR|=GPIO_Pin_0       				//ʹ��MCU��IO�ڶԵش�RM3100����
#define MEG_PRW_clr    GPIOB->ODR &= (~GPIO_Pin_0)  				//��ֹMCU��IO�ڶԵش�RM3100����

#endif

/* �شŶ˿����� */
#define R3100_IRQ_set       GPIOD->CR2|=GPIO_Pin_0;					//���ش�оƬ�ж�����
#define R3100_IRQ_clr       GPIOD->CR2&=(~GPIO_Pin_0);				//�صش�оƬ�ж�����
#define GetRdy()           (GPIOD->IDR & GPIO_Pin_0)				//�شŴ���������ж�

#define SCL_SET()	(GPIOE->DDR &= (~GPIO_Pin_1))  					//ģ��IIC0 ʱ�������
#define SCL_CLR()	(GPIOE->DDR |= GPIO_Pin_1)      				//ģ��IIC0 ʱ��������

#define SDA_SET()	(GPIOE->DDR &= (~GPIO_Pin_0))   				//ģ��IIC0 ���������
#define SDA_CLR()	(GPIOE->DDR |= GPIO_Pin_0)      				//ģ��IIC0 ����������

#define SDA_READ()	(GPIOE->IDR & GPIO_Pin_0)       				//�����������ֵ
#define IO_IIC_INIT()	GPIOE->ODR &= (~(GPIO_Pin_1|GPIO_Pin_0)) 	//IIC0�����ʼ��

#endif 



