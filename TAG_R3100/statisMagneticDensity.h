
#ifndef _STATIS_MAGN_DENSITY_H_
#define _STATIS_MAGN_DENSITY_H_

typedef struct _Sample_data {
	S16 x;
	S16 y;
	S16 z;
    
    // 模值
	U8 diffOfRM;
}Sample_data;
typedef enum {
	MAGNETIC_STATE_NUL,
	MAGNETIC_STATE_INIT,
	MAGNETIC_STATE_CONFIRM
}MAGNETIC_STATE;

typedef enum {
	MAGNETIC_HIGHT,
	MAGNETIC_MIDDLE,
	MAGNETIC_LOW
}MAGNETIC_LEVEL;

// 磁干扰强度
typedef struct _Magnetic_density {
	MAGNETIC_STATE state;
	MAGNETIC_LEVEL level;
	U8 done;                 // 完成一轮有效统计
	U8 val;
	S16 x;
	S16 y;
	S16 z;
}Magnetic_density;

/*
* 初始化
*/
void initStatisSet(Magnetic_density *magnetic_density);

/*
* 统计实现
*/
U8 statisProcess(Sample_data data, Magnetic_density *magnetic_density, U8 cmd/*1:持续更新*/);

/*
* 初始化有车且无车数据统计
*/
void initBottomTimesStatisSet(Magnetic_density *magnetic_density);

/*
* 初始化无车数据统计
*/
void initBottomCnsTimeStatisSet(Magnetic_density *magnetic_density);

/*
* 有车且无车时统计实现
*/
U8 bottomTimesAdjustStatisProcess(Sample_data data, Magnetic_density *magnetic_density);

/*
* 无车统计实现
*/
U8 bottomCnsTimeAdjustStatisProcess(Sample_data data, Magnetic_density *magnetic_density);

/*
* 判断长时间无车统计和某一次无车数据是否一致
*/
U8 matchTimesAndCnsTimeStatis(Magnetic_density *magnetic_density);

/*
* 判断本底偏移值(未判断或未偏移都返回0)
*/
U8 getBottomOffsetValue();


#endif


