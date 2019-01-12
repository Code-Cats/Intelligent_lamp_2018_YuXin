#include "control.h"
#include "led.h"
#include "timer.h"
#include "base_deal.h"
#include "usart.h"	
#include "light_dimmer.h"

extern LEDControlTypeDef LED_Control;

void LED_Runing(void)
{
	switch(LED_Control.state)	//�����ÿ2ms����һ�Σ�����Ƶ�����ڲ������������
	{
		case POWER_OFF:		//��״̬��Ϩ��״̬ LED=1
		case OFF_BRIGHT:	//�����״̬����Ϊ�Ѿ���һ���Ǹ��ŵĹ��������⣬�ȴ���ǿ���;��Զ��򿪣�ͨ��ѭ����¼�ý׶ι�ǿ����
		case OFF_DARK:	//��ʱû�õ����״̬
		{
			LED=1;	//Ϩ��
			break;
		}
		case AUTO_POWER_ON:	//����״̬�ǲ�ȷ���Ŀ�״̬��Ҳ�������Ϊ��ʱ�ر�״̬
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
		case ON_FIXED_LIGHT:	//��������״̬Ϊ��״̬
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


