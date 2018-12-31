#ifndef __TSL2561_H_
#define __TSL2561_H_
#include "sys.h"
//�������
#define TSL2561_ADDR0 0x29 // address with '0' shorted on board
#define TSL2561_ADDR  0x39 // default address
#define TSL2561_ADDR1 0x49 // address with '1' shorted on board

// TSL2561 registers

#define TSL2561_CMD           0x80
#define TSL2561_TIMING        0x81
#define	TSL2561_REG_ID        0x8A
#define	TSL2561_DATA0_LOW     0x8C
#define TSL2561_DATA0_HIGH    0x8D
#define	TSL2561_DATA1_LOW     0x8E
#define TSL2561_DATA1_HIGH    0x8F
//Control Register Value

#define TSL2561_POWER_ON      0x03
#define TSL2561_POWER_DOWN    0x00

//Timing Register Value.Set integration time
                                      //�����λ���û���ʱ��
#define  TIMING_13MS         0x00     //����ʱ��13.7����
#define  TIMING_101MS        0x01     //����ʱ��101����
#define  TIMING_402MS        0x02     //����ʱ��402����
#define  TIMING_402MS_16X		 0x12		//������402����

u16 TSL2561_Read(u8 command);
void TSL2561_PowerOn(void);
void TSL2561_PowerDown(void);
void TSL2561_TimingSet(u8 TIME);
void TSL2561_Init(u8 Time_model);
u16 TSL2561_Chanel0Read(void);
u16 TSL2561_Chanel1Read(void);


#endif
