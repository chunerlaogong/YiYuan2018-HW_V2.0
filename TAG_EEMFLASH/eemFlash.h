#ifndef eemFlash_h 
#define eemFlash 1
#include "struct.h"
#include "Common.h"

#define FLASH_START_ADDR 0x1000      //main程序eem起始地址
#define FLASH_Boot_Flag_Add 0x1000   //升级引导标志起始位置

//#define DEFAULT_SERVER_ADDRESS			"118.122.93.190:7788"
#if COAP_TEST
#define DEFAULT_SERVER_ADDRESS                  "180.101.147.115:5683"
#else
#define DEFAULT_SERVER_ADDRESS			"118.122.93.190:30001"
#endif
typedef struct
{
	uint8_t Tag_status;
	uint8_t Tag_initNB;
	uint16_t UserCode;
	uint8_t TagID[3];
	uint8_t UpdateFlag;
	uint8_t Version;
	uint16_t WdtInterval;		//WDT睡眠时间
	uint16_t HeartbeatInterval;	//心跳时间
	uint16_t AlarmValid;
	uint8_t ParaIntFlag[3];		//参数同步标志
	uint8_t LEDFlag;
	uint8_t GetEMBottom_RFFlag;	//地磁开关
	int16_t GetEMBottom_RFModThreshold;		//
	int16_t GetEMBottom_RFAngleThreshold;	//
	int16_t EMData_xBottom;
	int16_t EMData_yBottom;
	int16_t EMData_zBottom;
	uint8_t FastRouse;
	uint8_t FastRouseAlarm;
	uint8_t ServerCfg;
	uint8_t DefaultIPAddr[4];
	uint16_t DefaultPort;
	uint8_t Server1IPAddr[4];
	uint16_t Server1Port;
	uint8_t Server2IPAddr[4];
	uint16_t Server2Port;
}sParamOfNBTag,*pParamOfNBTag;

void ReadFlash2Buff(uint8_t* buff, uint8_t cLen, uint8_t cAddr);
void WriteBuff2Flash(uint8_t* buff, uint8_t cLen, uint8_t cAddr);
void WriteBuffToFlashEepRom(FLASH_MemType_TypeDef FLASH_MemType, uint32_t cAddr, uint8_t *pFromBuff, uint32_t cLen);
void ReadFlashEepRomToBuff(FLASH_MemType_TypeDef FLASH_MemType, uint32_t cAddr, uint8_t *pToBuff, uint32_t cLen);
extern Sontroler_Symple  TagCng_symple;
void testRomOperate(Pcontroler_Symple TagCng);

void ReadParamFrmFlash(Pcontroler_Symple TagCng);
//void writeFlashParameter(PcontrolerConfig TagCng);
void WriteTagParam(Pcontroler_Symple TagCng);
void ReadTagParam(Pcontroler_Symple TagCng);
void TagParamInit(Pcontroler_Symple TagCng);

#endif

