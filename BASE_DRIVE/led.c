#include "led.h"

void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	    //ʹ��PC�˿�ʱ��
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//����GPIO��ӳ�亯��
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;			    //LED0-->PA.4 �˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOC, &GPIO_InitStructure);			     //��ʼ��GPIOA.4
  GPIO_SetBits(GPIOC,GPIO_Pin_13);					//PA.4 �����
}


//void LED_Init(void)
//{
//	GPIO_InitTypeDef  GPIO_InitStructure;
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	    //ʹ��PC�˿�ʱ��
//	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//����GPIO��ӳ�亯��
//	
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;			    //LED0-->PA.4 �˿�����
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	 //�������
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //IO���ٶ�Ϊ50MHz
//  GPIO_Init(GPIOA, &GPIO_InitStructure);			     //��ʼ��GPIOA.4
//  GPIO_SetBits(GPIOA,GPIO_Pin_1);					//PA.4 �����
//}
