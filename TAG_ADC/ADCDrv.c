#include "stm8l15x_conf.h"
#include "ADCDrv.h"

void VRAD_init(void)
{     
	CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE);    /* Enable ADC1 clock */
	ADC_DeInit(ADC1);			//初始化ADC1
	ADC_Init(ADC1, ADC_ConversionMode_Single, ADC_Resolution_12Bit, ADC_Prescaler_2);/* Initialize and configure ADC1 */
	ADC_SamplingTimeConfig(ADC1, ADC_Group_SlowChannels, ADC_SamplingTime_384Cycles);/* ADC channel used for vdd measurement */
	ADC_VrefintCmd(ENABLE);		//启用内部参考电压
	ADC_Cmd(ADC1, ENABLE);		/* Enable ADC1 */
	ADC_ChannelCmd(ADC1, ADC_Channel_Vrefint, ENABLE); /* Enable ADC1 Channel used for Vref measurement */ 
}
void AD_init(void)
{
	CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE);    /* Enable ADC1 clock */
	ADC_DeInit(ADC1);			//初始化ADC1
	ADC_Init(ADC1, ADC_ConversionMode_Single, ADC_Resolution_12Bit, ADC_Prescaler_2);/* Initialize and configure ADC1 */
	ADC_SamplingTimeConfig(ADC1, ADC_Group_SlowChannels, ADC_SamplingTime_384Cycles);/* ADC channel used for AD measurement */
	ADC_Cmd(ADC1, ENABLE);		/* Enable ADC1 */
}

unsigned short VREF_Value(void)
{
	unsigned short i;
	unsigned short res=0;
	for(i=8;i>0;i--)/*8次平均*/
	{
		ADC_SoftwareStartConv(ADC1);//开始转换
		while(!(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)));//是否转换结束
		res+=ADC_GetConversionValue(ADC1);//读adc转换值
	}  
	return(res>>3); 
}

/*ADC 模块使用完必须关闭，否则会有1mA 电流消耗*/
unsigned short Battery_Read()
{
	unsigned short VADC=0;
	VRAD_init();
	VADC = VREF_Value();
	ADC_VrefintCmd(DISABLE); //关闭内部参考电压
	ADC_ChannelCmd(ADC1, ADC_Channel_Vrefint, DISABLE); /* disable ADC1 Channel used for Vref measurement */ 
	ADC_Cmd(ADC1, DISABLE);/* disable ADC1 */
	CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, DISABLE); 
	return VADC;
}
