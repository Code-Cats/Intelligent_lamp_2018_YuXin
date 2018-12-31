#include "GY_53_deal.h"
#include "delay.h"
#include "light_dimmer.h"

extern LightDimmerTypeDef LightDimmer;


extern u32 time_2ms_count;

typedef enum
{
	GY53_WAITING,
	FRAME_HEADER1,
	FRAME_HEADER2,
	DATA_TYPE,
	DATA_SIZE,
	DATA_HIGH,
	DATA_LOW,
	MEASUREMENT_MODE,
	CHECKSUM,
}GY53_STATE_DEF;

GY53_STATE_DEF GY53_STATE=GY53_WAITING;

u8 GY53_mode=0;

u32 time_2ms_last=0;
u32 time_diff=0;
void GY_53_Usart2deal(u8 Res)
{
	switch (GY53_STATE)
	{
		case GY53_WAITING:
		{
			if(Res==0x5A)
			{
				GY53_STATE=FRAME_HEADER1;
			}
			break;
		}
		case FRAME_HEADER1:
		{
			if(Res==0x5A)
			{
				GY53_STATE=DATA_TYPE;
			}
			else	//如果不是连续两个5A
			{
				GY53_STATE=GY53_WAITING;
			}
			break;
		}
		case FRAME_HEADER2:
		{
			break;
		}
		case DATA_TYPE:
		{
			if(Res==0x15)
			{
				GY53_STATE=DATA_SIZE;
			}
			break;
		}
		case DATA_SIZE:
		{
			if(Res==0x03)
			{
				GY53_STATE=DATA_HIGH;
			}
			break;
		}
		case DATA_HIGH:
		{
			LightDimmer.DisData.raw_validflag=0;	//原始数据无效，因为开始接受新的一帧
			LightDimmer.DisData.rawdata=Res;
			LightDimmer.DisData.rawdata=LightDimmer.DisData.rawdata<<8;
			GY53_STATE=DATA_LOW;
			break;
		}
		case DATA_LOW:
		{
			
			LightDimmer.DisData.rawdata+=Res;
			
			LightDimmer.DisData.filterdata_f=0.2f*LightDimmer.DisData.filterdata_f+0.8f*LightDimmer.DisData.rawdata;
			LightDimmer.DisData.filterdata=(u16)LightDimmer.DisData.filterdata_f;
			
			GY53_STATE=MEASUREMENT_MODE;
			LightDimmer.DisData.raw_validflag=1;	//原始数据已经更新完成
			
			time_diff=time_2ms_count-time_2ms_last;
			time_2ms_last=time_2ms_count;
			break;
		}
		case MEASUREMENT_MODE:
		{
			GY53_mode=Res;
			GY53_STATE=CHECKSUM;
			break;
		}
		case CHECKSUM:
		{
			GY53_STATE=GY53_WAITING;
			break;
		}
	}
}



void GY_53_CMD(u8 cmd)
{
	u8 sum=0xA5+cmd;
	
	delay_ms(10);
//	if(USART_GetFlagStatus(USART2,USART_FLAG_TC)== SET)	//如果上一帧发送完成
	{
		USART_SendData(USART2,0xA5);
	}
	delay_ms(10);
//	if(USART_GetFlagStatus(USART2,USART_FLAG_TC)== SET)	//如果上一帧发送完成
	{
		USART_SendData(USART2,cmd);
	}
	delay_ms(10);
//	if(USART_GetFlagStatus(USART2,USART_FLAG_TC)== SET)	//如果上一帧发送完成
	{
		USART_SendData(USART2,sum);
	}
	delay_ms(10);
}


