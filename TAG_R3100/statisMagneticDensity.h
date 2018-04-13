
#ifndef _STATIS_MAGN_DENSITY_H_
#define _STATIS_MAGN_DENSITY_H_

typedef struct _Sample_data {
	S16 x;
	S16 y;
	S16 z;
    
    // ģֵ
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

// �Ÿ���ǿ��
typedef struct _Magnetic_density {
	MAGNETIC_STATE state;
	MAGNETIC_LEVEL level;
	U8 done;                 // ���һ����Чͳ��
	U8 val;
	S16 x;
	S16 y;
	S16 z;
}Magnetic_density;

/*
* ��ʼ��
*/
void initStatisSet(Magnetic_density *magnetic_density);

/*
* ͳ��ʵ��
*/
U8 statisProcess(Sample_data data, Magnetic_density *magnetic_density, U8 cmd/*1:��������*/);

/*
* ��ʼ���г����޳�����ͳ��
*/
void initBottomTimesStatisSet(Magnetic_density *magnetic_density);

/*
* ��ʼ���޳�����ͳ��
*/
void initBottomCnsTimeStatisSet(Magnetic_density *magnetic_density);

/*
* �г����޳�ʱͳ��ʵ��
*/
U8 bottomTimesAdjustStatisProcess(Sample_data data, Magnetic_density *magnetic_density);

/*
* �޳�ͳ��ʵ��
*/
U8 bottomCnsTimeAdjustStatisProcess(Sample_data data, Magnetic_density *magnetic_density);

/*
* �жϳ�ʱ���޳�ͳ�ƺ�ĳһ���޳������Ƿ�һ��
*/
U8 matchTimesAndCnsTimeStatis(Magnetic_density *magnetic_density);

/*
* �жϱ���ƫ��ֵ(δ�жϻ�δƫ�ƶ�����0)
*/
U8 getBottomOffsetValue();


#endif


