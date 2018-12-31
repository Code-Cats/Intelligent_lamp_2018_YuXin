#include "base_deal.h"
#include "pwm.h"
#include "switch.h"

SwitchTypeDef Switch={0};

void PWM_PB4_Set(u16 pwm_set)	//部分重印射下，PB4为TIM3-CH1
{
	TIM_SetCompare1(TIM3,ESC_CYCLE-pwm_set);
}

void PWM_PB5_Set(u16 pwm_set)
{
	TIM_SetCompare2(TIM3,ESC_CYCLE-pwm_set);
}

void Switch_Scanf(void)
{
//	Switch.Infrare.rawdata[0]=PA2;
//	Switch.Infrare.rawdata[1]=PA3;
//	Switch.Infrare.rawdata[2]=PA0;
//	Switch.Infrare.rawdata[3]=PA1;
//	
//	Switch.Limit.rawdata[0]=PA15;
//	Switch.Limit.rawdata[1]=PB3;
//	Switch.Limit.rawdata[2]=PC14;
//	Switch.Limit.rawdata[3]=PC15;
}

void Switch_Filter(void)
{
	u16 Infrare_sum[4]={0};
//	u16 Limit_sum[4]={0};
	u8 i_count=0;
	u8 j_count=0;
	
	for(i_count=0;i_count<4;i_count++)
	{
		Infrare_sum[i_count]=0;	//借用这里清空一下
//		Limit_sum[i_count]=0;	//借用这个for清空一下
		
		Switch.Infrare.lastdata[i_count][Switch.Infrare.count]=Switch.Infrare.rawdata[i_count];	//当前数据送入记录数组
//		Switch.Limit.lastdata[i_count][Switch.Limit.count]=Switch.Limit.rawdata[i_count];	//当前数据送入记录数组
	}
		
	for(i_count=0;i_count<4;i_count++)
	{
		for(j_count=0;j_count<FILTER_ORDER;j_count++)
		{
			Infrare_sum[i_count]+=Switch.Infrare.lastdata[i_count][j_count];
//			Limit_sum[i_count]+=Switch.Limit.lastdata[i_count][j_count];
		}
	}
	
	for(i_count=0;i_count<4;i_count++)
	{
		Switch.Infrare.dealdata[i_count]=Infrare_sum[i_count]>(FILTER_ORDER/2)?1:0;
//		Switch.Limit.dealdata[i_count]=Limit_sum[i_count]>(FILTER_ORDER/2)?1:0;
	}

	
	Switch.Infrare.count=Switch.Infrare.count<(FILTER_ORDER-1)?(Switch.Infrare.count+1):0;
//	Switch.Limit.count=Switch.Limit.count<(FILTER_ORDER-1)?(Switch.Limit.count+1):0;
	
}


MainBoardSendTypeDef SendData=MAINBOARD_SENDDATA_DEFAULT;
//副板给主板串口发送函数	//2ms执行一次，10ms更新一次结果，14400的波特率，一个字节最多传输11位，11/14400=0.76ms
void MainBoard_SendData(void)
{
	if(USART_GetFlagStatus(USART1,USART_FLAG_TC)== SET)	//如果上一帧发送完成
	{
		if(SendData.statu==1)
		{
			SendData.data[0]=0x5A;	//防止帧头帧尾被破坏
			SendData.data[4]=0xA5;	//防止帧头帧尾被破坏
			USART_SendData(USART1,SendData.data[SendData.count]);
			SendData.count++;
			if(SendData.count>4)
			{
				SendData.statu=0;
				SendData.count=0;
			}
		}
	}
	
}


ReceiveDataTypeDef ReceiveData={0};

void Data_Receive(u8 data)	//从主板传过来的数据解析（主副板通用）
{
	if(data==0x5A&&ReceiveData.headOK_state==0)
	{
		ReceiveData.valid_state=0;	//数据接受期间不进行数据解析
		ReceiveData.headOK_state=1;	
		ReceiveData.count=0;	//重置count
	}
	
	if(ReceiveData.headOK_state==1)	//帧头已找到
	{
		ReceiveData.databuffer[ReceiveData.count]=data;
		ReceiveData.count++;
		if((data==0xA5&&ReceiveData.count!=5)||(ReceiveData.count>5))	//失效
		{
			ReceiveData.valid_state=0;
			ReceiveData.headOK_state=0;
			ReceiveData.count=0;	//重置count
		}
		else if(data==0xA5&&ReceiveData.count==5)
		{
			ReceiveData.valid_state=1;
			ReceiveData.headOK_state=0;
			ReceiveData.count=0;	//重置count
		}
	}
	
	//////////////////////////////这里放数据解析函数-->解析为真实数据
	
}

