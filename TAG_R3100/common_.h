
#ifndef _COMMON_H_
#define _COMMON_H_

#include "Type.h"
#include "struct.h"

#define MAX_(bit1, bit2)  ((bit1) >= (bit2) ? (bit1) : (bit2))// 最大值
#define MIN_(bit1, bit2)  ((bit1) <= (bit2) ? (bit1) : (bit2))// 最小值
#define TYPE_S8(data) (data) > 127 ? 127 : ((data) < -127 ? -127 : (data));//当data>127时，data=127；当-127=<data<=127时，data=data；当data<-127时，data=-127
#define TYPE_U8(data) (data) > 255 ? 255 : ((data) < 0 ? 0 : (data));//当data>255时，data=255；当0=<data<=255时，data=data；当data<0时，data=0 


//枚举车位状态
typedef enum {
	BERTH_STATE_NUL,//无车
	BERTH_STATE_HAVE,//有车
}BERTH_STATE;



#endif
