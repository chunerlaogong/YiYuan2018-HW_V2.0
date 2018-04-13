
#ifndef _BERTH_STATE_JUDGE_H_
#define _BERTH_STATE_JUDGE_H_

#include "common_.h"

#define STATE_REVERSAL 0x5a  // ״̬��ת  

typedef struct _InputInfo {

	U8 diffOfRM;
	S16 x;
	S16 y;
	S16 z;
	S8 x_b;
	S8 y_b;
	S8 z_b;
	U8 RM_thr;      // ģֵ��ֵ
	U8 angle_thr;   // ���ýǶ���ֵ
}InputInfo;

/*
* ��λ״̬�л� 
* ret=1:״̬��ת
*/
U8 berthStateSwithProcess( U8 *parkState, Pcontroler_Symple TagCng);


#endif

