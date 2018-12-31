#include "myiic.h"
#include "delay.h"
//任意指定IIC引脚需要：首先宏定义哪组引脚 及哪两个GPIO口
//根据宏定义初始化
//根据宏定义设置引脚切换可配置输入输出
//根据宏定义配置输入输出宏

//#define IIC1_SCL_Pin  GPIO_Pin_6	//SCL
//#define IIC1_SDA_Pin  GPIO_Pin_7	//SDA
//#define GPIO_IIC1     GPIOB



void IIC_Init(void)	//除了时钟初始化都已做替换
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	//使能GPIOB时钟
	   
	GPIO_InitStructure.GPIO_Pin = IIC1_SCL_Pin|IIC1_SDA_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_IIC1, &GPIO_InitStructure);
	GPIO_SetBits(GPIO_IIC1,IIC1_SCL_Pin|IIC1_SDA_Pin); 	//PB6,PB7 输出高
}


void IIC1_SDA_IN(void)//IO方向设置
{
	GPIO_InitTypeDef GPIO_InitStructure;
//	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	//使能GPIOB时钟
	   
	GPIO_InitStructure.GPIO_Pin = IIC1_SDA_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_IIC1, &GPIO_InitStructure);
	GPIO_SetBits(GPIO_IIC1,IIC1_SDA_Pin); 	//PB6,PB7 输出高
}


void IIC1_SDA_OUT(void)//IO方向设置
{
	GPIO_InitTypeDef GPIO_InitStructure;
//	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	//使能GPIOB时钟
	   
	GPIO_InitStructure.GPIO_Pin = IIC1_SDA_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_IIC1, &GPIO_InitStructure);
	GPIO_SetBits(GPIO_IIC1,IIC1_SDA_Pin); 	//PB6,PB7 输出高
}


//产生IIC起始信号
void IIC_Start(void)
{
	IIC1_SDA_OUT();     //sda线输出
	IIC1_SDA=1;	  	  
	IIC1_SCL=1;
	delay_us(4);
 	IIC1_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC1_SCL=0;//钳住I2C总线，准备发送或接收数据 
}	  

//产生IIC停止信号
void IIC_Stop(void)
{
	IIC1_SDA_OUT();//sda线输出
	IIC1_SCL=0;
	IIC1_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC1_SCL=1; 
	IIC1_SDA=1;//发送I2C总线结束信号
	delay_us(4);							   	
}

//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	IIC1_SDA_IN();      //SDA设置为输入  
	IIC1_SDA=1;delay_us(1);	   
	IIC1_SCL=1;delay_us(1);	 
	while(READ_IIC1_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC1_SCL=0;//时钟输出0 	   
	return 0;  
} 

//产生ACK应答
void IIC_Ack(void)
{
	IIC1_SCL=0;
	IIC1_SDA_OUT();
	IIC1_SDA=0;
	delay_us(2);
	IIC1_SCL=1;
	delay_us(2);
	IIC1_SCL=0;
}

//不产生ACK应答		    
void IIC_NAck(void)
{
	IIC1_SCL=0;
	IIC1_SDA_OUT();
	IIC1_SDA=1;
	delay_us(2);
	IIC1_SCL=1;
	delay_us(2);
	IIC1_SCL=0;
}	

//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	IIC1_SDA_OUT(); 	    
    IIC1_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
			IIC1_SDA=1;
		else
			IIC1_SDA=0;
		txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		IIC1_SCL=1;
		delay_us(2); 
		IIC1_SCL=0;	
		delay_us(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	IIC1_SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        IIC1_SCL=0; 
        delay_us(2);
		IIC1_SCL=1;
        receive<<=1;
        if(READ_IIC1_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}
