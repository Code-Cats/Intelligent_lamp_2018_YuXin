#include "stm32f10x.h"
#include "timer.h"
#include "led.h"
#include "pwm.h"
#include "base_deal.h"
#include "switch.h"
#include "delay.h"
#include "usart.h"	
#include "myiic.h"
#include "TSL2561.h"
#include "GY_53_deal.h"

#include "usart2.h"	 

#include "math.h"


#include "light_dimmer.h"
#include "touch.h"

extern LightDimmerTypeDef LightDimmer;
extern LEDControlTypeDef LED_Control;

u16 TSL2561_Calculation_my(u16 ch0,u16 ch1);

void SFE_TSL2561_read_data0_and_data1(u8 command,u16 *ch1,u16 *ch2);
/************************************************
STM32F103C8T6小绿板
************************************************/
///////////////////
u8 test1=0xA5;
u8 test2=0xAE;
u8 test_sum=0;

///////////////////
s8 test_abs_in=-4;
s8 test_result=0;
//////////////////

u16 TIM_R=0;

 u8 test_pa[2]={0};
 
 u16 Chanel0_Val,Chanel1_Val=0;
 
// u16 pwm_tem=500;
 int main(void)
 {	
	 LED_Control.state=POWER_OFF;
	 LED_Control.dimmer.touchDimmer_last=TOUCHING;
	delay_init();
	delay_ms(200);
	//AFIO->MAPR|=AFIO_MAPR_SWJ_CFG_JTAGDISABLE;	//	禁用JTAG，只启用SWD方式调试,貌似这条语句没有起到该有的作用
	uart_init(14400);
//	USART2_Init(9600);
	LED_Init();
	 Touch_GPIO_Init();
//	Switch_Init();
  TIM3_PWM_Init(ESC_CYCLE-1,72-1);	//ESC_CYCLE=0.5ms  2000hz>1250hz
	TIM1_Int_Init(200-1,720-1);	//	72MHZ频率，分频720，100khz计数频率，计数100次为1ms,计数200次为2ms
//	TIM1_PWM_Init(ESC_CYCLE-1,720-1);      
//  Switch_Init();
	 IIC_Init();	//PB6 PB7
	 delay_ms(200);
	 TSL2561_Init(TIMING_402MS_16X);	//TIMING_402MS  TIMING_101MS
	
//	 GY_53_CMD(0x53);	//一般测量模式
//	 
//	 GY_53_CMD(0x25);	//保存参数
//	 
//	 GY_53_CMD(0xAF);	//波特率115200
	 
	 USART2_Init(115200);
	 
//	 GY_53_CMD(0x25);	//保存参数
  while(1)
	{
		
		test_result=ABS(test_abs_in);
		test_sum=test1+test2;
////		USART_SendData(USART1,10);
//GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//调用GPIO重映射函数		
//		PWM_PB4_Set(pwm_tem);
//		PWM_PB5_Set(pwm_tem);
		
		
//		TIM1_1=100;
//		TIM1_2=100;
//		Chanel0_Val=TSL2561_Chanel0Read();
//		Chanel1_Val=TSL2561_Chanel1Read();
		SFE_TSL2561_read_data0_and_data1(TSL2561_DATA0_LOW,&Chanel0_Val,&Chanel1_Val);
		
		LED_Control.Lux.rawdata=TSL2561_Calculation_my(Chanel0_Val,Chanel1_Val);
		
//		TIM3_1=(u16)(5*(100-LightDimmer.RatioData.ratio_filter));
//		TIM3_2=(u16)(5*(100-LightDimmer.RatioData.ratio_filter));
		  
//		PA9=test_pa[0];
//		PA10=test_pa[1];
		
		LED=1;
		delay_ms(200);
		LED=0;
		
//		TIM_R=TSL2561_Read(TSL2561_TIMING);
		
		delay_ms(202);
	}
 }
 
 
 
 //一次把两个寄存器4个字节全部读取出来  测试一次只发data1_low指令读4个字节能否全部读出来
void SFE_TSL2561_read_data0_and_data1(u8 command,u16 *ch1,u16 *ch2)
{
//  u16 date;
	IIC_Start();
	IIC_Send_Byte(TSL2561_ADDR<<1);
	IIC_Wait_Ack();
	IIC_Send_Byte(command);
	IIC_Wait_Ack();
	
	IIC_Start();
	IIC_Send_Byte((TSL2561_ADDR<<1)+1);
	IIC_Wait_Ack();
	*ch1=IIC_Read_Byte(1);
	*ch1+=(IIC_Read_Byte(1)<<8);	//读取了两个字节？？？？？？？？？？？
	*ch2=IIC_Read_Byte(1);
	*ch2+=(IIC_Read_Byte(1)<<8);	//读取了两个字节？？？？？？？？？？？
	IIC_Stop();
//	return date;  
}

/*
 u16 TSL2561_Read(u8 command)
{
	u16 date;
	IIC_Start();
	IIC_Send_Byte(TSL2561_ADDR<<1);
	IIC_Wait_Ack();
	IIC_Send_Byte(command);
	IIC_Wait_Ack();
	
	IIC_Start();
	IIC_Send_Byte((TSL2561_ADDR<<1)+1);
	IIC_Wait_Ack();
	date=IIC_Read_Byte(1);
	date+=(IIC_Read_Byte(1)<<8);	//读取了两个字节？？？？？？？？？？？
	IIC_Stop();
	return date;
}
*/

u16 TSL2561_Calculation_my(u16 ch0,u16 ch1)
{
	float ch12_pro=(float)ch1/(float)ch0;
	u16 Lux_out=0;
	if(ch12_pro<=0.52f)
	{
		Lux_out=(u16)(0.0315f*ch0-0.0593f*ch0*pow(Lux_out,1.4f));
	}
	else if(ch12_pro<=0.65f)
	{
		Lux_out=(u16)(0.0229f*ch0-0.0291f*ch1);
	}
	else if(ch12_pro<=0.80f)
	{
		Lux_out=(u16)(0.0157f*ch0-0.0180f*ch1);
	}
	else if(ch12_pro<=1.30f)
	{
		Lux_out=(u16)(0.00338f*ch0-0.00260f*ch1);
	}
	else if(ch12_pro>1.30f)
	{
		Lux_out=0;
	}
	else
	{
		//应该不可能有这种情况
		return 0;
	}
	return Lux_out;
}

