
#ifndef BOTTOM_ADJUST_H_
#define BOTTOM_ADJUST_H_

#include "struct.h"

/*
* 本底值自适应调整
*/
U8 bottomvalAdjustProcess(U8 parkState);

/*
* 获取实时本底值
*/
SEMData getRealTimeBottom(void);

void GetDynamicBottom(void);


#endif


