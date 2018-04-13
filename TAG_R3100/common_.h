
#ifndef _COMMON_H_
#define _COMMON_H_

#include "Type.h"
#include "struct.h"

#define MAX_(bit1, bit2)  ((bit1) >= (bit2) ? (bit1) : (bit2))// ���ֵ
#define MIN_(bit1, bit2)  ((bit1) <= (bit2) ? (bit1) : (bit2))// ��Сֵ
#define TYPE_S8(data) (data) > 127 ? 127 : ((data) < -127 ? -127 : (data));//��data>127ʱ��data=127����-127=<data<=127ʱ��data=data����data<-127ʱ��data=-127
#define TYPE_U8(data) (data) > 255 ? 255 : ((data) < 0 ? 0 : (data));//��data>255ʱ��data=255����0=<data<=255ʱ��data=data����data<0ʱ��data=0 


//ö�ٳ�λ״̬
typedef enum {
	BERTH_STATE_NUL,//�޳�
	BERTH_STATE_HAVE,//�г�
}BERTH_STATE;



#endif
