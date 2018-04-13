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

BERTH_STATE g_park_state = BERTH_STATE_NUL;     // ��λ״̬
Magnetic_density g_magnetic_density;            // �����Ÿ��ŵ�ͳ����Ϣ
Magnetic_density g_magnetic_base;               // �޳��ų�ǿ��(����ų�+�����ų�)ͳ����Ϣ
Magnetic_density g_magnetic_base_lower;         // �޳�ʱ��ӽ����׵�ͳ����Ϣ

static struct  EMData g_nopark_data;       // ��¼�޳�״̬����һ��ɼ�ֵ
//static struct  EMData g_park_data;        // ��¼�г�״̬����һ��ɼ�ֵ

static MAGNETIC_STATE g_park_statis;           // ��¼���һ��ͣ��ͳ��״̬(����ͣ������ŵ���һֱ�޳������)

unsigned char FixedV_Check();                  // �̶�ֵ�жϳ�λ״̬
//extern unsigned int FreeParkCount;
extern U8 FastGetParkCount;
extern Sontroler_Symple TagCng_symple;
extern U8 m_clearBottomBufferFlag;             // ��Ҫ��ʼ����������׻������ڵ�����
extern SSensor3100   Sensor3100L;

extern U8 ParkState;              // ��ǰͣ��״̬̬�����ڱȽ�ͣ��״̬�Ƿ�ı�
extern void GoSleep(U16 nS);
extern void inToSleep(Pcontroler_Symple TagCng);
//void flashLeds(unsigned char ledNum,unsigned char ledflags);

/******************************function declaration******************************/


/*
* �仯ȷ��(״̬��ת)
*/
U8 changeConfirm(InputInfo input, Pcontroler_Symple TagCng);

/*
* �仯�ж�
*/
U8 changeJudge(InputInfo input);

/*
* ��ȡ�仯��ֵ
*/
void changeThresholdGet(U8 *thr1/*������ֵ*/, U8 *thr2/*�����ֵ*/, InputInfo input);

/*
* ��ȡ�仯��ֵ
*/
void calculateMeagneticFeature(InputInfo input, Sample_data *data);

/*
* ��ʼ��״̬�ж��������
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
* ��λ״̬�л�
*/
U8 berthStateSwithProcess(U8 *parkState, Pcontroler_Symple TagCng) {
    U8 ret = 0;
    static U8 initi = 0xee;
    static BERTH_STATE last_state = BERTH_STATE_NUL;//��ʷ��λ״̬
    BERTH_STATE state = g_park_state;//��ǰ��λ״̬
    Sample_data data;//��ʱ���������ڻ����������е��м�ֵ
    
    //��ʼ��״̬�ж��������
    InputInfo input = initiBerthStateInput();
    
    // ��������ֵ
    calculateMeagneticFeature(input, &data);
    
    //���ε��û���������־��һʱ�������ػ�������
    if (1 == m_clearBottomBufferFlag || initi == 0xee) {
        initStatisSet(&g_magnetic_base);
        initStatisSet(&g_magnetic_density);
		memset(&g_nopark_data, 0, sizeof(struct EMData));
        memset(&g_magnetic_base_lower, 0, sizeof(Magnetic_density));
        
        initi = 0;

    }
	// �˴����ö�̬���׺������Զ�̬���׵������ܽ��д���
	bottomvalAdjustProcess(*parkState);
    
    //��ǰ��λ״̬
    switch (state) {
        case BERTH_STATE_NUL://�޳�
            
          //���//��ǰ��λ״̬����ʷ��λ״̬����������Ҫ�����¼�������е�����
          if (BERTH_STATE_HAVE == last_state) {
              initStatisSet(&g_magnetic_base);// �޳��ų�ǿ��(����ų�+�����ų�)
              initStatisSet(&g_magnetic_density);// �����Ÿ���_�г�״̬��ʹ��
              
              // g_magnetic_base_lowerȡ��ֵ
              g_magnetic_base_lower.val = input.diffOfRM;
              g_magnetic_base_lower.x = input.x;
              g_magnetic_base_lower.y = input.y;
              g_magnetic_base_lower.z = input.z;
          }

          // ͳ�Ƶ�ǰ���ݣ����ڼ���
          statisProcess(data, &g_magnetic_base, 1);
          
          // ������ӽ����׵��޳�ͳ��ֵ
          if (1 == g_magnetic_base.done) {
              // ���Ե�һ��ͳ��ֵ ��ֹ�ʹų�����ʼֵƫС�����
              if (MAGNETIC_STATE_NUL == g_magnetic_base_lower.state) {
                  g_magnetic_base_lower.state = MAGNETIC_STATE_CONFIRM;
              }
              else if (g_magnetic_base_lower.val > g_magnetic_base.val) {
                   g_magnetic_base_lower = g_magnetic_base;
              }
          }
          
          // δ���ͳ��ʱ ������ӽ����׵��޳��ɼ�ֵ
          if (MAGNETIC_STATE_NUL == g_magnetic_base_lower.state &&
               g_magnetic_base_lower.val > input.diffOfRM) {
               g_magnetic_base_lower.val = input.diffOfRM;
               g_magnetic_base_lower.x = input.x;
               g_magnetic_base_lower.y = input.y;
               g_magnetic_base_lower.z = input.z;
          }

          // ���������״̬�ı䣬����Ҫ���������־
          if (changeConfirm(input, TagCng) == STATE_REVERSAL) {
              ret = STATE_REVERSAL;

              *parkState = 1;
              g_park_state = BERTH_STATE_HAVE;
          }
      
          break;
      case BERTH_STATE_HAVE://�г�

          //���//��ǰ��λ״̬����ʷ��λ״̬����������Ҫ�����¼�������е�����
          if (BERTH_STATE_NUL == last_state) {
              initStatisSet(&g_magnetic_density);// �����Ÿ���_�г�״̬��ʹ��
              memset(&g_magnetic_base_lower, 0, sizeof(Magnetic_density));
          }

          // ͳ�Ƶ�ǰ���ݣ����ڼ���
          statisProcess(data, &g_magnetic_density, 1);
          
          // ��¼���һ��ͣ��ͳ��״̬
          g_park_statis = g_magnetic_density.state;

          // ���������״̬�ı䣬����Ҫ���������־
          if (changeConfirm(input, TagCng) == STATE_REVERSAL) {
              ret = STATE_REVERSAL;
              
              *parkState = 0;
              g_park_state = BERTH_STATE_NUL;
          }
          break;
      default:
          break;  
    }

    last_state = (BERTH_STATE) state;//����ǰ״̬���浽��ʷ״̬��

	m_clearBottomBufferFlag = 0;//�屾�׻����־�ر�
  
    return ret;

}

/*
* �仯ȷ��(״̬��ת)
*/
U8 changeConfirm(InputInfo input, Pcontroler_Symple TagCng) {
    U8 ret = 0;
    if (changeJudge(input) == 1) {
        U8 i = 0;
        
        U16 SleepTime=1024;//˯��ʱ�䣬�ɳ�λ״̬ȷ����Ĭ��Ϊ����
        if (ParkState==0)  SleepTime=2048;//��λ״̬����ȷ���ٶȣ��޳�-->�г�������ȷ�ϣ��г�-->�޳�������ȷ��
        
        ret = STATE_REVERSAL;//Ĭ��״̬�����˷�ת
            
        FastGetParkCount=0;
        for (i = 0; i < 3; i++){
            LED2_clr;
            enableInterrupts();////���ж�,���ж϶�����˯�߱����Ѻ�ִ��
            GoSleep(SleepTime);//20170719
            inToSleep((Pcontroler_Symple)&TagCng_symple);//����˯��      
            //flashLeds(2, TagCng_symple.Config.TagNode.LEDFlag);
            Getdataa(TagCng, 1);//�ش����ݴ���
            
            // ���³�ʼ��input
            InputInfo input_temp = initiBerthStateInput();
            if (changeJudge(input_temp) == 0) {//������ټ���з����ж�������������μ�����
              
                FastGetParkCount = 3;//����һ�ο��ټ��
                ret = 0;//ȡ��״̬��ת��־�������˳����ת̬
                
                break;
            } 
        } 
        LED2_clr;
        
        // ��ʱ�����޳�ʱ����
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
* �仯�ж�
*/
U8 changeJudge(InputInfo input) {
    U8 ret = 0;
    U8 thr_absol = 0;  // ������ֵ
    U8 thr_change = 0; // �仯������ֵ
   // U8 ValueWeightFixed=0;//�̶���ֵȨ��ϵ��
    signed short range_RM = 0,range_abs_diff = 0,range_RM1=0;
 //   signed short Data_Temp=0;
    
    // ��ѯ��ֵ
    changeThresholdGet(&thr_absol, &thr_change, input);

    // �г�ʱ
    if (BERTH_STATE_HAVE == g_park_state) {
        // range_RM ����
        if (MAGNETIC_STATE_NUL == g_magnetic_density.state) {
            range_RM = 0;

        }
        else {
          
            // ��ǰֵ�г�ͳ�Ƶ�xyz���Բ�ֵ
            range_abs_diff = abs(g_magnetic_density.x - input.x) +\
				             abs(g_magnetic_density.y - input.y) +\
				             abs(g_magnetic_density.z - input.z);
            
            // range_RM �仯ֵ(ȡ�뱾�׵�ģ��ֵ���г�ͳ�Ƶ�xyz���Բ�ֵ�����ֵ)
            // ��С
            if (g_magnetic_density.val > input.diffOfRM) {
              
                range_RM = (g_magnetic_density.val - input.diffOfRM);
                range_RM = 0 - MAX_(range_RM, MIN_(range_abs_diff, range_RM * 2));
                
            } else {
                range_RM = (input.diffOfRM - g_magnetic_density.val); // ���ʹ�ñ仯�ж�
            }
        }
        
        // xyzֵ���䵽�޳�ʱͳ��ֵֵ����
		range_RM1 = abs(g_magnetic_base.x - input.x) +\
                    abs(g_magnetic_base.y - input.y) +\
                    abs(g_magnetic_base.z - input.z);
        
        /**********************************�г����޳��ж�**********************************/
        
        // ͨ�������ص����׸����ж��޳�
        if (input.diffOfRM < thr_absol) {
            ret = 1;
        } else if (MAGNETIC_STATE_NUL != g_magnetic_density.state && 
                   range_RM1 < input.RM_thr / 2 && range_abs_diff > input.RM_thr / 2) {
          ret = 1;
        }
        
        // ͨ���Ŷ�ֵ�ı仯�����ж��޳�
        if ((0 - range_RM) > thr_change) {
            ret = 1;
        }

    } else {
        // ���
        if (g_magnetic_base.val < input.diffOfRM) {
              
            range_RM = (input.diffOfRM - g_magnetic_base.val);
        } else {
            range_RM = (g_magnetic_base.val - input.diffOfRM);
        }
        
		
        // ��ǰ����ֵ����һ�β���ֵ(������)�ľ��Բ�ֵ
		signed short diff_nopark = abs(g_nopark_data.data_x - input.x) +\
                        abs(g_nopark_data.data_y - input.y) +\
                        abs(g_nopark_data.data_z - input.z);
        
        // xyzֵ���䵽��һ���г�ֵ����(����ͣ������ŵ���һֱ�޳������)
        signed short diff_park = 0;
        if (g_park_statis != MAGNETIC_STATE_NUL) {
            diff_park = abs(g_magnetic_density.x - input.x) + \
                        abs(g_magnetic_density.y - input.y) + \
                        abs(g_magnetic_density.z - input.z);
        } else {
            diff_park = 127;
        }
        
        // range_RM �仯ֵ(ȡ�뱾�׵�ģ��ֵ���޳�ʱͳ�Ƶ�xyz���Բ�ֵ�����ֵ)
        if (MAGNETIC_STATE_NUL != g_magnetic_base.state) {
            range_abs_diff = abs(g_magnetic_base.x - input.x) +\
                             abs(g_magnetic_base.y - input.y) +\
                             abs(g_magnetic_base.z - input.z);
        }
        range_RM = MAX_(range_RM, range_abs_diff);
        
        
        // range_RM1 �仯ֵ(ȡ�뱾���������ͳ��ֵ����ͳ��ֵʱ�ο���diff_nopark���ֵ)
        range_RM1 = abs(g_magnetic_base_lower.x - input.x) +\
                    abs(g_magnetic_base_lower.y - input.y) +\
                    abs(g_magnetic_base_lower.z - input.z);
		if (MAGNETIC_STATE_CONFIRM != g_magnetic_base_lower.state) {
            range_RM1 = MAX_(diff_nopark, range_RM1);
		}
        
        // ����ƫ��ֵ
        U8 bottomOffsetValue = getBottomOffsetValue();
        if (bottomOffsetValue == 0) {
          bottomOffsetValue = g_magnetic_base.val;
        }
        
        /**********************************�޳����г��ж�**********************************/
        // ͨ���뱾��ģ��ֵ�ж��г�
        if (input.diffOfRM > MIN_(thr_absol * 2, 20)) {
             ret = 1;
		} else if (input.diffOfRM >= thr_absol) {
            if (range_RM1 > input.RM_thr) {
			    ret = 1;
            } else if (diff_park < 5 && bottomOffsetValue <= 8) {
                ret = 1;
            }
        }
        
        // ͨ���뱾��(x, y, z)�仯�����ж��г�
        if (range_RM > thr_change && input.diffOfRM >= 5) {
              ret = 1;
        }
        
        // ͨ������(x, y, z)�仯Ȩֵ�ж��г�
        if (FixedV_Check() >= 3 && diff_nopark > input.RM_thr / 2) {
              ret = 1;
        }
    }


    return ret;
  
}

/*
* ��ȡ�仯��ֵ
*/
void changeThresholdGet(U8 *thr1/*������ֵ*/, U8 *thr2/*�����ֵ*/, InputInfo input) {

    U8 offset = 0;
    //�г����޳�ʱ����ֵ�趨��һ��
    
    // �г�ʱ
    if (BERTH_STATE_HAVE == g_park_state) {
        offset = g_magnetic_base.val;
        offset = MIN_(offset, 10);

        if (MAGNETIC_STATE_NUL == g_magnetic_density.state) {
          
            //������ֵ
            *thr1 = (U8)(input.RM_thr * 8 / 10 + offset * offset * 0.0381 + offset * 0.1216 + 0.015);
            
            //�����ֵ
            *thr2 = input.diffOfRM;
        } else {//if (MAGNETIC_STATE_INIT == g_magnetic_density.state) {
          
           //������ֵ
            *thr1 = (U8)(MIN_(input.RM_thr * 8 / 10 + offset * offset * 0.0381 + offset * 0.1216 + 0.015, g_magnetic_density.val * 8 / 10));
            *thr1 = MAX_(*thr1, 8);
            
            //�����ֵ
            if (MAGNETIC_STATE_INIT == g_magnetic_density.state)
                *thr2 = g_magnetic_density.val * 8 / 10;
            else
                *thr2 = MAX_(MIN_(g_magnetic_density.val * 5 / 10, 30), 10);
        }
    }
    else {
        offset = g_magnetic_base.val;
        offset = MIN_(offset, 10);
        
        //������ֵ
        *thr1 = (U8)(input.RM_thr + MAX_(0, offset * offset * 0.0381 + offset * 0.1216 + 0.015 - 1.0f));
        
        //�����ֵ
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
* ���������
*/
void calculateMeagneticFeature(InputInfo input, Sample_data *data) {
	memset(data, 0, sizeof(Sample_data));

	data->x = input.x;
	data->y = input.y;
	data->z = input.z;
	data->diffOfRM = input.diffOfRM;
	// �Ƕȼ���

}

