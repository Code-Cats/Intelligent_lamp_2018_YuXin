#include "touch.h"


void Touch_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	   
	 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4;			    
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 	 //上拉输入
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //IO口速度为50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);			     
  GPIO_SetBits(GPIOA,GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4);	
}
