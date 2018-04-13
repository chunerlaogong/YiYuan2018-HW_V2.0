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

static U8 nopark_startstatis_flag = 0;             // �����޳�״̬����

static Magnetic_density g_bottom_times;            // ͨ������޳����ݵ�������
static Magnetic_density g_bottom_longTime;         // ͨ����ʱ���޳�����ͳ�Ʊ���


static SEMData g_realTimeBottom;               // ʵʱ����ֵ
uint8_t GetTimeBottomCount=0;                            // ȡ��ʵʱ���״���,���=63��Ϊ����״̬
//U8 WarningFlag=0;//������־��

extern Magnetic_density g_magnetic_density;        // �����Ÿ���_�г�״̬��ʹ��
extern Magnetic_density g_magnetic_base;           // �޳��ų�ǿ��(����ų�+�����ų�)

extern Sontroler_Symple TagCng_symple;
extern uint8_t m_clearBottomBufferFlag;                 // ������׻����־
extern SSensorbottom DynamicBottom;                // ��̬����

/*
* �ж��Ƿ����㱾��Լ������
*/
uint8_t satisfyBottomConstraintCondition(Magnetic_density *magnetic_density);

void GetDynamicBottom(void)//ȡ��ʵʱ��̬����
{
                        
    //��̬���׼ƴμ�һ��ͨ��inf��Ϣ���������Ϸ�
    if (GetTimeBottomCount>50) 
        GetTimeBottomCount=1;
    else
        GetTimeBottomCount++;
                        
    //�����µĶ�̬����ֵ����ȫ�ֱ������ڱ��׼�����ʹ��
    DynamicBottom.EMData_xBottom =g_realTimeBottom.data_x;//x��̬����ֵ
    DynamicBottom.EMData_yBottom =g_realTimeBottom.data_y;//y��̬����ֵ
    DynamicBottom.EMData_zBottom =g_realTimeBottom.data_z;//z��̬����ֵ  
}

/*
* ����ֵ����Ӧ����
*/
U8 bottomvalAdjustProcess(U8 parkState) {
	static U8 initi = 0xee;
	static BERTH_STATE last_state = BERTH_STATE_NUL;//
	Sample_data data;

    // У׼���׻�������� ��ʼ��ͳ����Ϣ
	if (1 == m_clearBottomBufferFlag || initi == 0xee) {
		initBottomTimesStatisSet(&g_bottom_times);
		initBottomCnsTimeStatisSet(&g_bottom_longTime);
		getRealTimeBottom();
		initi = 0;
	}
    
    //ȡ���޳��ų�ǿ��(����ų�+�����ų�)
	data.x = g_magnetic_base.x;
	data.y = g_magnetic_base.y;
	data.z = g_magnetic_base.z;
	data.diffOfRM = g_magnetic_base.val;

    //�г����޳�״̬�ֱ��ж�
	switch (parkState) {
	case BERTH_STATE_NUL://�޳�

		// ͨ������޳����ݵ�������
		if (1 == nopark_startstatis_flag) {
			if (1 == g_magnetic_base.done) {
				if (bottomTimesAdjustStatisProcess(data, &g_bottom_times) == 1) {

					// �ж��Ƿ����㱾�׵�������
					if (satisfyBottomConstraintCondition(&g_bottom_times) == 1) {

						// ��������
						g_realTimeBottom.data_x = g_bottom_times.x;//x��̬����ֵ
						g_realTimeBottom.data_y = g_bottom_times.y;//y��̬����ֵ
						g_realTimeBottom.data_z = g_bottom_times.z;//z��̬����ֵ

                        GetDynamicBottom();//ȡ��ʵʱ��̬����
					} else {
						// ������������̬���׿ɵ���Χ������´οɵ�����������˱�־
                        GetTimeBottomCount=63;

					}
				}
				nopark_startstatis_flag = 0;
			}
		}

		// ͨ����ʱ���޳�����ͳ�Ʊ���
		if (1 == g_magnetic_base.done)
		{
			if (bottomCnsTimeAdjustStatisProcess(data, &g_bottom_longTime) == 1) {

				// �жϺ�ĳһ���޳������Ƿ�һ��
				if (matchTimesAndCnsTimeStatis(&g_bottom_longTime) == 1) {

					// �ж��Ƿ����㱾�׵�������
					if (satisfyBottomConstraintCondition(&g_bottom_longTime) == 1) {
						// ��������
						g_realTimeBottom.data_x = g_bottom_longTime.x;//x��̬����ֵ
						g_realTimeBottom.data_y = g_bottom_longTime.y;//y��̬����ֵ
						g_realTimeBottom.data_z = g_bottom_longTime.z;//z��̬����ֵ  

                        GetDynamicBottom();//ȡ��ʵʱ��̬����
					}
					else {
						// ������������̬���׿ɵ���Χ������´οɵ�����������˱�־
                        GetTimeBottomCount=63;

					}
                    // ��ճ�ʱ���޳�ͳ��
					initBottomCnsTimeStatisSet(&g_bottom_longTime);
				}
			}
		}

		break;
	case BERTH_STATE_HAVE://�г�

		// ��ʼ��
		if (BERTH_STATE_NUL == last_state)
			nopark_startstatis_flag = 0;

		if (MAGNETIC_STATE_CONFIRM == g_magnetic_density.state) {
			nopark_startstatis_flag = 1;
            
            // ��ճ�ʱ���޳�ͳ��
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
* ��ȡʵʱ����ֵ
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
* �ж��Ƿ����㱾��Լ������
*/
U8 satisfyBottomConstraintCondition(Magnetic_density *statisBottom) {

	short xBottom = TagCng_symple.Config.TagPara.EMData_xBottom;
	short yBottom = TagCng_symple.Config.TagPara.EMData_yBottom;
	short zBottom = TagCng_symple.Config.TagPara.EMData_zBottom;
	short x_vary = (statisBottom->x - xBottom);//Xǿ�ȱ仯ʸ����,��λ
	short y_vary = (statisBottom->y - yBottom);//yǿ�ȱ仯ʸ����,��λ
	short z_vary = (statisBottom->z - zBottom);//zǿ�ȱ仯ʸ����,��λ

	short diffOfRM = (short)(sqrtf(x_vary*x_vary + y_vary*y_vary + 2 * z_vary*z_vary));

	x_vary = (statisBottom->x - g_realTimeBottom.data_x);//Xǿ�ȱ仯ʸ����,��λ
	y_vary = (statisBottom->y - g_realTimeBottom.data_y);//yǿ�ȱ仯ʸ����,��λ
	z_vary = (statisBottom->z - g_realTimeBottom.data_z);//zǿ�ȱ仯ʸ����,��λ

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




