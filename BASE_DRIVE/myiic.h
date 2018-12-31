#ifndef __MYIIC_H_
#define __MYIIC_H_

#include "sys.h"

#define IIC1_SCL_Pin  GPIO_Pin_6	//SCL
#define IIC1_SDA_Pin  GPIO_Pin_7	//SDA
#define GPIO_IIC1     GPIOB


//IO��������
void IIC1_SDA_IN(void);
void IIC1_SDA_OUT(void);


//IO��������	 
#define IIC1_SCL    PBout(6) //SCL
#define IIC1_SDA    PBout(7) //SDA	 
#define READ_IIC1_SDA   PBin(7)  //����SDA 


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

#endif
