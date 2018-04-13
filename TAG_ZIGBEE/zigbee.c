
#include "zigbee.h"
#include "Cyrf6936.h"
#include "protocol.h"
#include  "McuTarger.h"
#include "Common.h"
#include "struct.h"
#include "compilefiles.h"

unsigned char RfBufftext[50]={0x00};
void flashLeds(unsigned char ledNum,unsigned char ledflags);
#ifndef NopFunction
#define NopFunction	1

#endif


#ifndef TRUE
#define TRUE    0x01
#endif

#ifndef FALSE
#define FALSE   0x00
#endif

extern U8 RFCloseFlag;//rf关闭标志，0以外任何值--------需要关闭
extern unsigned char SendNodeCount;//心跳包发送节点计数
void send_string(unsigned char *s,unsigned int length);
extern unsigned int get_RspwaitData(unsigned char selec);
extern unsigned char first_colisionflag;
extern void SpiWirteByteBuff(U8 cAddr,U8* pBuff,U8 cLen);

extern unsigned char  SendPackCount;//发报警包控制数，用于控制发包的时间间隔
//#define RF_RESET PD_ODR_ODR5;
extern void WatchTimerSet(unsigned char temp);

extern void ReadParamFrmFlash(Pcontroler_Symple TagCng);
extern void MiniSleep(U8 cTimes);
#if	DEBUG
volatile unsigned int AckRecSucessCount=0,RecCount,SendCount=0;
volatile unsigned char SendAckCount=0;
extern void Wdt_ini_RF(Pcontroler_Symple TagCng);
extern unsigned char channel1_rate;    // 1通道频率

#endif

void waitSpitms(unsigned char tlms);
U8 CrashCount=0;//碰撞计次，用来统计碰撞次数
extern void SpiInit();//初始化SPI

const unsigned char Data_PNCODE_table[4][16] =
{
   {0x83,0xF7,0xA8,0x2D,0x7A,0x44,0x64,0xD3,
   0x3F,0x2C,0x4E,0xAA,0x71,0x48,0x7A,0xC9,},

   {0x17,0xFF,0x9E,0x21,0x36,0x90,0xC7,0x82,
   0xA6,0x46,0xB5,0x9A,0x3A,0x30,0xB6,0xAD,},

   {0xBC,0x5D,0x9A,0x5B,0xEE,0x7F,0x42,0xEB,
   0x24,0xF5,0xDD,0xF8,0x7A,0x77,0x74,0xE7,},

   {0x3D,0x70,0x7C,0x94,0xDC,0x84,0xAD,0x95,
   0x1E,0x6A,0xF0,0x37,0x52,0x7B,0x11,0xD4,},

};

const unsigned char  SOP_PNCODE_table[12][8] =
{
    {0xB7, 0x8F, 0x2B, 0x01, 0xEF, 0x76, 0x0D, 0x01,},     // PN Code 0  
    {0x82, 0xC7, 0x90, 0x36, 0x21, 0x9e, 0xFF, 0x17,},     // PN Code 1 default
    {0xE3, 0x5C, 0xFA, 0x02, 0x97, 0x93, 0xF9, 0x02,},     // PN Code 2
    {0xCB, 0x52, 0x77, 0x03, 0x1A, 0xBF, 0x74, 0x03,},     // PN Code 3
    {0xBF, 0x6B, 0x90, 0x06, 0xF3, 0x8E, 0x8D, 0x06,},     // PN Code 4
    {0x4F, 0x17, 0xEC, 0x09, 0xAD, 0xA4, 0xEB, 0x09,},     // PN Code 5
    {0xAD, 0x39, 0xA2, 0x0F, 0x9B, 0xC5, 0xA1, 0x0F,},     // PN Code 6
    {0x3A, 0x6F, 0x89, 0x1C, 0xDF, 0xD4, 0x88, 0x1C,},     // PN Code 7
    {0xD2, 0x8F, 0xB1, 0x2A, 0xEF, 0x64, 0xB0, 0x2A,},     // PN Code 8
    {0x66, 0xCD, 0x7C, 0x50, 0xDD, 0x26, 0x7C, 0x50,},     // PN Code 9
    {0xE2, 0xF8, 0xCC, 0x91, 0x3C, 0x37, 0xCC, 0x91,},     // PN Code 10
    {0xB2, 0x3D, 0x0D, 0xA5, 0xDF, 0x83, 0x0C, 0xA5,},     // PN Code 11
};
 const unsigned char PREAM_pn_code_table[12][3] = 
{
    {0x02, 0xF7, 0x02},
	{0x02, 0x7A, 0x02},
	{0x02, 0xD3, 0x3F},
	{0x02, 0x4E, 0xAA}, 
	{0x02, 0x48, 0x7A},
	{0x02, 0x17, 0xFF},
	{0x02, 0x21, 0x36},
	{0x02, 0xC7, 0x82},
    {0x02, 0x46, 0xB5},
	{0x02, 0x3A, 0x30},
	{0x02, 0xAD, 0xBC},
	{0x02, 0x9A, 0x5B},
	//{0x02, 0x33, 0x33},
	//{0x02, 0x24, 0xF5},

};

void reg_wregister(unsigned char r_addr,unsigned char r_data)//将r_data数据写入地址=r_addr的6936寄存器
{
    SpiWriteByte(r_addr,r_data);
   
}



/*************************************************
  Function:   unsigned char reg_rdstatus(unsigned char r_addr)
  Description:  用于读zigbee芯片寄存器
  Calls:          
  Called By:      
  Table Accessed: 
  Table Updated:  
  Input:     1.unsigned char r_addr,需要读取的寄存器地址            
  Output:         
  Return:    1.寄存器内容   
  Others:        1.2007,6,5 创建
  author:   张建福
  			
*************************************************/
unsigned char reg_rdstatus(unsigned char r_addr)
{    
    return  SpiReadByte(r_addr);
 	
}

/*************************************************
  Function:   void RadioSetSopPnCode(void)
  Description:  用于设置zigbee短包包头
  Calls:          
  Called By:      
  Table Accessed: 
  Table Updated:  
  Input:     1.UINT8 patternNu,选择zigbee包头编号            
  Output:         
  Return:       
  Others:        1.2007,6,5 创建
  author:   张建福
  		
*************************************************/
void RadioSetSopPnCode(PcontrolerConfig TagCng)
{
    U8 i;
    for( i=0; i<8; i++ )
    {
        reg_wregister( SOP_CODE_ADR, TagCng->TagNode.remot_PN_adress[i]);
    }
}

/*************************************************
  Function:     UINT8 RadioSetPA(UINT8 pa)
  Description:  用于设置zigbee发射功率
  Calls:          
  Called By:      
  Table Accessed: 
  Table Updated:  
  Input:     1.UINT8 pa,设置zigbee功率值            
  Output:         
  Return:       
  Others:        1.2007,6,5 创建
  author:   张建福
  			
*************************************************/

/*UINT8 RadioSetPA(UINT8 pa)
{
    UINT8 reg;

    reg = reg_rdstatus( TX_CFG_ADR );

    reg_wregister( TX_CFG_ADR, (reg & ~PA_VAL_MSK) | (pa & PA_VAL_MSK) );

    return reg & PA_VAL_MSK;
}*/

/*************************************************
  Function:     void RadioSetChannel(UINT8 channel)
  Description:  用于设置zigbee通道
  Calls:          
  Called By:      
  Table Accessed: 
  Table Updated:  
  Input:     1.UINT8 channel,设置zigbee通道号            
  Output:         
  Return:       
  Others:        1.2007,6,5 创建
  author:   张建福
  			
*************************************************/

void RadioSetChannel(U8 channel)
{
        reg_wregister( CHANNEL_ADR, channel ); 
}


/*************************************************
  Function:     void radio_data_pn_code(void)
  Description:  用于设置zigbee扰码
  Calls:          
  Called By:      
  Table Accessed: 
  Table Updated:  
  Input:     1.UINT8 pn_code,选择扰码编号  
  Output:         
  Return:       
  Others:        1.2007,6,5 创建
  author:   张建福
  			
*************************************************/


 void radio_data_pn_code(PcontrolerConfig  TagCng)	//设置6936的data_pn_code
{
    U8 i;

    for( i=0; i<16; i++ )
    {
        reg_wregister( DATA_CODE_ADR, TagCng->TagNode.remot_DATA_pn_addr[i]);
    }
}



/*************************************************
  Function:     void RadioSetpreamPnCodee(void)
  Description:  用于设置zigbee 前导码扰码
  Calls:          
  Called By:      
  Table Accessed: 
  Table Updated:  
  Input:     1.UINT8 pn_code,选择扰码编号            
  Output:         
  Return:       
  Others:        1.2007,6,18 创建
  author:   张建福
  			
*************************************************/
void RadioSetpreamPnCode(PcontrolerConfig  TagCng,unsigned char removal)
{
    U8 i;
    for( i=0; i<3; i++ )
    {
	    if(removal !=0)
            reg_wregister( PREAMBLE_ADR,  TagCng->TagNode.remot_pream_addr[i] );
		else
            reg_wregister( PREAMBLE_ADR,0);
    }
}




void RadioSetMfgChannel(unsigned int channel)                                 
{
     reg_wregister(CHANNEL_ADR, (channel + LS_CHANNEL_OFFSET));
}//channel+2



void SelectChanel(PcontrolerConfig TagCng)
{
    
    switch(TagCng->TagNode.chanel_temp)
	{
         case 0x01:
             RadioSetMfgChannel(TagCng->TagNode.remot_1channel);//设置1通道
             TagCng->TagNode.remot_PN_code= TagCng->TagNode.remot_PN_Constcode+1;
             TagCng->TagNode.remot_pream_code=TagCng->TagNode.remot_pream_Constcode+1;
             break;
         case 0x02:
            RadioSetMfgChannel(TagCng->TagNode.remot_2channel);//设置2通道
            TagCng->TagNode.remot_PN_code= TagCng->TagNode.remot_PN_Constcode+2;
            TagCng->TagNode.remot_pream_code=TagCng->TagNode.remot_pream_Constcode+2;
             break;
         case 0x03:
            RadioSetMfgChannel(TagCng->TagNode.remot_3channel);//设置1通道
            TagCng->TagNode.remot_PN_code= TagCng->TagNode.remot_PN_Constcode+3;
            TagCng->TagNode.remot_pream_code=TagCng->TagNode.remot_pream_Constcode+3;
             break;
         case 0x04:
            RadioSetMfgChannel(TagCng->TagNode.remot_4channel);//设置2通道
            TagCng->TagNode.remot_PN_code= TagCng->TagNode.remot_PN_Constcode+4;
            TagCng->TagNode.remot_pream_code=TagCng->TagNode.remot_pream_Constcode+4;
             break;
         case 0x05:
            RadioSetMfgChannel(TagCng->TagNode.remot_5channel);//设置1通道
            TagCng->TagNode.remot_PN_code= TagCng->TagNode.remot_PN_Constcode+5;
            TagCng->TagNode.remot_pream_code=TagCng->TagNode.remot_pream_Constcode+5;
             break;
         case 0x06:
            RadioSetMfgChannel(TagCng->TagNode.remot_6channel);//设置2通道
            TagCng->TagNode.remot_PN_code= TagCng->TagNode.remot_PN_Constcode+6;
            TagCng->TagNode.remot_pream_code=TagCng->TagNode.remot_pream_Constcode+6;
            break;
          case 0x07:
            RadioSetMfgChannel(TagCng->TagNode.remot_7channel);//设置2通道
            TagCng->TagNode.remot_PN_code= TagCng->TagNode.remot_PN_Constcode+7;
            TagCng->TagNode.remot_pream_code=TagCng->TagNode.remot_pream_Constcode+7;
             break;
         default:
             RadioSetMfgChannel(TagCng->TagNode.remot_0channel);//设置0通道
             TagCng->TagNode.remot_PN_code= TagCng->TagNode.remot_PN_Constcode;
             TagCng->TagNode.remot_pream_code=TagCng->TagNode.remot_pream_Constcode;
             break;
	}
    radio_inimap(TagCng);
    radio_data_pn_code(TagCng); //设置数据pn码
    RadioSetSopPnCode(TagCng);//  设置sop pn码
    RadioSetpreamPnCode(TagCng,1);//设置前导码*/
    minidelay(10);//2uS*
 
}
void SetPowerAndBitrate(PcontrolerConfig TagCng)
{
    nop();
    nop();
  	switch(((TagCng->TagNode.RF0bard&0x0f0)>>4))
	{
	   case 0x00:
	 
            TagCng->TagNode.FRAMe_Cfg_ADR=0;
            reg_wregister( TX_CFG_ADR,DATMODE_RAW|TagCng->TagNode.remot_power);
            reg_wregister( FRAMING_CFG_ADR, TagCng->TagNode.FRAMe_Cfg_ADR);
            reg_wregister( TX_OFFSET_LSB_ADR, 0x55 );
            reg_wregister( TX_OFFSET_MSB_ADR, 0x05 );
          
            reg_wregister(CRC_SEED_MSB_ADR,0x1a);//crc种子
            reg_wregister(CRC_SEED_LSB_ADR,0xbb);
            return;
	   case 0x01:
	   
            reg_wregister( TX_CFG_ADR, (DATCODE_LEN_32 | DATMODE_8DR|TagCng->TagNode.remot_power));
            reg_wregister(DATA32_THOLD_ADR,0x05);
           break;
	    case 0x02:
        
             reg_wregister( TX_CFG_ADR, (DATCODE_LEN_64 | DATMODE_8DR|TagCng->TagNode.remot_power));
             reg_wregister(DATA64_THOLD_ADR,0x0e);
           break;
	   case 0x03:
	
             reg_wregister( TX_CFG_ADR, DATCODE_LEN_32 | DATMODE_DDR|TagCng->TagNode.remot_power);
             reg_wregister(DATA32_THOLD_ADR,0x05);
           break;
	    case 0x04:
	
             reg_wregister( TX_CFG_ADR, DATCODE_LEN_64 | DATMODE_DDR|TagCng->TagNode.remot_power);
             reg_wregister(DATA64_THOLD_ADR,0x0e);
           break;
	    case 0x05:
	
            reg_wregister( TX_CFG_ADR, (DATCODE_LEN_64 | DATMODE_SDR|TagCng->TagNode.remot_power));
            reg_wregister(DATA64_THOLD_ADR,0x0e);
           break;
	   default:
            break;
	 }

}

/************************************************************************************************
  Function: void radio_ini(PcontrolerConfig TagCng,unsigned char removal)
  Description: 初始化mcu
  note：
  Calls:          
  Called By: OutFromSleep(Pcontroler_Symple TagCng)  main(void)      
  Table Accessed: 
  Table Updated:  
  Input:               
  Output:         
  Return:       
  Timers:  2014.01.02    
  author:  ME 
  			
************************************************************************************************/



void radio_ini(PcontrolerConfig TagCng,unsigned char removal)
{  
    unsigned char i;
    DISABLE_RF_IRQ_INT();//#define DISABLE_RF_IRQ_INT() PD_CR2_C24 = 0

    switch(TagCng->TagNode.RFiniFlag)
	{
        case 0x01:
            outOfSleep();
            SelectChanel(TagCng);
            break;

        default:
                                        
         //   
         //   minidelay(5);
            //reg_wregister( MODE_OVERRIDE_ADR, RST );//device rst
           SETB_set;
            ////SETB_rst()=1;	//SETB_rst() (PD_ODR_ODR5)
            //delay(1L);
            minidelay(10);
            ////SETB_rst()=0;
            SETB_clr;
            //delay(30L);
            minidelay(20);
         //   IO_PA_EN()=1;
            minidelay(50); 
            reg_wregister( MODE_OVERRIDE_ADR, RF_RST );//device rst
            //delay(30L);
            minidelay(100);
            SelectChanel(TagCng);
         //   reg_wregister( XACT_CFG_ADR,FRC_END_STATE|END_STATE_RX|ACK_TO_4X);
            reg_wregister( XACT_CFG_ADR,FRC_END_STATE|END_STATE_IDLE|ACK_TO_4X);
            
            reg_wregister(PWR_CTRL_ADR,0x80);
            SetPowerAndBitrate(TagCng);
            switch(removal)
            {
                case 0x00:
                    TagCng->TagNode.FRAMe_Cfg_ADR = SOP_EN |LEN_EN | 0x04;//
                    reg_wregister(TX_OVERRIDE_ADR, 0x00);
                    reg_wregister( RX_OVERRIDE_ADR,0x08);
                  
                    break;
                case 0x01:
                    TagCng->TagNode.FRAMe_Cfg_ADR  = SOP_EN | SOP_LEN | LEN_EN | 0x0e;
                    reg_wregister(TX_OVERRIDE_ADR, 0);
                    reg_wregister(RX_OVERRIDE_ADR,0x08);
                      
                    break;
                default:
                    break;
           
            }
            reg_wregister( RX_CFG_ADR, (TagCng->TagNode.reci_sensity| HILO_EN|RXOW_EN) );
            reg_wregister( FRAMING_CFG_ADR, TagCng->TagNode.FRAMe_Cfg_ADR);
            reg_wregister( TX_OFFSET_LSB_ADR, 0x55 );
            reg_wregister( TX_OFFSET_MSB_ADR, 0x05 );
           
            reg_wregister(CRC_SEED_MSB_ADR,0x1a);//crc种子
            reg_wregister(CRC_SEED_LSB_ADR,0xbb);
            
            RFCloseFlag=1;//开rf关闭标志
            break;
	}


 }


void BackRfChannel(PcontrolerConfig TagCng)
{
    //降低发送功率，限制范围
    TagCng->TagNode.remot_powerBack=TagCng->TagNode.remot_power;
    TagCng->TagNode.remot_power=2;//临时更改，具体功率需要实际测试
    
    //改变F1通道通讯频率为默认频率
    TagCng->TagNode.channel1_rateBack=TagCng->TagNode.remot_1channel;
    TagCng->TagNode.remot_1channel=channel1_rate;
    
    //降低接收灵敏度，限制范围
    //agCng->TagNode.reci_sensityBack=TagCng->TagNode.reci_sensity;
    //TagCng->TagNode.reci_sensity=0x20;
}
void CoveryChannel(PcontrolerConfig TagCng)
{
    //恢复发送功率
    TagCng->TagNode.remot_power =TagCng->TagNode.remot_powerBack;
    
    //恢复F1通道通讯频率
    TagCng->TagNode.remot_1channel=TagCng->TagNode.channel1_rateBack;
    
    //恢复接收灵敏度
    //TagCng->TagNode.reci_sensity=TagCng->TagNode.reci_sensityBack;
}
/*************************************************
  Function:    void radio_inimap(PcontrolerConfig TagCng)
  Description:  用于Cy6936初始化
  Calls:          
  Called By:      
  Table Accessed: 
  Table Updated:  
  Input:                 
  Output:         
  Return:       
  Others:        1.2007,6,5 创建
  author:   张建福
  			
*************************************************/

void radio_inimap(PcontrolerConfig TagCng)
{

    unsigned char temp;
 
 

    for(temp = 0; temp < 8; temp++)
        TagCng->TagNode.remot_PN_adress[temp]= SOP_PNCODE_table[(TagCng->TagNode.remot_PN_code%12)][temp];

    for(temp = 0; temp < 16; temp++)
        TagCng->TagNode.remot_DATA_pn_addr[temp]= Data_PNCODE_table[(TagCng->TagNode.remot_PN_code%4)][temp];

    for(temp = 0; temp < 3; temp++)
        TagCng->TagNode.remot_pream_addr[temp]= PREAM_pn_code_table[TagCng->TagNode.remot_pream_code][temp] ;



}


/*************************************************
  Function:   void exint0(void)
  Description:    这是外中断服务0程序,用于ＣＹＲＦ6936的中断请求
　　　　　　　　　用于数据接收．　　　　　　　　　

  Calls:          
  Called By:      
  Table Accessed: 
  Table Updated:  
  Input:   
              
                 
  Output:    ＊reg_rdata:全局变量,接收ＢＵＦＦＥＲ
  　　　　　　　rf_reci_length:全局变量,接收长度
  　　　　　　 rf_reci_finish:全局变量,接收结束标记
  　　　　　　
  Return:       
  Others:        1.2007,6,5 创建
  author:   　　张建福
  //  unsigned char recifinish;
	//		  unsigned char IssStrenth;
	//		  unsigned char reciLength;			
*************************************************/

unsigned char exint0(Pcontroler_Symple TagCng)	//6936 IRQ中断服务程序，读取接收到的数据
{
    unsigned int i=0,usercode=0;
    unsigned char j,*aData;
    unsigned int nAccum;
    unsigned char status;
    unsigned int crcdata1=0,crcdata2=0;
    Pframe_ctrlArea  RfreciPage;
    RfreciPage = (Pframe_ctrlArea)&TagCng->rfReci[0];
   
    status = reg_rdstatus( RX_IRQ_STATUS_ADR );
    if((status&0x85)!=0)		//有错误产生，Receive Overwrite,Receive Buffer Error,Receive Error产生
    {
         radio_ini((PcontrolerConfig)TagCng,1);
         RXDrf();
         return FALSE;
    }	
    if((status&0x08) ==0)	//收到数据为空
	{
        return FALSE;
	}
    TagCng->reciLength = reg_rdstatus( RX_LENGTH_ADR );
    
    if((TagCng->reciLength>(RfReciBufferSize-1))||(TagCng->reciLength==0x00))
	{
        radio_ini((PcontrolerConfig)TagCng,1);
        RXDrf();
        return FALSE;
	 }
          
    j=0;
    TagCng->IssStrenth=reg_rdstatus( RSSI_ADR )&0x1f;
    aData = (unsigned char *)&TagCng->rfReci[0];
    nAccum=0;
	         
    do
     {
            
        *aData++ =reg_rdstatus( RX_BUFFER_ADR );
        j++;
        if((reg_rdstatus( RX_IRQ_STATUS_ADR )&0x08) ==0)	//Receive Buffer 不为空
        {
             for(i =0;i<TagCng->Config.TagPara.tempComp;i++)
			{
				  if((reg_rdstatus( RX_IRQ_STATUS_ADR )&0x08) !=0)	//Receive Buffer 不为空
				  {
					   i=TagCng->Config.TagPara.tempComp;
				  }
          	}
         } 
	  }while(j<TagCng->reciLength);
     PA_EN_clr;
     j = reg_rdstatus( RX_STATUS_ADR );//936芯片内部CRC校验
     nop();
     usercode=TagCng->rfReci[0]+(TagCng->rfReci[1]<<8);//低字节在前
   // usercode=(TagCng->rfReci[0]<<8)+TagCng->rfReci[1];//高字节在前
     crcdata1=CRC_16((unsigned char *)RfreciPage,TagCng->reciLength-2);
     crcdata2=TagCng->rfReci[TagCng->reciLength-1]+(TagCng->rfReci[TagCng->reciLength-2]<<8);
               

       
     if(  ((j&0x08) !=0)                                //若6936芯片内部CRC校验错误或
//		||((RfreciPage->Frame_ctrl&0x38)!=0x00)	         //帧类型不是电子标签控制帧
        ||(usercode!=TagCng->Config.TagNode.UserCode)	//接收到的用户码跟标签的用户码不匹配
        || (crcdata1!=crcdata2)   	   )                 //数据包内部crc校验数据不匹配
	 {
          radio_ini((PcontrolerConfig)TagCng,1);
		  RXDrf();
          return FALSE;
	 }


     j =(RfreciPage->text[6]&0x038);		//提取帧属性
	 
	 if( ( (RfreciPage->text[0] ==TagCng_symple.Config.TagNode.Tag_id_nub[0])	//收到的帧目的地址为本标签
	      &&(RfreciPage->text[1] == TagCng_symple.Config.TagNode.Tag_id_nub[1])
	      &&(RfreciPage->text[2] == TagCng_symple.Config.TagNode.Tag_id_nub[2])
		  )&&(RfreciPage->text[7]!=0x16) )
      {
              	     
		   switch(j)
		   {
			    case (CoodSubFame):
         
//                     nop();
//                     nop();
//                     TagCng->recifinish =0x01;
//                     nop();
//                     nop();
//                    return TRUE;
		        case (R_WSubFame):
                case (ReadSubFame):
                     nop();
                     nop();
                     TagCng->recifinish =0x01;
                    return TRUE;
          	    default:
                     #if TestDistance ==0
                         RXDrf();
                         return FALSE;
                     #else
                        RXDrf();
                        return TRUE;
                    #endif
	  		   
			 }
         nop();
         nop();
         return FALSE;
	  }

      else if((       (RfreciPage->text[0] ==0)		//收到的帧为广播帧
	                 &&(RfreciPage->text[1] == 0)
	                 &&(RfreciPage->text[2] == 0))
	           	     &&(RfreciPage->text[7]==0x16)	)
	 {                                                                            
           switch(j)
		   {
			    case (CoodSubFame):
			    case (R_WSubFame):
                case (ReadSubFame):
                    nop();
                    nop();
			        TagCng->recifinish =0x01;
    		        return TRUE;
                case (Tag_SubFame):
                    nop();
                    nop();
                    RXDrf();
                    nop();
                   return TRUE;
			   default:
                  nop();
                  nop();
                  RXDrf();
	  		      return FALSE;
	    	}
    }         
	else
	{
		 nop();
		 RXDrf();
		 return FALSE;
    }

   nop();
   nop();	
}





unsigned char ACK_deal(Pcontroler_Symple TagCng)
{

   unsigned long  Rycycle;
   unsigned char  j,status,*aData,temp;
   unsigned int   i,nAccum;
   unsigned char datatemp[10];
   reg_wregister( XACT_CFG_ADR,FRC_END_STATE|END_STATE_RX|ACK_TO_4X);
   TagCng->Config.TagNode.chanel_temp=5;
   SelectChanel((PcontrolerConfig)TagCng);
   //reg_wregister( RX_CFG_ADR, (TagCng->Config.TagNode.reci_sensity| HILO_EN |RXOW_EN) );
   RXDrf();
   for(Rycycle=0;Rycycle<TagCng->Config.TagPara.GetAckDls;Rycycle++)
    {
       
	   if((reg_rdstatus( RX_IRQ_STATUS_ADR )&0x08)!=0)	//Receive Buffer Not Empty时
	     {
	       if(  reg_rdstatus(RX_LENGTH_ADR)!=0x08)
           return FALSE;
    	   aData =&datatemp[0];
		   nAccum =0;
		   j=0;
          
           do{		//RF接收数据
                *aData =reg_rdstatus( RX_BUFFER_ADR );
            
			    j++;
				
                for(i =0;i<TagCng->Config.TagPara.tempComp;i++)
			     {
			 
			       if((reg_rdstatus( RX_IRQ_STATUS_ADR )&0x08) ==0) //Receive Buffer 为空时
			        {
				      if(j>7)
				       {i=TagCng->Config.TagPara.tempComp;}
				    }
				  else
				   {
				     i=TagCng->Config.TagPara.tempComp;
				   }
			    }
	    
			 }while(j<8);
             
        nop();
        nop();
        if((nAccum !=0)||((datatemp[2]&0x0f8)!=(RespOfFrame|Tag_SubFame)))	//判定帧类型
	 
		  {
               return FALSE;	
		  }	
       
        if(   (datatemp[3] == TagCng->Config.TagNode.Tag_id_nub[0])
	        &&(datatemp[4] == TagCng->Config.TagNode.Tag_id_nub[1])
	        &&(datatemp[5] == TagCng->Config.TagNode.Tag_id_nub[2])
	        )
		   {
     	     TagCng->Config.TagNode.RFReciTDelay = datatemp[2]&0x07;
             return TRUE;		
		   }
	   else
		 {
           return FALSE;			
		 } 
	  }	 
	 	
	}
   return FALSE;

}



unsigned char CheckRssiValue(Pcontroler_Symple TagCng,unsigned int Value )
{
    if(Value>(TagCng->Config.TagPara.Rssi_bottom+3))
        return FALSE;
    else
        return TRUE;
}



unsigned char collisoHnead_deal(Pcontroler_Symple TagCng)
{
    unsigned char pointer=0,temp[10],i;
	unsigned int  count,total=50;

	for(count =0;count<300;count++)
	{
        nop();
		temp[pointer++] = reg_rdstatus( RSSI_ADR )&0x1f;
        pointer=pointer%6;
	    if(count>12)
		{
            total=temp[0]+temp[1]+temp[2]+temp[3]+temp[4]+temp[5];
            total = total/6;//将采集到的信号强度相加取平均
            count=300;
            
            if(CheckRssiValue(TagCng,total)==TRUE)
                return TRUE;
            if(count>150)
            {
                return FALSE;
            }
         }
        
		minidelay(10);
        
    }
  
    return FALSE;
}

unsigned char Getchannel_timer(Pcontroler_Symple TagCng)
{
    //取得系统配置时间
    
    switch(((TagCng->Config.TagNode.RF0bard&0x0f0)>>4))
    {
        case 0x00:
        case 0x01:
        case 0x02:
        case 0x04:
            TagCng->Config.TagPara.WaitRspTimerConst=60;
            TagCng->Config.TagPara.tempComp =240;
            TagCng->Config.TagPara.GetAckDls =320;
            TagCng->Config.TagPara.tempOver =1200;
          
            break;
        case 0x03:
            TagCng->Config.TagPara.WaitRspTimerConst=30;
            TagCng->Config.TagPara.tempComp =160;
            TagCng->Config.TagPara.GetAckDls =160;
            TagCng->Config.TagPara.tempOver =600;
            break;
        case 0x05:
            TagCng->Config.TagPara.WaitRspTimerConst=120;
            TagCng->Config.TagPara.tempComp =400;
            TagCng->Config.TagPara.GetAckDls =640;
            TagCng->Config.TagPara.tempOver =3200;
            break;
        default:
            return FALSE;
  
    }
	return TRUE;

}


unsigned char RadioSendPacket(unsigned char retry, unsigned char length ,unsigned char packageType)
{
    Pcontroler_Symple TagCng;
    unsigned char status;
    unsigned char send_count,*s;
    unsigned char tx_count;
	unsigned long Rycycle;
	unsigned long i;

    TagCng = &TagCng_symple;//Error[Pe513]: a value of type "Controler_Symple const volatile *" cannot be assigned to an entity of type "Pcontroler_Symple" E:\work\software\geomagnetic\PSCReader\PIC\zigbee_m.c 982 

	tx_count=0;
    do
    {
        if(packageType!=2)
        {
            if(collisoHnead_deal(TagCng) ==FALSE)
            {
                nop();
                CrashCount++;
                CrashCount=CrashCount;
                if (SendNodeCount<(SendPackCount+3))
                {
                    PA_EN_clr;//关闭发送功放电源，用于降低功耗
                    return FALSE;
                }
                
            }
        }
        flashLeds(1,TagCng_symple.Config.TagNode.LEDFlag);

        s = (unsigned char *)&TagCng->txReci[0];
         // reg_wregister( XACT_CFG_ADR,FRC_END_STATE|END_STATE_RX|ACK_TO_4X);	//强制进入RXMODE,ACK Timeout=4X32US(SOP LEN=0),ACK Timeout=4X64US(SOP LEN=1)
        reg_wregister( XACT_CFG_ADR,FRC_END_STATE|END_STATE_IDLE|ACK_TO_4X);//进入空闲模式
        reg_wregister( TX_LENGTH_ADR,length);//写入发送数据长度
        reg_wregister( TX_CTRL_ADR, TX_CLR );//清除发送缓冲区
          
        reg_wregister( TX_BUFFER_ADR, s[0]);//写入数据，第1字节
        reg_wregister( TX_BUFFER_ADR, s[1]);//写入数据，第2字节
        reg_wregister( TX_BUFFER_ADR, s[2]);//写入数据，第3字节
        reg_wregister( TX_BUFFER_ADR, s[3]);//写入数据，第4字节
 //         reg_wregister( TX_BUFFER_ADR, s[4]);//写入数据，第5字节
 //         reg_wregister( TX_BUFFER_ADR, s[5]);
 //         reg_wregister( TX_BUFFER_ADR, s[6]);
 //         reg_wregister( TX_BUFFER_ADR, s[7]);
 //         reg_wregister( TX_BUFFER_ADR, s[8]);
 //         reg_wregister( TX_BUFFER_ADR, s[9]);
//          reg_wregister( TX_BUFFER_ADR, s[10]);//写入数据，第10字节
        reg_wregister( TX_CTRL_ADR, TX_GO );//启动发送
        //如果发送过快至缓冲区溢出，或为空后超过时间未填入数据，会导致错误发生
        //为防止错误发生，先写入部分数据到缓冲区后再启动发送
        //发送过程中循环检查发送缓冲区是否为满，不满则继续填入数据，
        //否则延时等待数据取为未满,然后填入数据
        //延时时间系数由波特率决定
        for(send_count=4;send_count<length;send_count++)
        {
                      
            reg_wregister( TX_BUFFER_ADR, s[send_count]);//写入数据
            for(i=0;i<TagCng->Config.TagPara.tempComp;i++)
            { 
                status=reg_rdstatus(TX_IRQ_STATUS_ADR);
                if((status&0x20)!=0)
                {
                    i =TagCng->Config.TagPara.tempComp+1;
                }
            }
           
         }
           
        nop();
        nop();
        if(packageType==0x02)
        {
            TagCng->Config.TagPara.tempOver=1200;
        }
	    for(Rycycle=0;Rycycle<TagCng->Config.TagPara.tempOver;Rycycle++)
	    {
            status  =  reg_rdstatus(TX_IRQ_STATUS_ADR);
            nop();
            nop();

            if((status &TXC_IRQ)!=0)	//判断传输完成中断状态
		    {	
				nop();
                PA_EN_clr;//关闭发送功放电源，用于降低功耗

                LED1_clr;
			    switch(packageType) 
				{
                    case 0x00:
                         TagCng->Config.TagPara.CollisonInterval =0;
                         TagCng->Config.TagNode.RFReciTDelay =6;
                         //delay(295);
                         return TRUE;
                    case 0x01:
                
              //            status =send_end_deal(TagCng);
//                          TagCng->Config.TagNode.chanel_temp=1;
//                          radio_ini((PcontrolerConfig)TagCng,1);
//                          RXDrf();
                          return  status;
                    case 0x02:
                        TagCng->Config.TagPara.CollisonInterval =0;
                        TagCng->Config.TagNode.RFReciTDelay =0;
                        nop();
                        nop();
                        return TRUE;
                    default:
                        return FALSE;
				}
		    }
			if((status & 0x05) != 0)		//如果出现buffer错误或则发送错误，则退出循环
		 	{
				Rycycle = TagCng->Config.TagPara.tempOver;

			}
            minidelay(4);//2uS*2
        }
        nop();
        nop();
        tx_count++  ;
    }
    while( tx_count<retry );
    nop();
    nop();
    PA_EN_clr;//关闭发送功放电源，用于降低功耗
    LED1_clr;
    return FALSE;
}



/*************************************************
  Function:   void RFinoSleep(void)
  Description:    设置ＲＦ为睡眠模式
   Calls:          
  Called By:      
  Table Accessed: 
  Table Updated:  
  Input:  
                 
  Output:    
  Return:       
  Others:        1.200９,５,１２ 创建
  author:   张建福
  			
*************************************************/
/*void RfSleep()
{
    DISABLE_RF_IRQ_INT();
    IO_RX_SW() = 0;
    SpiWriteByte( XACT_CFG_ADR,FRC_END_STATE|END_STATE_SLEEP|ACK_TO_4X);

}*/

void RFinoSleep(void)
{
   // DISABLE_RF_IRQ_INT();
   // IO_RX_SW() = 0;
   // SpiWriteByte( XACT_CFG_ADR,FRC_END_STATE|END_STATE_SLEEP|ACK_TO_4X);
    reg_wregister(MODE_OVERRIDE_ADR,0x00);
    reg_wregister( PWR_CTRL_ADR,0x00);
    reg_wregister( XACT_CFG_ADR,FRC_END_STATE|END_STATE_SLEEP|ACK_TO_4X);

    PA_EN_clr;

}
void RXDrf(void)	//若未执行接收动作则启动接收
{
     PA_EN_set;
     reg_wregister( XACT_CFG_ADR,FRC_END_STATE|END_STATE_IDLE|ACK_TO_4X);
     //reg_wregister( XACT_CFG_ADR,FRC_END_STATE|END_STATE_RX|ACK_TO_4X);
     reg_wregister( RX_CTRL_ADR, RX_GO);
     minidelay(2);//2uS

}
void RXDIdle(void)	//若未执行接收动作则启动接收
{ 
     PA_EN_set;
     reg_wregister( XACT_CFG_ADR,FRC_END_STATE|END_STATE_IDLE|ACK_TO_4X);
     reg_wregister( RX_CTRL_ADR, RX_GO);
     minidelay(2);//2uS

}
/*************************************************
  Function:   void RFinoSleep(void)
  Description:    设置ＲＦ为空闲模式
   Calls:          
  Called By:      
  Table Accessed: 
  Table Updated:  
  Input:  
                 
  Output:    
  Return:       
  Others:        1.200９,５,１２ 创建
  author:   张建福
 *************************************************/ 
 			
/*void RFinoIdle(void)
{
reg_wregister( XACT_CFG_ADR,FRC_END_STATE|END_STATE_IDLE|ACK_TO_4X);
}*/

void check_crystal_stableback(void)
{
  unsigned int i;
  for(i=0;i<1000;i++)
  {
    if((reg_rdstatus( TX_IRQ_STATUS_ADR )&0x80)!=0)
    {
    nop();
    nop();
    i =1000;
    }
  }
   nop();
   nop();
}

void check_crystal_stable(void)
{
  unsigned int i;
  for(i=0;i<6000;i++)
  {
    if((reg_rdstatus( TX_IRQ_STATUS_ADR )&0x80)!=0)
    {
    nop();
    nop();
    i =6000;
    }
  }
   nop();
   nop();
}

void outOfSleep(void)
{
reg_wregister(MODE_OVERRIDE_ADR,0x38);
//reg_wregister(XACT_CFG_ADR,0x80);
//reg_wregister(IO_CFG_ADR,0x02);;
//reg_wregister(MODE_OVERRIDE_ADR,0x20);


}


void test_mouulata(void)
{

    reg_wregister(PREAMBLE_ADR,0x01);
    reg_wregister(PREAMBLE_ADR,0x00);
    reg_wregister(PREAMBLE_ADR,0x00);
    reg_wregister( TX_OVERRIDE_ADR, FRC_PREAMBLE );
    reg_wregister( TX_CTRL_ADR, TX_GO );

}