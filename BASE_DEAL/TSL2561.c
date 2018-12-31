#include "TSL2561.h"
#include "myiic.h"

/*
//IIC所有操作函数
void IIC_Init(void);                //初始化IIC的IO口				 
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	

*/

/*
功能：写命令和数据
参数：command高四位为控制位低四位为要写入数据的地址，date要写入的数据
*/
void TSL2561_Write(u8 command,u8 date)
{
	IIC_Start();
	IIC_Send_Byte(TSL2561_ADDR<<1);
	IIC_Wait_Ack();
	IIC_Send_Byte(command);
	IIC_Wait_Ack();
	IIC_Send_Byte(date);
	IIC_Wait_Ack();
	IIC_Stop();
}

/*
功能：读取一个字节的数据
参数：command高四位为命令，低四位为要读的数据地址地址
*/
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

/*
功能：启动TLS模块
*/
void TSL2561_PowerOn(void)
{
	TSL2561_Write(TSL2561_CMD,TSL2561_POWER_ON);
}
/*
功能：关闭TSL模块
*/
void TSL2561_PowerDown(void)
{
	TSL2561_Write(TSL2561_CMD,TSL2561_POWER_DOWN);
}

/*
功能：设置积分时间
参数：TIMING_13MS、TIMING_101MS、TIMING_402MS
*/
void TSL2561_TimingSet(u8 TIME)
{
	IIC_Start();
	IIC_Send_Byte(TSL2561_ADDR<<1);
	IIC_Wait_Ack();
	IIC_Send_Byte(TSL2561_TIMING);
	IIC_Wait_Ack();
	IIC_Send_Byte(TIME);
	IIC_Wait_Ack();
	IIC_Stop();
}

/*
功能：初始化TSL2561模块
参数：
*/
void TSL2561_Init(u8 Time_model)
{
	TSL2561_PowerOn();
	
	TSL2561_TimingSet(Time_model);
}

/*
功能：读取整个字节chanel0
参数：CHANEL
*/
u16 TSL2561_Chanel0Read(void)
{
    u16 buff;
	buff=TSL2561_Read(TSL2561_DATA0_LOW);
	return buff;
}
/*
功能：读取chanel1数据
参数：无
*/
u16 TSL2561_Chanel1Read(void)
{
    u16 buff;
	buff=TSL2561_Read(TSL2561_DATA0_LOW);
	return buff;
}


