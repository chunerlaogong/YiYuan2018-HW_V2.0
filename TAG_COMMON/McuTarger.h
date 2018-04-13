#ifndef _McuTarger__H__
#define _McuTarger__H__

//#define NB05_01													//利尔达NBIOT模组
#define BC95_B5														//上海移远NBIOT模组

#ifdef  NB05_01

#define LED1_clr       GPIOB->ODR&=(~GPIO_Pin_3)					//LED1关闭
#define LED1_set       GPIOB->ODR|=(GPIO_Pin_3)						//LED1开启
#define LED1_tgl       GPIOB->ODR^=(GPIO_Pin_3)						//LED1取反

#define LED2_clr       GPIOB->ODR&=(~GPIO_Pin_2)					//LED2关闭
#define LED2_set       GPIOB->ODR|=(GPIO_Pin_2)						//LED2开启
#define LED2_tgl       GPIOB->ODR^=(GPIO_Pin_2)						//LED2取反

#define ROUSE_IRQ_set       GPIOB->CR2|=GPIO_Pin_0;					//开外部唤醒中断输入
#define ROUSE_IRQ_clr       GPIOB->CR2&=(~GPIO_Pin_0);				//关外部唤醒中断输入

#define R3100_power_set   GPIOB->ODR|=(GPIO_Pin_1)					//地磁供电使能(MCP1824T高电平使能)
#define R3100_power_clr   GPIOB->ODR&=(~GPIO_Pin_1)					//地磁供电关闭

#define BOOST_EN_clr		GPIOB->ODR&=(~GPIO_Pin_4)				//升压芯片禁止
#define BOOST_EN_set		GPIOB->ODR|=GPIO_Pin_4      			//升压芯片使能

#define MEG_PRW_set    GPIOE->ODR|=GPIO_Pin_4       				//使能MCU的IO口对地磁RM3100供电
#define MEG_PRW_clr    GPIOE->ODR &= (~GPIO_Pin_4)  				//禁止MCU的IO口对地磁RM3100供电

#define ILIM0_set     GPIOB->ODR|=GPIO_Pin_5
#define ILIM0_clr     GPIOB->ODR &= (~GPIO_Pin_5) 

#elif defined (BC95_B5)

#define LED1_clr       GPIOD->ODR&=(~GPIO_Pin_6)					//LED1关闭
#define LED1_set       GPIOD->ODR|=(GPIO_Pin_6)						//LED1开启
#define LED1_tgl       GPIOD->ODR^=(GPIO_Pin_6)						//LED1取反

#define LED2_clr       GPIOD->ODR&=(~GPIO_Pin_5)					//LED2关闭
#define LED2_set       GPIOD->ODR|=(GPIO_Pin_5)						//LED2开启
#define LED2_tgl       GPIOD->ODR^=(GPIO_Pin_5)						//LED2取反

#define ROUSE_IRQ_set       GPIOD->CR2|=GPIO_Pin_3;					//开外部唤醒中断输入
#define ROUSE_IRQ_clr       GPIOD->CR2&=(~GPIO_Pin_3);				//关外部唤醒中断输入

#define BTPowerSet		GPIOD->ODR |= GPIO_Pin_7					// 蓝牙电源使能PD7
#define BTPowerClr		GPIOD->ODR &= (~GPIO_Pin_7)

#define NBiotPowerSet	GPIOD->ODR |= GPIO_Pin_4					// NBiot电源使能PD4
#define	NBiotPowerClr	GPIOD->ODR &= (~GPIO_Pin_4)		

#define RESET_NB_set    GPIOA->ODR |= GPIO_Pin_4       				//复位NB模块使能  
#define RESET_NB_clr    GPIOA->ODR &= (~GPIO_Pin_4)    				//复位NB模块失能

#define MEG_PRW_set    GPIOB->ODR|=GPIO_Pin_0       				//使能MCU的IO口对地磁RM3100供电
#define MEG_PRW_clr    GPIOB->ODR &= (~GPIO_Pin_0)  				//禁止MCU的IO口对地磁RM3100供电

#endif

/* 地磁端口配置 */
#define R3100_IRQ_set       GPIOD->CR2|=GPIO_Pin_0;					//开地磁芯片中断输入
#define R3100_IRQ_clr       GPIOD->CR2&=(~GPIO_Pin_0);				//关地磁芯片中断输入
#define GetRdy()           (GPIOD->IDR & GPIO_Pin_0)				//地磁传感器输出中断

#define SCL_SET()	(GPIOE->DDR &= (~GPIO_Pin_1))  					//模拟IIC0 时钟线输出
#define SCL_CLR()	(GPIOE->DDR |= GPIO_Pin_1)      				//模拟IIC0 时钟线输入

#define SDA_SET()	(GPIOE->DDR &= (~GPIO_Pin_0))   				//模拟IIC0 数据线输出
#define SDA_CLR()	(GPIOE->DDR |= GPIO_Pin_0)      				//模拟IIC0 数据线输入

#define SDA_READ()	(GPIOE->IDR & GPIO_Pin_0)       				//读数据线输出值
#define IO_IIC_INIT()	GPIOE->ODR &= (~(GPIO_Pin_1|GPIO_Pin_0)) 	//IIC0输出初始化

#endif 



