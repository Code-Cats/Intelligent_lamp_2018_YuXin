#ifndef __LIGHT_DIMMER_H_
#define __LIGHT_DIMMER_H_

#include "sys.h"


#define ABS(x) x>0?x:-x

#define TOUCHING 1
#define DISTOUCH 0

#define RECODE_NUM 20
typedef struct
{
	struct
	{
		u8 raw_validflag;
		u16 rawdata;
		u16 lastdata;
		u16 filterdata;
		float filterdata_f;
	}DisData;
	struct
	{
		s16 now_data;
		u8 trigger_flag;	//���ű�־
		u16 recover_count;	//���ű�־�ָ��ļ���
	}Dis_diff;
	struct
	{
		u8 record[RECODE_NUM];
		u8 recode_count;
		u16 ratio;
		float ratio_f;
		float ratio_filter;
//		u8 updata_enable;	//����ʹ��λ������Ϊһ��ʹ��
	}RatioData;
	struct
	{
		u16 AF_up_count;
		u16 AF_down_count;
	}Touch_AF;	//����ʱ��������
}LightDimmerTypeDef;	//����ṹ��

typedef enum
{ 
	POWER_OFF,	//��״̬
	OFF_BRIGHT,	//�ر�ʱ�����
	OFF_DARK,	//�ر�ʱ�����������
	AUTO_POWER_ON,	//���ݻ������Զ���
	WAIT_AUTO_OFF,	//�Զ��صƵ���ʱ����
	AUTO_POWER_OFF,	//�Զ��ع���̬
	ON_FIXED_LIGHT,	//�̶���ǿ״̬
	ON_DIMMING,	//����״̬
}LED_ControlStateTypeDef;	//����״̬ö��

#define AUTO_ON_RECOED_NUM 30	//�Զ������ж���״̬�ļ�¼����Ԫ����
typedef struct
{
	LED_ControlStateTypeDef state;
	struct
	{
		u8 enable;
		u8 touchDimmer_last;
		u16 delayOFF_count;
	}dimmer;
	struct
	{
		u8 touchAoff_last;
		u16 wait_off_time;
		u16 wait_off_confim_time;
	}Auto_OFF;
	struct
	{
		u16 bright_count;
		u16 dark_count;
		u16 record_count;
		u16 bright_record[AUTO_ON_RECOED_NUM];	//ѭ����¼
		u16 dark_recoed[10];
		u16 wait_confirm_time;
	}Auto_ON;	//�ص�ʱ������������
	struct
	{
		u16 rawdata;
	}Lux;
	struct
	{
		u16 Main_pwmTar;	//��ǰĿ��ֵ
		u16 Main_pwmDeal;	//��������Ŀ��ֵ �൱�ڽ���ģ�鸴�ü沢/����
	}Out;
	u8 touchPower_last;
}
LEDControlTypeDef;	//LED���ƽṹ��


typedef struct
{
	u8 test;
}
Adaptive_DimmingTypeDef;	//�Զ�����ṹ��

void LED_dimmer(void);

void GY_53_dis_diff(u16 dis);

void Dis_dimmer_Debounce(u16 dis);	//������������

void LED_ControlTask(void);

u8 wait_AutoOn_ACK(void);	//�ݶ��������	//�ֻ�һ��ȷ�� �ص�����ģʽ

u16 calc_ratioFormLux(u16 lux_last,u16 lux_now);	//ͨ��������ǿ�����貹���ԴPWMռ�ձ�

u16 Averaging_u16(u16 data[],u16 num);	//��u16ƽ��ֵ
#endif


