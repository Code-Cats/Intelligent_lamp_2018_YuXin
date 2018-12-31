#include "TSL2561.h"
#include "myiic.h"

/*
//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	

*/

/*
���ܣ�д���������
������command����λΪ����λ����λΪҪд�����ݵĵ�ַ��dateҪд�������
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
���ܣ���ȡһ���ֽڵ�����
������command����λΪ�������λΪҪ�������ݵ�ַ��ַ
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
	date+=(IIC_Read_Byte(1)<<8);	//��ȡ�������ֽڣ���������������������
	IIC_Stop();
	return date;
}

/*
���ܣ�����TLSģ��
*/
void TSL2561_PowerOn(void)
{
	TSL2561_Write(TSL2561_CMD,TSL2561_POWER_ON);
}
/*
���ܣ��ر�TSLģ��
*/
void TSL2561_PowerDown(void)
{
	TSL2561_Write(TSL2561_CMD,TSL2561_POWER_DOWN);
}

/*
���ܣ����û���ʱ��
������TIMING_13MS��TIMING_101MS��TIMING_402MS
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
���ܣ���ʼ��TSL2561ģ��
������
*/
void TSL2561_Init(u8 Time_model)
{
	TSL2561_PowerOn();
	
	TSL2561_TimingSet(Time_model);
}

/*
���ܣ���ȡ�����ֽ�chanel0
������CHANEL
*/
u16 TSL2561_Chanel0Read(void)
{
    u16 buff;
	buff=TSL2561_Read(TSL2561_DATA0_LOW);
	return buff;
}
/*
���ܣ���ȡchanel1����
��������
*/
u16 TSL2561_Chanel1Read(void)
{
    u16 buff;
	buff=TSL2561_Read(TSL2561_DATA0_LOW);
	return buff;
}


