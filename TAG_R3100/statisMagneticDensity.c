#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common_.h"
#include "statisMagneticDensity.h"

#define MAX_STATIS_SIZE 8     // ���ͳ����
#define STATIS_SET_SIZE 4     // ͳ�Ƽ�
#define STATIS_SUBSET_SIZE 6  // ͳ���Ӽ�

#define BOTTOM_TIMES_STATIS_SET_SIZE 4     // �г����޳�ʱ����ͳ�Ƽ�
#define BOTTOM_TIME_STATIS_SET_SIZE 8      // �޳�����ͳ�Ƽ�
#define BOTTOM_TIME_STATIS_SUBSET_SIZE 8   // �޳�����ͳ���Ӽ�

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

StatisSet g_statis_subset;          // ͳ���Ӽ�6
StatisSet g_statis_set;             // ͳ�Ƽ� 4
StatisSet g_bottom_times_set;       // 5
StatisSet g_bottom_time_subset;     // 8
StatisSet g_bottom_time_set;        // 8


/*
* ��ʼ��
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
* ����
*/
void zeroStatisSet(StatisSet *statis_set) {
    statis_set->len = 0;
}

/*
* ���
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
* ����
*/
U8 queueFullStatisSet(StatisSet *statis_set) {
    return (statis_set->size == statis_set->len);
}

/*
* �ӿ�
*/
U8 queueEmptyStatisSet(StatisSet *statis_set) {
    return (statis_set->len == 0);
}

/*
* ����
*/
void dequeueStatisSet(StatisSet *statis_set) {
    U8 n = 0;
    for (n = 1; n < statis_set->len; n++) {
        statis_set->data[n - 1] = statis_set->data[n];
    }
    statis_set->len = MAX_(statis_set->len - 1, 0);
}

/*
* ����ͳ�Ʒ���;�ֵ
*/
void calculateStatisVarianceMean(StatisSet *statis_set) {
    U8 n = 0;

    if (statis_set->size <= 0) {
        return;
    }

    // ��ֵ
    statis_set->mean = 0;
    int mean = 0;
    for (n = 0; n < statis_set->size; n++) {
        mean += statis_set->data[n].diffOfRM;
    }
    statis_set->mean = mean / statis_set->size;

    // ����
	int var = 0;
    for (n = 0; n < statis_set->size; n++) {
        float diff = (statis_set->data[n].diffOfRM - statis_set->mean);
       var += diff * diff;
    }
    statis_set->var = (U8)(var / statis_set->size);

    // ��ֵ
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
* �ж�ǿ�ȼ���
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
* ��������
*/
U8 dealFullSet(StatisSet *statis_set) {
    U8 ret = 0;

    // �����ֵ����
    calculateStatisVarianceMean(statis_set);

    if (statis_set->var < MAX_((U8)(statis_set->mean * 0.1), 2)) {
        // ���
        zeroStatisSet(statis_set);
        ret = 1;
    }
    else {
        // ����
        dequeueStatisSet(statis_set);
    }

    return ret;
}

/*
* ͳ��ʵ��
*/
U8 statisProcess(Sample_data data, Magnetic_density *magnetic_density, U8 cmd/*1:��������*/) {

	magnetic_density->done = 0;

    if (MAGNETIC_STATE_CONFIRM == magnetic_density->state && 0 == cmd) {
        return 1;
    }

    // ͳ�Ƽ���
    if (1 == queueFullStatisSet(&g_statis_set)) {
        if (1 == dealFullSet(&g_statis_set)) {
          // �ж�ǿ�ȼ���
          magnetic_density->level = judgeMagnLevel(g_statis_set.mean);
          magnetic_density->state = MAGNETIC_STATE_CONFIRM;
          magnetic_density->val = g_statis_set.mean;
          magnetic_density->x = g_statis_set.mean_x;
          magnetic_density->y = g_statis_set.mean_y;
          magnetic_density->z = g_statis_set.mean_z;

		  magnetic_density->done = 1;
        }
    }
    else { // �Ӽ���
        if (1 == queueFullStatisSet(&g_statis_subset)) {
            if (1 == dealFullSet(&g_statis_subset)) {
              Sample_data data;
              data.diffOfRM = g_statis_subset.mean;
              data.x = g_statis_subset.mean_x;
              data.y = g_statis_subset.mean_y;
              data.z = g_statis_subset.mean_z;
              
              // ���뼯
              enqueueStatisSet(&g_statis_set, data);
              
              if (MAGNETIC_STATE_NUL == magnetic_density->state) {
                // �ж�ǿ�ȼ���
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
            // �����Ӽ�
            enqueueStatisSet(&g_statis_subset, data);
        }
    }

    return 1;

}

/*
* ��ʼ���г����޳�����ͳ��
*/
void initBottomTimesStatisSet(Magnetic_density *magnetic_density) {

	memset(&g_bottom_times_set, 0, sizeof(StatisSet));
	g_bottom_times_set.size = BOTTOM_TIMES_STATIS_SET_SIZE;
	magnetic_density->state = MAGNETIC_STATE_NUL;
	magnetic_density->level = MAGNETIC_MIDDLE;
	magnetic_density->val = 0;
}

/*
* ��ʼ���޳�����ͳ��
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
* ����ͳ�Ʒ���;�ֵ
*/
void calculateBottomStatisVarianceMean(StatisSet *statis_set) {
	U8 n = 0;

	if (statis_set->size <= 0) {
		return;
	}

	// ��ֵ
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

	// ����
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
* �޳�����
*/
void deleteDataStatisSet(StatisSet *statis_set) {
	U8 n = 0, k = 0;
	U8 maxDevIndex = 0;
	short dev = 0, maxDev = 0; // ƫ��
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

	// ��ֹһֱȥ����������
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
* ��������
*/
U8 dealFullBottomSet(StatisSet *statis_set) {
	U8 ret = 0;

	// �����ֵ����
	calculateBottomStatisVarianceMean(statis_set);

	if (statis_set->var_x <= 4 && statis_set->var_y <= 4 && statis_set->var_z <= 4) {
		// ���
		zeroStatisSet(statis_set);
		ret = 1;
	}
	else {
		// �޳�����
		deleteDataStatisSet(statis_set);
	}

	return ret;
}

/*
* �г����޳�ʱͳ��ʵ��
*/
U8 bottomTimesAdjustStatisProcess(Sample_data data, Magnetic_density *magnetic_density) {

  	// �����Ӽ�
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
* �޳�ͳ��ʵ��
*/
U8 bottomCnsTimeAdjustStatisProcess(Sample_data data, Magnetic_density *magnetic_density) {

    // ���뵽�Ӽ�
    enqueueStatisSet(&g_bottom_time_subset, data);
    
    // �Ӽ���
    if (1 == queueFullStatisSet(&g_bottom_time_subset)) {
        if (1 == dealFullBottomSet(&g_bottom_time_subset)) {
            Sample_data data;
            data.diffOfRM = g_bottom_time_subset.mean;
            data.x = g_bottom_time_subset.mean_x;
            data.y = g_bottom_time_subset.mean_y;
            data.z = g_bottom_time_subset.mean_z;

            // ���뵽��
            enqueueStatisSet(&g_bottom_time_set, data);
        }
        else {}
    } // end if (1 == queueFullStatisSet(&g_statis_subset))
  
	// ͳ�Ƽ���
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
* �жϳ�ʱ���޳�ͳ�ƺ�ĳһ���޳������Ƿ�һ��
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
* �жϱ���ƫ��ֵ(δ�жϻ�δƫ�ƶ�����0)
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





