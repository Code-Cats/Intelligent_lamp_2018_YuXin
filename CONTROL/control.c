#include "control.h"
#include "led.h"
#include "timer.h"
#include "base_deal.h"
#include "usart.h"	
#include "light_dimmer.h"

extern LEDControlTypeDef LED_Control;

void LED_Runing(void)
{
	switch(LED_Control.state)	//该语句每2ms运行一次，其他频率在内部语句再做限制
	{
		case POWER_OFF:		//该状态是熄灭状态 LED=1
		case OFF_BRIGHT:	//进入该状态即认为已经有一个非干扰的工作环境光，等待光强降低就自动打开，通过循环记录该阶段光强采样
		case OFF_DARK:	//暂时没用到这个状态
		{
			LED=1;	//熄灭
			break;
		}
		case AUTO_POWER_ON:	//以下状态是不确定的开状态，也可以理解为定时关闭状态
		case WAIT_AUTO_OFF:
		{
			if(time_2ms_count%400==0)
			{
				LED=!LED;
			}
			break;
		}
		case AUTO_POWER_OFF:
		{
			if(time_2ms_count%250==0)
			{
				LED=!LED;
			}
			break;
		}
		case ON_FIXED_LIGHT:	//以下两个状态为开状态
		case ON_DIMMING:
		{
			LED=0;
			break;
		}
	}
}

void Control_Task(void)
{
	Switch_Scanf();
	Switch_Filter(); 
	if(SendData.statu==0)
	{
		SendData.data[1]=Switch.Limit.rawdata[0]<<7|Switch.Limit.rawdata[1]<<6|Switch.Limit.rawdata[2]<<5|Switch.Limit.rawdata[3]<<4|Switch.Infrare.dealdata[0]<<3|Switch.Infrare.dealdata[1]<<2|Switch.Infrare.dealdata[2]<<1|Switch.Infrare.dealdata[3];
		SendData.statu=1;
	}
	
	MainBoard_SendData();
	
}


