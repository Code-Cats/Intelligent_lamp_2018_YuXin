#include "pwm.h"

//TIM3 PWM部分初始化 通道1-2，PB4-5
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM3_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//使能定时器3时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟
	
//	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); //Timer3部分重映射  查数据手册：TIM3_CH1,2,3,4->PB4,5,0,1   无映射：PA6 PA7 PB0 PB1

 
   //设置该引脚为复用输出功能,输出TIM3 CH2的PWM脉冲波形	GPIOA.7
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO
	
 
   //初始化TIM3
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM3 Channel2 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC1
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC2

	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR1上的预装载寄存器
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR2上的预装载寄存器
 
	TIM_Cmd(TIM3, ENABLE);  //使能TIM3
	

}

//TIM1 PWM部分初始化 通道1-2，PA8-9 无重映射
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM1_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);	//使能定时器3时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟
	
//	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); //Timer3部分重映射  查数据手册：TIM3_CH1,2,3,4->PB4,5,0,1   无映射：PA6 PA7 PB0 PB1

 
   //设置该引脚为复用输出功能,输出TIM3 CH2的PWM脉冲波形	GPIOA.8-9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO
	
//////	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //TIM_CH2	//串口1
//////	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //复用推挽输出
//////	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//////	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO
//////	
//////	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //TIM_CH2
//////	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //复用推挽输出
//////	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//////	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO
	
 
   //初始化TIM1  定时器里初始化过
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	
	
	
	//初始化TIM3 Channel2 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输0出极性:TIM输出比较极性高
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC1
//	TIM_OC2Init(TIM1, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC2
//	TIM_OC3Init(TIM1, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC2

	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //使能TIM3在CCR1上的预装载寄存器
//	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);  //使能TIM3在CCR2上的预装载寄存器
//	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);  //使能TIM3在CCR2上的预装载寄存器
 
 TIM_CtrlPWMOutputs(TIM1, ENABLE);                             //pwm输出
	TIM_Cmd(TIM1, ENABLE);  //使能TIM3
	
	
	/*
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;//PWM输出使能
					 //TIM_OCInitStructure.TIM_OutputState=TIM_OutputNState_Disable;  //PWM输出使能
	TIM_OCInitStructure.TIM_Pulse=10;
					 //TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;
					// TIM_OCInitStructure.TIM_OCNPolarity=TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState=TIM_OCIdleState_Set;
	//TIM_OCInitStructure.TIM_OCNIdleState=TIM_OCNIdleState_Reset;
	TIM_OC1Init(TIM1,&TIM_OCInitStructure);


TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;//PWM输出使能
					 //TIM_OCInitStructure.TIM_OutputState=TIM_OutputNState_Disable;  //PWM输出使能
	TIM_OCInitStructure.TIM_Pulse=10;
					 //TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;
					// TIM_OCInitStructure.TIM_OCNPolarity=TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState=TIM_OCIdleState_Set;
	//TIM_OCInitStructure.TIM_OCNIdleState=TIM_OCNIdleState_Reset;
	TIM_OC2Init(TIM1,&TIM_OCInitStructure);


	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse=10;
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState=TIM_OCIdleState_Set;
	TIM_OC2Init(TIM1,&TIM_OCInitStructure);

	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);

	TIM_Cmd(TIM1,ENABLE);//ê1?ü?ò??ê§?üTIMxíaéè

	//下面这句是高级定时器才有，输出PWM必须打开

	TIM_CtrlPWMOutputs(TIM1, ENABLE);                             //pwm输出
	*/
	
	

}
