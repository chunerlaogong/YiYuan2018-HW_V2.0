
#ifndef _BERTH_STATE_JUDGE_H_
#define _BERTH_STATE_JUDGE_H_

#include "common_.h"

#define STATE_REVERSAL 0x5a  // 状态反转  

typedef struct _InputInfo {

	U8 diffOfRM;
	S16 x;
	S16 y;
	S16 z;
	S8 x_b;
	S8 y_b;
	S8 z_b;
	U8 RM_thr;      // 模值阈值
	U8 angle_thr;   // 配置角度阈值
}InputInfo;

/*
* 泊位状态切换 
* ret=1:状态反转
*/
U8 berthStateSwithProcess( U8 *parkState, Pcontroler_Symple TagCng);


#endif

