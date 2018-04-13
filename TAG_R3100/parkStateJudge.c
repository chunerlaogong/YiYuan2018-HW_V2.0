#include <stdlib.h>
#include <string.h>
#include "Common.h"
#include  "McuTarger.h"
#include "Iic.h"
#include "RegPscStruct.h"
#include "struct.h"
#include "compilefiles.h"
#include "stm8l15x_exti.h"
#include "stm8l15x_it.h"

#include "common_.h"
#include "struct.h"

#include "statisMagneticDensity.h"
#include "R3100.h"
#include "bottomvalAdjustAdaptive.h"

#include "parkStateJudge.h"

/******************************global declaration******************************/

BERTH_STATE g_park_state = BERTH_STATE_NUL;     // 车位状态
Magnetic_density g_magnetic_density;            // 车辆磁干扰的统计信息
Magnetic_density g_magnetic_base;               // 无车磁场强度(地球磁场+环境磁场)统计信息
Magnetic_density g_magnetic_base_lower;         // 无车时最接近本底的统计信息

static struct  EMData g_nopark_data;       // 记录无车状态最新一组采集值
//static struct  EMData g_park_data;        // 记录有车状态最新一组采集值

static MAGNETIC_STATE g_park_statis;           // 记录最近一次停车统计状态(用于停车后干扰导致一直无车的情况)

unsigned char FixedV_Check();                  // 固定值判断车位状态
//extern unsigned int FreeParkCount;
extern U8 FastGetParkCount;
extern Sontroler_Symple TagCng_symple;
extern U8 m_clearBottomBufferFlag;             // 需要初始化，清除本底缓冲区内的数据
extern SSensor3100   Sensor3100L;

extern U8 ParkState;              // 当前停车状态态，用于比较停车状态是否改变
extern void GoSleep(U16 nS);
extern void inToSleep(Pcontroler_Symple TagCng);
//void flashLeds(unsigned char ledNum,unsigned char ledflags);

/******************************function declaration******************************/


/*
* 变化确认(状态反转)
*/
U8 changeConfirm(InputInfo input, Pcontroler_Symple TagCng);

/*
* 变化判断
*/
U8 changeJudge(InputInfo input);

/*
* 获取变化阈值
*/
void changeThresholdGet(U8 *thr1/*绝对阈值*/, U8 *thr2/*相对阈值*/, InputInfo input);

/*
* 获取变化阈值
*/
void calculateMeagneticFeature(InputInfo input, Sample_data *data);

/*
* 初始化状态判断输入参数
*/
InputInfo initiBerthStateInput() {
  
    InputInfo input;
    input.x = (Sensor3100L.EMData_x );
    input.y = (Sensor3100L.EMData_y );
    input.z = (Sensor3100L.EMData_z );

//	SEMData btmVal = getRealTimeBottom();
//    input.x_b = TYPE_S8(btmVal.data_x);
//    input.y_b = TYPE_S8(btmVal.data_y);
//    input.z_b = TYPE_S8(btmVal.data_z);

	input.x_b = TYPE_S8(TagCng_symple.Config.TagPara.EMData_xBottom);
    input.y_b = TYPE_S8(TagCng_symple.Config.TagPara.EMData_yBottom);
    input.z_b = TYPE_S8(TagCng_symple.Config.TagPara.EMData_zBottom);
	
    input.diffOfRM = (Sensor3100L.diffOfRM > 127) ? 127 : (U8)Sensor3100L.diffOfRM;
    input.RM_thr = (U8)TagCng_symple.Config.TagPara.GetEMBottom_RFModThreshold;
    input.angle_thr = (U8)TagCng_symple.Config.TagPara.GetEMBottom_RFAngleThreshold;
    
    return input;
}

/*
* 泊位状态切换
*/
U8 berthStateSwithProcess(U8 *parkState, Pcontroler_Symple TagCng) {
    U8 ret = 0;
    static U8 initi = 0xee;
    static BERTH_STATE last_state = BERTH_STATE_NUL;//历史车位状态
    BERTH_STATE state = g_park_state;//当前车位状态
    Sample_data data;//临时变量，用于缓存计算过程中的中间值
    
    //初始化状态判断输入参数
    InputInfo input = initiBerthStateInput();
    
    // 计算特征值
    calculateMeagneticFeature(input, &data);
    
    //初次调用或清除缓存标志置一时，清除相关缓存区域
    if (1 == m_clearBottomBufferFlag || initi == 0xee) {
        initStatisSet(&g_magnetic_base);
        initStatisSet(&g_magnetic_density);
		memset(&g_nopark_data, 0, sizeof(struct EMData));
        memset(&g_magnetic_base_lower, 0, sizeof(Magnetic_density));
        
        initi = 0;

    }
	// 此处调用动态本底函数，对动态本底调整功能进行处理
	bottomvalAdjustProcess(*parkState);
    
    //当前车位状态
    switch (state) {
        case BERTH_STATE_NUL://无车
            
          //如果//当前车位状态与历史车位状态不符，则需要清除记录缓存区中的数据
          if (BERTH_STATE_HAVE == last_state) {
              initStatisSet(&g_magnetic_base);// 无车磁场强度(地球磁场+环境磁场)
              initStatisSet(&g_magnetic_density);// 车辆磁干扰_有车状态下使用
              
              // g_magnetic_base_lower取初值
              g_magnetic_base_lower.val = input.diffOfRM;
              g_magnetic_base_lower.x = input.x;
              g_magnetic_base_lower.y = input.y;
              g_magnetic_base_lower.z = input.z;
          }

          // 统计当前数据，用于计算
          statisProcess(data, &g_magnetic_base, 1);
          
          // 保存最接近本底的无车统计值
          if (1 == g_magnetic_base.done) {
              // 忽略第一次统计值 防止低磁车辆开始值偏小的情况
              if (MAGNETIC_STATE_NUL == g_magnetic_base_lower.state) {
                  g_magnetic_base_lower.state = MAGNETIC_STATE_CONFIRM;
              }
              else if (g_magnetic_base_lower.val > g_magnetic_base.val) {
                   g_magnetic_base_lower = g_magnetic_base;
              }
          }
          
          // 未完成统计时 保存最接近本底的无车采集值
          if (MAGNETIC_STATE_NUL == g_magnetic_base_lower.state &&
               g_magnetic_base_lower.val > input.diffOfRM) {
               g_magnetic_base_lower.val = input.diffOfRM;
               g_magnetic_base_lower.x = input.x;
               g_magnetic_base_lower.y = input.y;
               g_magnetic_base_lower.z = input.z;
          }

          // 如果发生了状态改变，则需要处理后续标志
          if (changeConfirm(input, TagCng) == STATE_REVERSAL) {
              ret = STATE_REVERSAL;

              *parkState = 1;
              g_park_state = BERTH_STATE_HAVE;
          }
      
          break;
      case BERTH_STATE_HAVE://有车

          //如果//当前车位状态与历史车位状态不符，则需要清除记录缓存区中的数据
          if (BERTH_STATE_NUL == last_state) {
              initStatisSet(&g_magnetic_density);// 车辆磁干扰_有车状态下使用
              memset(&g_magnetic_base_lower, 0, sizeof(Magnetic_density));
          }

          // 统计当前数据，用于计算
          statisProcess(data, &g_magnetic_density, 1);
          
          // 记录最近一次停车统计状态
          g_park_statis = g_magnetic_density.state;

          // 如果发生了状态改变，则需要处理后续标志
          if (changeConfirm(input, TagCng) == STATE_REVERSAL) {
              ret = STATE_REVERSAL;
              
              *parkState = 0;
              g_park_state = BERTH_STATE_NUL;
          }
          break;
      default:
          break;  
    }

    last_state = (BERTH_STATE) state;//将当前状态保存到历史状态中

	m_clearBottomBufferFlag = 0;//清本底缓存标志关闭
  
    return ret;

}

/*
* 变化确认(状态反转)
*/
U8 changeConfirm(InputInfo input, Pcontroler_Symple TagCng) {
    U8 ret = 0;
    if (changeJudge(input) == 1) {
        U8 i = 0;
        
        U16 SleepTime=1024;//睡眠时间，由车位状态确定，默认为快速
        if (ParkState==0)  SleepTime=2048;//车位状态决定确认速度，无车-->有车，慢速确认，有车-->无车，快速确认
        
        ret = STATE_REVERSAL;//默认状态发生了反转
            
        FastGetParkCount=0;
        for (i = 0; i < 3; i++){
            LED2_clr;
            enableInterrupts();////开中断,关中断动作在睡眠被唤醒后执行
            GoSleep(SleepTime);//20170719
            inToSleep((Pcontroler_Symple)&TagCng_symple);//进入睡眠      
            //flashLeds(2, TagCng_symple.Config.TagNode.LEDFlag);
            Getdataa(TagCng, 1);//地磁数据处理
            
            // 重新初始化input
            InputInfo input_temp = initiBerthStateInput();
            if (changeJudge(input_temp) == 0) {//如果快速检测中发现有抖动，则放弃本次检测进程
              
                FastGetParkCount = 3;//启动一次快速检测
                ret = 0;//取消状态反转标志，立即退出检查转态
                
                break;
            } 
        } 
        LED2_clr;
        
        // 及时保存无车时数据
        if (1 == ParkState) {
            g_nopark_data.data_x = input.x;
            g_nopark_data.data_y = input.y;
            g_nopark_data.data_z = input.z;		
        }
    } // end if (changeJudge(input) == 1)
	else {
        if (0 == ParkState) {
            g_nopark_data.data_x = input.x;
            g_nopark_data.data_y = input.y;
            g_nopark_data.data_z = input.z;
        }
	}

    return ret;
  
}

/*
* 变化判断
*/
U8 changeJudge(InputInfo input) {
    U8 ret = 0;
    U8 thr_absol = 0;  // 绝对阈值
    U8 thr_change = 0; // 变化幅度阈值
   // U8 ValueWeightFixed=0;//固定阀值权重系数
    signed short range_RM = 0,range_abs_diff = 0,range_RM1=0;
 //   signed short Data_Temp=0;
    
    // 查询阈值
    changeThresholdGet(&thr_absol, &thr_change, input);

    // 有车时
    if (BERTH_STATE_HAVE == g_park_state) {
        // range_RM 降幅
        if (MAGNETIC_STATE_NUL == g_magnetic_density.state) {
            range_RM = 0;

        }
        else {
          
            // 当前值有车统计的xyz绝对差值
            range_abs_diff = abs(g_magnetic_density.x - input.x) +\
				             abs(g_magnetic_density.y - input.y) +\
				             abs(g_magnetic_density.z - input.z);
            
            // range_RM 变化值(取与本底的模差值和有车统计的xyz绝对差值的最大值)
            // 变小
            if (g_magnetic_density.val > input.diffOfRM) {
              
                range_RM = (g_magnetic_density.val - input.diffOfRM);
                range_RM = 0 - MAX_(range_RM, MIN_(range_abs_diff, range_RM * 2));
                
            } else {
                range_RM = (input.diffOfRM - g_magnetic_density.val); // 变大不使用变化判断
            }
        }
        
        // xyz值回落到无车时统计值值附近
		range_RM1 = abs(g_magnetic_base.x - input.x) +\
                    abs(g_magnetic_base.y - input.y) +\
                    abs(g_magnetic_base.z - input.z);
        
        /**********************************有车到无车判断**********************************/
        
        // 通过基本回到本底附近判断无车
        if (input.diffOfRM < thr_absol) {
            ret = 1;
        } else if (MAGNETIC_STATE_NUL != g_magnetic_density.state && 
                   range_RM1 < input.RM_thr / 2 && range_abs_diff > input.RM_thr / 2) {
          ret = 1;
        }
        
        // 通过扰动值的变化幅度判断无车
        if ((0 - range_RM) > thr_change) {
            ret = 1;
        }

    } else {
        // 变大
        if (g_magnetic_base.val < input.diffOfRM) {
              
            range_RM = (input.diffOfRM - g_magnetic_base.val);
        } else {
            range_RM = (g_magnetic_base.val - input.diffOfRM);
        }
        
		
        // 当前采样值与上一次采样值(无跳变)的绝对差值
		signed short diff_nopark = abs(g_nopark_data.data_x - input.x) +\
                        abs(g_nopark_data.data_y - input.y) +\
                        abs(g_nopark_data.data_z - input.z);
        
        // xyz值回落到上一次有车值附近(用于停车后干扰导致一直无车的情况)
        signed short diff_park = 0;
        if (g_park_statis != MAGNETIC_STATE_NUL) {
            diff_park = abs(g_magnetic_density.x - input.x) + \
                        abs(g_magnetic_density.y - input.y) + \
                        abs(g_magnetic_density.z - input.z);
        } else {
            diff_park = 127;
        }
        
        // range_RM 变化值(取与本底的模差值和无车时统计的xyz绝对差值的最大值)
        if (MAGNETIC_STATE_NUL != g_magnetic_base.state) {
            range_abs_diff = abs(g_magnetic_base.x - input.x) +\
                             abs(g_magnetic_base.y - input.y) +\
                             abs(g_magnetic_base.z - input.z);
        }
        range_RM = MAX_(range_RM, range_abs_diff);
        
        
        // range_RM1 变化值(取与本底最相近的统计值，无统计值时参考与diff_nopark最大值)
        range_RM1 = abs(g_magnetic_base_lower.x - input.x) +\
                    abs(g_magnetic_base_lower.y - input.y) +\
                    abs(g_magnetic_base_lower.z - input.z);
		if (MAGNETIC_STATE_CONFIRM != g_magnetic_base_lower.state) {
            range_RM1 = MAX_(diff_nopark, range_RM1);
		}
        
        // 本底偏移值
        U8 bottomOffsetValue = getBottomOffsetValue();
        if (bottomOffsetValue == 0) {
          bottomOffsetValue = g_magnetic_base.val;
        }
        
        /**********************************无车到有车判断**********************************/
        // 通过与本底模差值判断有车
        if (input.diffOfRM > MIN_(thr_absol * 2, 20)) {
             ret = 1;
		} else if (input.diffOfRM >= thr_absol) {
            if (range_RM1 > input.RM_thr) {
			    ret = 1;
            } else if (diff_park < 5 && bottomOffsetValue <= 8) {
                ret = 1;
            }
        }
        
        // 通过与本底(x, y, z)变化幅度判断有车
        if (range_RM > thr_change && input.diffOfRM >= 5) {
              ret = 1;
        }
        
        // 通过本底(x, y, z)变化权值判断有车
        if (FixedV_Check() >= 3 && diff_nopark > input.RM_thr / 2) {
              ret = 1;
        }
    }


    return ret;
  
}

/*
* 获取变化阈值
*/
void changeThresholdGet(U8 *thr1/*绝对阈值*/, U8 *thr2/*相对阈值*/, InputInfo input) {

    U8 offset = 0;
    //有车或无车时，阈值设定不一致
    
    // 有车时
    if (BERTH_STATE_HAVE == g_park_state) {
        offset = g_magnetic_base.val;
        offset = MIN_(offset, 10);

        if (MAGNETIC_STATE_NUL == g_magnetic_density.state) {
          
            //绝对阈值
            *thr1 = (U8)(input.RM_thr * 8 / 10 + offset * offset * 0.0381 + offset * 0.1216 + 0.015);
            
            //相对阈值
            *thr2 = input.diffOfRM;
        } else {//if (MAGNETIC_STATE_INIT == g_magnetic_density.state) {
          
           //绝对阈值
            *thr1 = (U8)(MIN_(input.RM_thr * 8 / 10 + offset * offset * 0.0381 + offset * 0.1216 + 0.015, g_magnetic_density.val * 8 / 10));
            *thr1 = MAX_(*thr1, 8);
            
            //相对阈值
            if (MAGNETIC_STATE_INIT == g_magnetic_density.state)
                *thr2 = g_magnetic_density.val * 8 / 10;
            else
                *thr2 = MAX_(MIN_(g_magnetic_density.val * 5 / 10, 30), 10);
        }
    }
    else {
        offset = g_magnetic_base.val;
        offset = MIN_(offset, 10);
        
        //绝对阈值
        *thr1 = (U8)(input.RM_thr + MAX_(0, offset * offset * 0.0381 + offset * 0.1216 + 0.015 - 1.0f));
        
        //相对阈值
        if (MAGNETIC_STATE_NUL == g_magnetic_base.state) {
            *thr2 = (U8)(input.diffOfRM * 2);
        } else {//if (MAGNETIC_STATE_INIT == g_magnetic_density.state) {
            if (MAGNETIC_STATE_INIT == g_magnetic_base.state)
                *thr2 =  (U8)(input.RM_thr + MAX_(offset * 0.6, 6));
            else
                *thr2 = (U8)(input.RM_thr + MAX_(offset * 0.5, 5));
        }
    }

}

/*
* 计算磁特征
*/
void calculateMeagneticFeature(InputInfo input, Sample_data *data) {
	memset(data, 0, sizeof(Sample_data));

	data->x = input.x;
	data->y = input.y;
	data->z = input.z;
	data->diffOfRM = input.diffOfRM;
	// 角度计算

}

