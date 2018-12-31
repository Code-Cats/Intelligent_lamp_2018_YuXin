#ifndef BASE_DEAL_H
#define BASE_DEAL_H

#include "sys.h"

void PWM_PB4_Set(u16 pwm_set);
void PWM_PB5_Set(u16 pwm_set);
void MainBoard_SendData(void);
void Switch_Scanf(void);
void Switch_Filter(void);
void Data_Receive(u8 data);	//从主板传过来的数据解析（主副板通用）

#define FILTER_ORDER 16
typedef struct
{
	struct
	{
		u8 rawdata[4];
		u8 count;
		u8 lastdata[4][FILTER_ORDER];
		u8 dealdata[4];
	}Infrare;
	struct
	{
		u8 rawdata[4];
		u8 count;
		u8 lastdata[4][FILTER_ORDER];
		u8 dealdata[4];
	}Limit;
}SwitchTypeDef;


typedef struct
{
	u8 statu;
	u8 data[5];
	u8 count;
}MainBoardSendTypeDef;

#define MAINBOARD_SENDDATA_DEFAULT \
{\
	0,\
	{0x5A,0,0,0,0xA5},\
	0,\
}\

typedef struct
{
	u8 headOK_state;
	u8 valid_state;	//数据帧有效标志位
	u8 databuffer[5];
	u8 count;
}ReceiveDataTypeDef;



extern SwitchTypeDef Switch;
extern MainBoardSendTypeDef SendData;
extern ReceiveDataTypeDef ReceiveData;

#endif

