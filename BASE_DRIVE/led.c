#include "led.h"

void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	    //使能PC端口时钟
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//调用GPIO重映射函数
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;			    //LED0-->PA.4 端口配置
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	 //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //IO口速度为50MHz
  GPIO_Init(GPIOC, &GPIO_InitStructure);			     //初始化GPIOA.4
  GPIO_SetBits(GPIOC,GPIO_Pin_13);					//PA.4 输出高
}


//void LED_Init(void)
//{
//	GPIO_InitTypeDef  GPIO_InitStructure;
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	    //使能PC端口时钟
//	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//调用GPIO重映射函数
//	
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;			    //LED0-->PA.4 端口配置
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	 //推挽输出
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //IO口速度为50MHz
//  GPIO_Init(GPIOA, &GPIO_InitStructure);			     //初始化GPIOA.4
//  GPIO_SetBits(GPIOA,GPIO_Pin_1);					//PA.4 输出高
//}
