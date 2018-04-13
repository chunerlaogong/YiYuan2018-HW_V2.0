/**
  ******************************************************************************
  * @file    ADC/ADC_IT/stm8l15x_it.c
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    13-May-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x_it.h"
//#include "Common.h"
#include "eemFlash.h"
#include "UART.h"
#include "struct.h"
#include "compilefiles.h"
#include "McuTarger.h"
#include "MyNBIOT.h"
#include "Bluetooth.h"
#include "TIM4Drv.h"	  

unsigned char datatemp;
//extern sNB NB_BC95;
extern sNBiotAT NBAtCommand;
extern sBT bluetooth_data;
extern Sontroler_Symple TagCng_symple;
extern uint32_t TIM4_Count;
//extern void writeFlashParameter(PcontrolerConfig TagCng);
unsigned char HandWakeFlag=0;
unsigned char Test_Product_Flag = 0;


/** @addtogroup STM8L15x_StdPeriph_Examples
  * @{
  */

/** @addtogroup ADC_IT
  * @{
  */

unsigned char xxx = 0;
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint16_t ADCdata;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/

#ifdef _COSMIC_
/**
  * @brief Dummy interrupt routine
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(NonHandledInterrupt, 0)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
#endif

/**
  * @brief TRAP interrupt routine
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER_TRAP(TRAP_IRQHandler)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
/**
  * @brief FLASH Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(FLASH_IRQHandler, 1)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
/**
  * @brief DMA1 channel0 and channel1 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(DMA1_CHANNEL0_1_IRQHandler, 2)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
/**
  * @brief DMA1 channel2 and channel3 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(DMA1_CHANNEL2_3_IRQHandler, 3)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
/**
  * @brief RTC / CSS_LSE Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(RTC_CSSLSE_IRQHandler, 4)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  
  RTC_WakeUpCmd(DISABLE);
  RTC_ClearFlag(RTC_FLAG_WUTF);
  RTC_ClearFlag(RTC_FLAG_ALRAF);
}
/**
  * @brief External IT PORTE/F and PVD Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTIE_F_PVD_IRQHandler, 5)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief External IT PORTB / PORTG Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTIB_G_IRQHandler, 6)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief External IT PORTD /PORTH Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTID_H_IRQHandler, 7)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief External IT PIN0 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI0_IRQHandler, 8)
{
	disableInterrupts();
	nop();
	nop();
	if((GPIOD->IDR & 0x01)==0x01) //再判断是否按下   
	{   
		nop();
	}

	nop();
	EXTI_ClearITPendingBit(EXTI_IT_Pin0);
	//GPIO_Init(GPIOB, GPIO_Pin_0,GPIO_Mode_In_PU_No_IT);
	EXTI_SetPinSensitivity(EXTI_Pin_0, EXTI_Trigger_Rising);
	nop();
	enableInterrupts();
	/* In order to detect unexpected events during development,
	it is recommended to set a breakpoint on the following instruction.
	*/
}
/**
  * @brief External IT PIN1 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI1_IRQHandler, 9)
{
	/* In order to detect unexpected events during development,
	it is recommended to set a breakpoint on the following instruction.
	*/
	EXTI_ClearITPendingBit(EXTI_IT_Pin1);
}

/**
  * @brief External IT PIN2 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI2_IRQHandler, 10)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
    
}

/**
  * @brief External IT PIN3 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI3_IRQHandler, 11)
{
	/* In order to detect unexpected events during development,
	it is recommended to set a breakpoint on the following instruction.
	*/
	nop();nop();nop();nop();
	if((GPIOD->IDR & 0x08) == 0x08) 
	{
//		if(TagCng_symple.Config.Tag_status == LEAVE_FACTORY)
//		{
//			TagCng_symple.Config.Tag_status = ACTIVATE;
//			TagCng_symple.Config.Tag_initNB = ACTIVATE;
//			WriteTagParam((Pcontroler_Symple)&TagCng_symple);
//			Test_Product_Flag = 1;
////			NBiotPowerSet;					//打开NB电源
//		}
//		else{
//			HandWakeFlag=1;
//		}
		BTPowerSet;
		print("BTPowerSet......\n");
        USART_Cmd(USART_FOR_BT, ENABLE);
		Test_Product_Flag = 1;
		NBAtCommand.NBEnableFlag = 0x00;
		TagCng_symple.Config.TagNode.McuSleepFlag = 0x00;
		TIM4_Cmd(ENABLE);
		bluetooth_data.BTEnableFlag = 0x01;
		LED1_set;
	}   
	EXTI_SetPinSensitivity(EXTI_Pin_3, EXTI_Trigger_Rising);
	EXTI_ClearITPendingBit(EXTI_IT_Pin3);
}

/**
  * @brief External IT PIN4 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI4_IRQHandler, 12)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief External IT PIN5 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI5_IRQHandler, 13)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
   EXTI_ClearITPendingBit(EXTI_IT_Pin5);
}

/**
  * @brief External IT PIN6 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI6_IRQHandler, 14)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief External IT PIN7 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI7_IRQHandler, 15)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
/**
  * @brief LCD /AES Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(LCD_AES_IRQHandler, 16)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
/**
  * @brief CLK switch/CSS/TIM1 break Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(SWITCH_CSS_BREAK_DAC_IRQHandler, 17)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief ADC1/Comparator Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(ADC1_COMP_IRQHandler, 18)
{
	/* In order to detect unexpected events during development,
	it is recommended to set a breakpoint on the following instruction.
	*/
	/* Get ADC convertion value */
	ADCdata = ADC_GetConversionValue(ADC1);
}

/**
  * @brief TIM2 Update/Overflow/Trigger/Break /USART2 TX Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQHandler, 19)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
//  TimingDelay_Decrement();
//  TIM2_ClearITPendingBit(TIM2_IT_Update);
}

/**
  * @brief Timer2 Capture/Compare / USART2 RX Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM2_CC_USART2_RX_IRQHandler, 20)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}


/**
  * @brief Timer3 Update/Overflow/Trigger/Break Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM3_UPD_OVF_TRG_BRK_USART3_TX_IRQHandler, 21)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
/**
  * @brief Timer3 Capture/Compare /USART3 RX Interrupt routine.
  * @param  None
  * @retval None
  */

INTERRUPT_HANDLER(TIM3_CC_USART3_RX_IRQHandler, 22)
{
	/* In order to detect unexpected events during development,
	it is recommended to set a breakpoint on the following instruction.
	*/
	volatile unsigned char clear;//定义为valotile是为了不然编译器优化导致产生clear定义却未使用的警告
	if(USART_GetITStatus(USART_FOR_NB, USART_IT_IDLE)!=RESET)
	{
		clear = USART_FOR_NB->SR;
		clear = USART_FOR_NB->DR;
		judge_NBIOT_Rx_Buff((pNBiotAT)&NBAtCommand);
	}
	if(USART_GetITStatus(USART_FOR_NB, USART_IT_RXNE)!=RESET)
	{
		datatemp= USART_ReceiveData8(USART_FOR_NB);
		NBAtCommand.ATReturn[NBAtCommand.RxCnt++] = datatemp;
	}
}
/**
  * @brief TIM1 Update/Overflow/Trigger/Commutation Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM1_UPD_OVF_TRG_COM_IRQHandler, 23)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  TIM1_ClearFlag(TIM1_FLAG_Update);
}
/**
  * @brief TIM1 Capture/Compare Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM1_CC_IRQHandler, 24)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief TIM4 Update/Overflow/Trigger Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM4_UPD_OVF_TRG_IRQHandler, 25)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  /* Cleat Interrupt Pending bit */
  TIM4_ClearITPendingBit(TIM4_IT_Update);
  if(TIM4_Count++ > TIM4_DELAY_COUNT)
  {
	  TIM4_Count = 0;
	  TagCng_symple.Config.TagNode.McuSleepFlag = 0x01;
	  NBAtCommand.NBEnableFlag = 0x01;
	  TIM4_Cmd(DISABLE);
	  bluetooth_data.BTEnableFlag = 0x00;
	  print("BTPowerClr......\n");
	  BTPowerClr;   
      USART_Cmd(USART_FOR_BT, DISABLE);  //and by yjd
	  LED1_clr;
  }
}
/**
  * @brief SPI1 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(SPI1_IRQHandler, 26)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief USART1 TX / TIM5 Update/Overflow/Trigger/Break Interrupt  routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(USART1_TX_TIM5_UPD_OVF_TRG_BRK_IRQHandler, 27)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief USART1 RX / Timer5 Capture/Compare Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(USART1_RX_TIM5_CC_IRQHandler, 28)
{
	/* In order to detect unexpected events during development,
	it is recommended to set a breakpoint on the following instruction.
	*/
	disableInterrupts();
	volatile uint8_t usart1_data;
	if(USART_GetITStatus(USART1, USART_IT_IDLE)!=RESET){		//
		usart1_data = USART_ReceiveData8(USART1);				// clear receive buff
		USART_ClearITPendingBit(USART1, USART_IT_IDLE);
		judge_BT_buffer((pBT)&bluetooth_data);
		deal_BT_data((pBT)&bluetooth_data);
	}
	if(USART_GetITStatus(USART1, USART_IT_RXNE)!=RESET){	//
		usart1_data = USART_ReceiveData8(USART1);
		bluetooth_data.BT_RX_BUF_TMP[bluetooth_data.BT_RX_CNT++] = usart1_data;
	}
	else{
		usart1_data = USART_ReceiveData8(USART1);				// clear receive buff
		USART_ClearITPendingBit(USART1, USART_IT_IDLE);
	}
	enableInterrupts();
}

/**
  * @brief I2C1 / SPI2 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(I2C1_SPI2_IRQHandler, 29)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @}
  */

/**
  * @}
  */
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/