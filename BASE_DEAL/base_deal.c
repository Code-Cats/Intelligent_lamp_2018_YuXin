#include "base_deal.h"
#include "pwm.h"
#include "switch.h"

SwitchTypeDef Switch={0};

void PWM_PB4_Set(u16 pwm_set)	//������ӡ���£�PB4ΪTIM3-CH1
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
		Infrare_sum[i_count]=0;	//�����������һ��
//		Limit_sum[i_count]=0;	//�������for���һ��
		
		Switch.Infrare.lastdata[i_count][Switch.Infrare.count]=Switch.Infrare.rawdata[i_count];	//��ǰ���������¼����
//		Switch.Limit.lastdata[i_count][Switch.Limit.count]=Switch.Limit.rawdata[i_count];	//��ǰ���������¼����
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
//��������崮�ڷ��ͺ���	//2msִ��һ�Σ�10ms����һ�ν����14400�Ĳ����ʣ�һ���ֽ���ഫ��11λ��11/14400=0.76ms
void MainBoard_SendData(void)
{
	if(USART_GetFlagStatus(USART1,USART_FLAG_TC)== SET)	//�����һ֡�������
	{
		if(SendData.statu==1)
		{
			SendData.data[0]=0x5A;	//��ֹ֡ͷ֡β���ƻ�
			SendData.data[4]=0xA5;	//��ֹ֡ͷ֡β���ƻ�
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

void Data_Receive(u8 data)	//�����崫���������ݽ�����������ͨ�ã�
{
	if(data==0x5A&&ReceiveData.headOK_state==0)
	{
		ReceiveData.valid_state=0;	//���ݽ����ڼ䲻�������ݽ���
		ReceiveData.headOK_state=1;	
		ReceiveData.count=0;	//����count
	}
	
	if(ReceiveData.headOK_state==1)	//֡ͷ���ҵ�
	{
		ReceiveData.databuffer[ReceiveData.count]=data;
		ReceiveData.count++;
		if((data==0xA5&&ReceiveData.count!=5)||(ReceiveData.count>5))	//ʧЧ
		{
			ReceiveData.valid_state=0;
			ReceiveData.headOK_state=0;
			ReceiveData.count=0;	//����count
		}
		else if(data==0xA5&&ReceiveData.count==5)
		{
			ReceiveData.valid_state=1;
			ReceiveData.headOK_state=0;
			ReceiveData.count=0;	//����count
		}
	}
	
	//////////////////////////////��������ݽ�������-->����Ϊ��ʵ����
	
}

