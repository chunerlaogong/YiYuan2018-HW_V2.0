#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common_.h"
#include "statisMagneticDensity.h"

#define MAX_STATIS_SIZE 8     // 最大统计数
#define STATIS_SET_SIZE 4     // 统计集
#define STATIS_SUBSET_SIZE 6  // 统计子集

#define BOTTOM_TIMES_STATIS_SET_SIZE 4     // 有车切无车时数据统计集
#define BOTTOM_TIME_STATIS_SET_SIZE 8      // 无车数据统计集
#define BOTTOM_TIME_STATIS_SUBSET_SIZE 8   // 无车数据统计子集

typedef struct _StatisSet {

    Sample_data data[MAX_STATIS_SIZE];
    U8 var;
	U8 var_x;
	U8 var_y;
	U8 var_z;
    U8 mean;
    S16 mean_x;
    S16 mean_y;
    S16 mean_z;
    U8 size;
    U8 len;
}StatisSet;

StatisSet g_statis_subset;          // 统计子集6
StatisSet g_statis_set;             // 统计集 4
StatisSet g_bottom_times_set;       // 5
StatisSet g_bottom_time_subset;     // 8
StatisSet g_bottom_time_set;        // 8


/*
* 初始化
*/
void initStatisSet(Magnetic_density *magnetic_density) {
  
    memset(&g_statis_subset, 0, sizeof(StatisSet));
    memset(&g_statis_set, 0, sizeof(StatisSet));
    g_statis_subset.size = STATIS_SUBSET_SIZE;
    g_statis_set.size = STATIS_SET_SIZE;
    magnetic_density->state = MAGNETIC_STATE_NUL;
    magnetic_density->level = MAGNETIC_MIDDLE;
    magnetic_density->val = 0;

}

/*
* 清零
*/
void zeroStatisSet(StatisSet *statis_set) {
    statis_set->len = 0;
}

/*
* 入队
*/
void enqueueStatisSet(StatisSet *statis_set, Sample_data data) {
    U8 n = 0;
    if (statis_set->size == 0) {
        return;
    }

    n = MIN_(statis_set->size - 1, statis_set->len);
    statis_set->data[n] = data;
    statis_set->len = MIN_(statis_set->len + 1, statis_set->size);
}

/*
* 队满
*/
U8 queueFullStatisSet(StatisSet *statis_set) {
    return (statis_set->size == statis_set->len);
}

/*
* 队空
*/
U8 queueEmptyStatisSet(StatisSet *statis_set) {
    return (statis_set->len == 0);
}

/*
* 出队
*/
void dequeueStatisSet(StatisSet *statis_set) {
    U8 n = 0;
    for (n = 1; n < statis_set->len; n++) {
        statis_set->data[n - 1] = statis_set->data[n];
    }
    statis_set->len = MAX_(statis_set->len - 1, 0);
}

/*
* 计算统计方差和均值
*/
void calculateStatisVarianceMean(StatisSet *statis_set) {
    U8 n = 0;

    if (statis_set->size <= 0) {
        return;
    }

    // 均值
    statis_set->mean = 0;
    int mean = 0;
    for (n = 0; n < statis_set->size; n++) {
        mean += statis_set->data[n].diffOfRM;
    }
    statis_set->mean = mean / statis_set->size;

    // 方差
	int var = 0;
    for (n = 0; n < statis_set->size; n++) {
        float diff = (statis_set->data[n].diffOfRM - statis_set->mean);
       var += diff * diff;
    }
    statis_set->var = (U8)(var / statis_set->size);

    // 均值
    statis_set->mean_x = statis_set->mean_y = statis_set->mean_z = 0;
    int mean_x = 0, mean_y = 0, mean_z = 0;
    for (n = 0; n < statis_set->size; n++) {
        mean_x += statis_set->data[n].x;
        mean_y += statis_set->data[n].y;
        mean_z += statis_set->data[n].z;
    }
    
    float sign_x = mean_x >= 0 ? 0.5 : -0.5;
    float sign_y = mean_y >= 0 ? 0.5 : -0.5;   
    float sign_z = mean_z >= 0 ? 0.5 : -0.5;
    statis_set->mean_x = (S16)((float)mean_x / statis_set->size + sign_x);
    statis_set->mean_y = (S16)((float)mean_y / statis_set->size + sign_y);
    statis_set->mean_z = (S16)((float)mean_z / statis_set->size + sign_z);

}

/*
* 判断强度级别
*/
MAGNETIC_LEVEL judgeMagnLevel(U8 val) {

    MAGNETIC_LEVEL level;

    if (val > 40) {
        level = MAGNETIC_HIGHT;
    }
    else if (val > 20) {
        level = MAGNETIC_MIDDLE;
    }
    else {
        level = MAGNETIC_LOW;
    }

    return level;
}

/*
* 满集处理
*/
U8 dealFullSet(StatisSet *statis_set) {
    U8 ret = 0;

    // 计算均值方差
    calculateStatisVarianceMean(statis_set);

    if (statis_set->var < MAX_((U8)(statis_set->mean * 0.1), 2)) {
        // 清空
        zeroStatisSet(statis_set);
        ret = 1;
    }
    else {
        // 出队
        dequeueStatisSet(statis_set);
    }

    return ret;
}

/*
* 统计实现
*/
U8 statisProcess(Sample_data data, Magnetic_density *magnetic_density, U8 cmd/*1:持续更新*/) {

	magnetic_density->done = 0;

    if (MAGNETIC_STATE_CONFIRM == magnetic_density->state && 0 == cmd) {
        return 1;
    }

    // 统计集满
    if (1 == queueFullStatisSet(&g_statis_set)) {
        if (1 == dealFullSet(&g_statis_set)) {
          // 判断强度级别
          magnetic_density->level = judgeMagnLevel(g_statis_set.mean);
          magnetic_density->state = MAGNETIC_STATE_CONFIRM;
          magnetic_density->val = g_statis_set.mean;
          magnetic_density->x = g_statis_set.mean_x;
          magnetic_density->y = g_statis_set.mean_y;
          magnetic_density->z = g_statis_set.mean_z;

		  magnetic_density->done = 1;
        }
    }
    else { // 子集满
        if (1 == queueFullStatisSet(&g_statis_subset)) {
            if (1 == dealFullSet(&g_statis_subset)) {
              Sample_data data;
              data.diffOfRM = g_statis_subset.mean;
              data.x = g_statis_subset.mean_x;
              data.y = g_statis_subset.mean_y;
              data.z = g_statis_subset.mean_z;
              
              // 插入集
              enqueueStatisSet(&g_statis_set, data);
              
              if (MAGNETIC_STATE_NUL == magnetic_density->state) {
                // 判断强度级别
                magnetic_density->level = judgeMagnLevel(g_statis_subset.mean);
                magnetic_density->state = MAGNETIC_STATE_INIT;
                magnetic_density->val = g_statis_subset.mean;
                magnetic_density->x = g_statis_subset.mean_x;
				magnetic_density->y = g_statis_subset.mean_y;
				magnetic_density->z = g_statis_subset.mean_z;
              }
              else {}
          } else {}
        } // end if (1 == queueFullStatisSet(&g_statis_subset))
        else {
            // 插入子集
            enqueueStatisSet(&g_statis_subset, data);
        }
    }

    return 1;

}

/*
* 初始化有车且无车数据统计
*/
void initBottomTimesStatisSet(Magnetic_density *magnetic_density) {

	memset(&g_bottom_times_set, 0, sizeof(StatisSet));
	g_bottom_times_set.size = BOTTOM_TIMES_STATIS_SET_SIZE;
	magnetic_density->state = MAGNETIC_STATE_NUL;
	magnetic_density->level = MAGNETIC_MIDDLE;
	magnetic_density->val = 0;
}

/*
* 初始化无车数据统计
*/
void initBottomCnsTimeStatisSet(Magnetic_density *magnetic_density) {

	memset(&g_bottom_time_set, 0, sizeof(StatisSet));
	memset(&g_bottom_time_subset, 0, sizeof(StatisSet));
	g_bottom_time_subset.size = BOTTOM_TIME_STATIS_SUBSET_SIZE;
	g_bottom_time_set.size = BOTTOM_TIME_STATIS_SET_SIZE;
	magnetic_density->state = MAGNETIC_STATE_NUL;
	magnetic_density->level = MAGNETIC_MIDDLE;
	magnetic_density->val = 0;
}

/*
* 计算统计方差和均值
*/
void calculateBottomStatisVarianceMean(StatisSet *statis_set) {
	U8 n = 0;

	if (statis_set->size <= 0) {
		return;
	}

	// 均值
	statis_set->mean_x = statis_set->mean_y = statis_set->mean_z = 0;
	int mean_x = 0, mean_y = 0, mean_z = 0;
	for (n = 0; n < statis_set->size; n++) {
		mean_x += statis_set->data[n].x;
		mean_y += statis_set->data[n].y;
		mean_z += statis_set->data[n].z;
	}
	statis_set->mean_x = mean_x / statis_set->size;
	statis_set->mean_y = mean_y / statis_set->size;
	statis_set->mean_z = mean_z / statis_set->size;

	// 方差
	int var_x = 0, var_y = 0, var_z = 0;
	for (n = 0; n < statis_set->size; n++) {
		short diff = (statis_set->data[n].x - statis_set->mean_x);
		var_x += diff * diff;
		diff = (statis_set->data[n].y - statis_set->mean_y);
		var_y += diff * diff;
		diff = (statis_set->data[n].z - statis_set->mean_z);
		var_z += diff * diff;
	}
	statis_set->var_x = TYPE_U8(var_x / statis_set->size);
	statis_set->var_y = TYPE_U8(var_y / statis_set->size);
	statis_set->var_z = TYPE_U8(var_z / statis_set->size);
}

/*
* 剔除数据
*/
void deleteDataStatisSet(StatisSet *statis_set) {
	U8 n = 0, k = 0;
	U8 maxDevIndex = 0;
	short dev = 0, maxDev = 0; // 偏差
	for (n = 0; n < statis_set->len; n++) {
		dev = abs(statis_set->data[n].x - statis_set->mean_x);
		dev += abs(statis_set->data[n].y - statis_set->mean_y);
		dev += abs(statis_set->data[n].z - statis_set->mean_z);
		if (dev > maxDev)
		{
			maxDev = dev;
			maxDevIndex = n;
		}
	}

	// 防止一直去除最新数据
	if (maxDevIndex == statis_set->len - 1)
		maxDevIndex = 0;

	for (n = 0, k = 0; n < statis_set->len; n++) {
		if (n == maxDevIndex)
			continue;
		statis_set->data[k++] = statis_set->data[n];
	}

	statis_set->len = MAX_(statis_set->len - 1, 0);
}

/*
* 满集处理
*/
U8 dealFullBottomSet(StatisSet *statis_set) {
	U8 ret = 0;

	// 计算均值方差
	calculateBottomStatisVarianceMean(statis_set);

	if (statis_set->var_x <= 4 && statis_set->var_y <= 4 && statis_set->var_z <= 4) {
		// 清空
		zeroStatisSet(statis_set);
		ret = 1;
	}
	else {
		// 剔除数据
		deleteDataStatisSet(statis_set);
	}

	return ret;
}

/*
* 有车且无车时统计实现
*/
U8 bottomTimesAdjustStatisProcess(Sample_data data, Magnetic_density *magnetic_density) {

  	// 插入子集
    enqueueStatisSet(&g_bottom_times_set, data);
    
	if (1 == queueFullStatisSet(&g_bottom_times_set)) {
		if (1 == dealFullBottomSet(&g_bottom_times_set)) {
			magnetic_density->state = MAGNETIC_STATE_CONFIRM;
			magnetic_density->val = g_bottom_times_set.mean;
			magnetic_density->x = g_bottom_times_set.mean_x;
			magnetic_density->y = g_bottom_times_set.mean_y;
			magnetic_density->z = g_bottom_times_set.mean_z;
			return 1;
		}
		else {}
	} // end if (1 == queueFullStatisSet(&g_statis_subset))

	return 0;
}

/*
* 无车统计实现
*/
U8 bottomCnsTimeAdjustStatisProcess(Sample_data data, Magnetic_density *magnetic_density) {

    // 插入到子集
    enqueueStatisSet(&g_bottom_time_subset, data);
    
    // 子集满
    if (1 == queueFullStatisSet(&g_bottom_time_subset)) {
        if (1 == dealFullBottomSet(&g_bottom_time_subset)) {
            Sample_data data;
            data.diffOfRM = g_bottom_time_subset.mean;
            data.x = g_bottom_time_subset.mean_x;
            data.y = g_bottom_time_subset.mean_y;
            data.z = g_bottom_time_subset.mean_z;

            // 插入到集
            enqueueStatisSet(&g_bottom_time_set, data);
        }
        else {}
    } // end if (1 == queueFullStatisSet(&g_statis_subset))
  
	// 统计集满
	if (1 == queueFullStatisSet(&g_bottom_time_set)) {
		if (1 == dealFullBottomSet(&g_bottom_time_set)) {
			magnetic_density->state = MAGNETIC_STATE_CONFIRM;
			magnetic_density->val = g_bottom_time_set.mean;
			magnetic_density->x = g_bottom_time_set.mean_x;
			magnetic_density->y = g_bottom_time_set.mean_y;
			magnetic_density->z = g_bottom_time_set.mean_z;

			return 1;
		}
	}

	return 0;

}

/*
* 判断长时间无车统计和某一次无车数据是否一致
*/
U8 matchTimesAndCnsTimeStatis(Magnetic_density *magnetic_density) {
	U8 n = 0;

	for (n = 0; n < g_bottom_times_set.size; n++) {
		if (abs(magnetic_density->x - g_bottom_times_set.data[n].x) <= 3 &&
			abs(magnetic_density->y - g_bottom_times_set.data[n].y) <= 3 &&
            abs(magnetic_density->z - g_bottom_times_set.data[n].z) <= 3) {
			return 1;
		}
	}

	return 0;
}

/*
* 判断本底偏移值(未判断或未偏移都返回0)
*/
U8 getBottomOffsetValue() {

  	U8 n = 0;
    U8 offset_cnt = 0;
    U16 offset_sum = 0;

	for (n = 0; n < g_bottom_times_set.size; n++) {
		if (0 == g_bottom_times_set.data[n].x &&
			0 == g_bottom_times_set.data[n].y &&
			0 == g_bottom_times_set.data[n].z) {
			continue;
		}
        
        offset_cnt++;
        offset_sum += g_bottom_times_set.data[n].diffOfRM;
	}
    
    return ((offset_cnt == 0) ? 0 : (U8)(offset_sum / offset_cnt));
    
    
}





