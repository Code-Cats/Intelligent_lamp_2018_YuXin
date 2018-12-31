#include "switch.h"


void Switch_Init(void)
{
//	W1_Init();
//	W2_Init();
//	S1_4_Init();
}

void W1_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	   
	 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;			    
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 	 //上拉输入
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //IO口速度为50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);			     
  GPIO_SetBits(GPIOA,GPIO_Pin_0 | GPIO_Pin_1);				
}

void W2_Init(void)	//和串口2冲突
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	    
	 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;		
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 	 //上拉输入
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //IO口速度为50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);			    
  GPIO_SetBits(GPIOA,GPIO_Pin_2 | GPIO_Pin_3);				
}

void W3_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	   
	 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;			   
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 	 //上拉输入
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //IO口速度为50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);			     
  GPIO_SetBits(GPIOA,GPIO_Pin_6 | GPIO_Pin_7);			
}

void W4_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	   
	 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;			    
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 	 //上拉输入
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //IO口速度为50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);			     
  GPIO_SetBits(GPIOB,GPIO_Pin_0 | GPIO_Pin_1);					//输出高
}

void S1_4_Init(void)
{ 
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	   
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	    //使能PB端口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	    //使能PC端口时钟
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//调用GPIO重映射函数
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;			   
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //IO口速度为50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);			    
//  GPIO_ResetBits(GPIOA,GPIO_Pin_15);				
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;			   
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 	
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //IO口速度为50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);			    
// GPIO_SetBits(GPIOB,GPIO_Pin_3);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;			   
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 	
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //IO口速度为50MHz
  GPIO_Init(GPIOC, &GPIO_InitStructure);			    
 // GPIO_ResetBits(GPIOC,GPIO_Pin_14 | GPIO_Pin_15);		
}

