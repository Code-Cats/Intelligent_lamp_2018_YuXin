#include "control.h"
#include "led.h"
#include "timer.h"
#include "base_deal.h"
#include "usart.h"	


void LED_Runing(void)
{
	if(time_2ms_count%250==0)
	{
//		LED=!LED;
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


