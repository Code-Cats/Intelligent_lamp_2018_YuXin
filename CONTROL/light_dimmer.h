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
		u8 trigger_flag;	//干扰标志
		u16 recover_count;	//干扰标志恢复的计数
	}Dis_diff;
	struct
	{
		u8 record[RECODE_NUM];
		u8 recode_count;
		u16 ratio;
		float ratio_f;
		float ratio_filter;
//		u8 updata_enable;	//更新使能位，若不为一则不使能
	}RatioData;
	struct
	{
		u16 AF_up_count;
		u16 AF_down_count;
	}Touch_AF;	//调光时按键复用
}LightDimmerTypeDef;	//调光结构体

typedef enum
{ 
	POWER_OFF,	//关状态
	OFF_BRIGHT,	//关闭时外界亮
	OFF_DARK,	//关闭时外界由亮至暗
	AUTO_POWER_ON,	//根据环境光自动开
	WAIT_AUTO_OFF,	//自动关灯的延时过程
	AUTO_POWER_OFF,	//自动关过渡态
	ON_FIXED_LIGHT,	//固定光强状态
	ON_DIMMING,	//调光状态
}LED_ControlStateTypeDef;	//控制状态枚举

#define AUTO_ON_RECOED_NUM 30	//自动开启中对亮状态的记录数组元素数
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
		u16 bright_record[AUTO_ON_RECOED_NUM];	//循环记录
		u16 dark_recoed[10];
		u16 wait_confirm_time;
	}Auto_ON;	//关的时候外面是亮的
	struct
	{
		u16 rawdata;
	}Lux;
	struct
	{
		u16 Main_pwmTar;	//当前目标值
		u16 Main_pwmDeal;	//缓慢跟随目标值 相当于渐变模块复用兼并/机智
	}Out;
	u8 touchPower_last;
}
LEDControlTypeDef;	//LED控制结构体


typedef struct
{
	u8 test;
}
Adaptive_DimmingTypeDef;	//自动调光结构体

void LED_dimmer(void);

void GY_53_dis_diff(u16 dis);

void Dis_dimmer_Debounce(u16 dis);	//跳变消除函数

void LED_ControlTask(void);

u8 wait_AutoOn_ACK(void);	//暂定这个名字	//手挥一挥确认 回到定光模式

u16 calc_ratioFormLux(u16 lux_last,u16 lux_now);	//通过两个光强计算需补充光源PWM占空比

u16 Averaging_u16(u16 data[],u16 num);	//求u16平均值
#endif


