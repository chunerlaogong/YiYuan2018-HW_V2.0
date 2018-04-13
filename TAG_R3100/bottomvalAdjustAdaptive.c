#include <stdlib.h>

#include "Common.h"

#include "common_.h"
#include "struct.h"
#include "statisMagneticDensity.h"
#include "parkStateJudge.h"
#include "R3100.h"
#include "compilefiles.h"
#include "bottomvalAdjustAdaptive.h"


/******************************global declaration******************************/

static U8 nopark_startstatis_flag = 0;             // 单次无车状态调整

static Magnetic_density g_bottom_times;            // 通过多次无车数据调整本底
static Magnetic_density g_bottom_longTime;         // 通过长时间无车数据统计本底


static SEMData g_realTimeBottom;               // 实时本底值
uint8_t GetTimeBottomCount=0;                            // 取得实时本底次数,如果=63则为报警状态
//U8 WarningFlag=0;//报警标志，

extern Magnetic_density g_magnetic_density;        // 车辆磁干扰_有车状态下使用
extern Magnetic_density g_magnetic_base;           // 无车磁场强度(地球磁场+环境磁场)

extern Sontroler_Symple TagCng_symple;
extern uint8_t m_clearBottomBufferFlag;                 // 清除本底缓存标志
extern SSensorbottom DynamicBottom;                // 动态本底

/*
* 判断是否满足本底约束条件
*/
uint8_t satisfyBottomConstraintCondition(Magnetic_density *magnetic_density);

void GetDynamicBottom(void)//取得实时动态本底
{
                        
    //动态本底计次加一，通过inf信息将本计数上发
    if (GetTimeBottomCount>50) 
        GetTimeBottomCount=1;
    else
        GetTimeBottomCount++;
                        
    //将最新的动态本底值赋给全局变量，在本底计算是使用
    DynamicBottom.EMData_xBottom =g_realTimeBottom.data_x;//x动态本底值
    DynamicBottom.EMData_yBottom =g_realTimeBottom.data_y;//y动态本底值
    DynamicBottom.EMData_zBottom =g_realTimeBottom.data_z;//z动态本底值  
}

/*
* 本底值自适应调整
*/
U8 bottomvalAdjustProcess(U8 parkState) {
	static U8 initi = 0xee;
	static BERTH_STATE last_state = BERTH_STATE_NUL;//
	Sample_data data;

    // 校准本底或初次运行 初始化统计信息
	if (1 == m_clearBottomBufferFlag || initi == 0xee) {
		initBottomTimesStatisSet(&g_bottom_times);
		initBottomCnsTimeStatisSet(&g_bottom_longTime);
		getRealTimeBottom();
		initi = 0;
	}
    
    //取得无车磁场强度(地球磁场+环境磁场)
	data.x = g_magnetic_base.x;
	data.y = g_magnetic_base.y;
	data.z = g_magnetic_base.z;
	data.diffOfRM = g_magnetic_base.val;

    //有车或无车状态分别判断
	switch (parkState) {
	case BERTH_STATE_NUL://无车

		// 通过多次无车数据调整本底
		if (1 == nopark_startstatis_flag) {
			if (1 == g_magnetic_base.done) {
				if (bottomTimesAdjustStatisProcess(data, &g_bottom_times) == 1) {

					// 判断是否满足本底调整条件
					if (satisfyBottomConstraintCondition(&g_bottom_times) == 1) {

						// 调整本底
						g_realTimeBottom.data_x = g_bottom_times.x;//x动态本底值
						g_realTimeBottom.data_y = g_bottom_times.y;//y动态本底值
						g_realTimeBottom.data_z = g_bottom_times.z;//z动态本底值

                        GetDynamicBottom();//取得实时动态本底
					} else {
						// 报警，超过动态本底可调范围。如果下次可调整，则清除此标志
                        GetTimeBottomCount=63;

					}
				}
				nopark_startstatis_flag = 0;
			}
		}

		// 通过长时间无车数据统计本底
		if (1 == g_magnetic_base.done)
		{
			if (bottomCnsTimeAdjustStatisProcess(data, &g_bottom_longTime) == 1) {

				// 判断和某一次无车数据是否一致
				if (matchTimesAndCnsTimeStatis(&g_bottom_longTime) == 1) {

					// 判断是否满足本底调整条件
					if (satisfyBottomConstraintCondition(&g_bottom_longTime) == 1) {
						// 调整本底
						g_realTimeBottom.data_x = g_bottom_longTime.x;//x动态本底值
						g_realTimeBottom.data_y = g_bottom_longTime.y;//y动态本底值
						g_realTimeBottom.data_z = g_bottom_longTime.z;//z动态本底值  

                        GetDynamicBottom();//取得实时动态本底
					}
					else {
						// 报警，超过动态本底可调范围。如果下次可调整，则清除此标志
                        GetTimeBottomCount=63;

					}
                    // 清空长时间无车统计
					initBottomCnsTimeStatisSet(&g_bottom_longTime);
				}
			}
		}

		break;
	case BERTH_STATE_HAVE://有车

		// 初始化
		if (BERTH_STATE_NUL == last_state)
			nopark_startstatis_flag = 0;

		if (MAGNETIC_STATE_CONFIRM == g_magnetic_density.state) {
			nopark_startstatis_flag = 1;
            
            // 清空长时间无车统计
            initBottomCnsTimeStatisSet(&g_bottom_longTime);
        }

		break;
	default:
		break;
	}

	last_state =(BERTH_STATE)parkState;

	return 1;

}

/*
* 获取实时本底值
*/
SEMData getRealTimeBottom(void) {

	static U8 initi = 0xee;

	if (1 == m_clearBottomBufferFlag || initi == 0xee) {
		g_realTimeBottom.data_x = (TagCng_symple.Config.TagPara.EMData_xBottom );
		g_realTimeBottom.data_y = (TagCng_symple.Config.TagPara.EMData_yBottom );
		g_realTimeBottom.data_z = (TagCng_symple.Config.TagPara.EMData_zBottom );
		initi = 0;
        GetTimeBottomCount=0;
	}

	return g_realTimeBottom;
}

/*
* 判断是否满足本底约束条件
*/
U8 satisfyBottomConstraintCondition(Magnetic_density *statisBottom) {

	short xBottom = TagCng_symple.Config.TagPara.EMData_xBottom;
	short yBottom = TagCng_symple.Config.TagPara.EMData_yBottom;
	short zBottom = TagCng_symple.Config.TagPara.EMData_zBottom;
	short x_vary = (statisBottom->x - xBottom);//X强度变化矢量差,单位
	short y_vary = (statisBottom->y - yBottom);//y强度变化矢量差,单位
	short z_vary = (statisBottom->z - zBottom);//z强度变化矢量差,单位

	short diffOfRM = (short)(sqrtf(x_vary*x_vary + y_vary*y_vary + 2 * z_vary*z_vary));

	x_vary = (statisBottom->x - g_realTimeBottom.data_x);//X强度变化矢量差,单位
	y_vary = (statisBottom->y - g_realTimeBottom.data_y);//y强度变化矢量差,单位
	z_vary = (statisBottom->z - g_realTimeBottom.data_z);//z强度变化矢量差,单位

	short diffOfRM1 = (short)(sqrtf(x_vary*x_vary + y_vary*y_vary + 2 * z_vary*z_vary));

	if (abs(statisBottom->x - g_realTimeBottom.data_x) <= 15 &&
		abs(statisBottom->y - g_realTimeBottom.data_y) <= 15 &&
		abs(statisBottom->z - g_realTimeBottom.data_z) <= 15 &&
		abs(statisBottom->x - xBottom) <= 15 &&
		abs(statisBottom->y - yBottom) <= 15 &&
		abs(statisBottom->z - zBottom) <= 15 &&
		diffOfRM1 < 15 && diffOfRM < 20)
	{
		return 1;
	}

	return 0;

}




