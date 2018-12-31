#include "light_dimmer.h"
#include "GY_53_deal.h"
#include "pwm.h"
#include "touch.h"

//LED_ControlStateTypeDef LedControlState=POWER_OFF;	//LED����״̬ö��	��ע�� ���ϵ���LEDControl��
LightDimmerTypeDef LightDimmer;

LEDControlTypeDef LED_Control={0};

#define DISTURBANCE_THRESHOLD 26

extern u32 time_2ms_count;

//#define TOUCH_DIMMER	//����ѡ��
//#define TOUCH_ON_OFF	//����
//#define TOUCH_AUTO_OFF	//��ʱ����

/*
	POWER_OFF,	//��״̬
	OFF_BRIGHT,	//�ر�ʱ�����
	OFF_DARK,	//�ر�ʱ�����������
	AUTO_POWER_ON,	//���ݻ������Զ���
	WAIT_AUTO_OFF,	//�Զ��صƵ���ʱ����
	AUTO_POWER_OFF,	//�Զ��ع���̬
	ON_FIXED_LIGHT,	//�̶���ǿ״̬
	ON_DIMMING,	//����״̬
*/
#define AUTO_ON_BRIGHT_THRESHOLD 140
#define AUTO_ON_DARK_THRESHOLD 15
//�ӹص�״̬�л������ƣ����ݵ�ǰ�������Ŀ�����Ź���м��㣬�����ǿ�ǵ��ӵģ���ô�ⶨ�̶��˾���ĵ������ߣ��Ϳ����������Ҫ���ٹ�ǿ
#define DIMMER_DISMAX 250

#define YHE_BEST_LUX 350	//���������������
void LED_ControlTask(void)	//��ʱ����2ms
{
	
	switch(LED_Control.state)	//�����ÿ2ms����һ�Σ�����Ƶ�����ڲ������������
	{
		case POWER_OFF:
		{
			LED_Control.Out.Main_pwmTar=500;	//500��0ռ�ձ� Ϩ��
			
			if(TOUCH_ON_OFF==TOUCHING&&LED_Control.touchPower_last==DISTOUCH)	//ֱ�Ӱ����ؿ���
			{
				LED_Control.state=ON_FIXED_LIGHT;	//�л�������̬
				
				LightDimmer.RatioData.ratio=calc_ratioFormLux(LED_Control.Lux.rawdata,YHE_BEST_LUX);	//��������д��Ż�
				LightDimmer.RatioData.ratio_f=LightDimmer.RatioData.ratio;	//��ֹratio_f��ֵ�������Ӱ�죬ֱ��ͬ��
				LED_Control.Out.Main_pwmTar=(u16)(5*(100-LightDimmer.RatioData.ratio));
			}
			LED_Control.touchPower_last=TOUCH_ON_OFF;
			
			if(time_2ms_count%500==1)	//���������������1s
			{
				if(LED_Control.Lux.rawdata>AUTO_ON_BRIGHT_THRESHOLD)
				{
					LED_Control.Auto_ON.bright_count++;	//��ǿ����һ��ֵ������Ϊʹ���˴��ڹ���״̬��������ֹ����
				}
				else
				{
					LED_Control.Auto_ON.bright_count=0;
				}
				
				if(LED_Control.Auto_ON.bright_count>15)	//����15*1=15�����Ϊ������
				{
					LED_Control.Auto_ON.bright_count=0;	//�����Ա���һ��
					LED_Control.state=OFF_BRIGHT;	//�л����Զ�������̬
					for(int i=0;i<AUTO_ON_RECOED_NUM;i++)	//������¼����Ԥ����������ֹ֮������ʱ��С��һ������ѭ����ʱ��
					{
						LED_Control.Auto_ON.bright_record[i]=LED_Control.Lux.rawdata;
					}
				}
			}//���������������1s ����
			
			break;
		}
		case OFF_BRIGHT:	//�����״̬����Ϊ�Ѿ���һ���Ǹ��ŵĹ��������⣬�ȴ���ǿ���;��Զ��򿪣�ͨ��ѭ����¼�ý׶ι�ǿ����
		{	
			
			if(TOUCH_ON_OFF==TOUCHING&&LED_Control.touchPower_last==DISTOUCH)	//ֱ�Ӱ����ؿ���
			{
				LED_Control.state=ON_FIXED_LIGHT;	//�л�������̬
				
				LightDimmer.RatioData.ratio=calc_ratioFormLux(LED_Control.Lux.rawdata,YHE_BEST_LUX);	//��������д��Ż�
				LightDimmer.RatioData.ratio_f=LightDimmer.RatioData.ratio;	//��ֹratio_f��ֵ�������Ӱ�죬ֱ��ͬ��
				LED_Control.Out.Main_pwmTar=(u16)(5*(100-LightDimmer.RatioData.ratio));
			}
			LED_Control.touchPower_last=TOUCH_ON_OFF;
			
			if(time_2ms_count%200)//���������������404s
			{
				if(LED_Control.Lux.rawdata<AUTO_ON_DARK_THRESHOLD)
				{
					LED_Control.Auto_ON.dark_count++;	//��ǿС��һ��ֵ������Ϊʹ���˴Ӵ��ڹ���״̬���ڰ�״̬
				}
				else	//�������ģ�������¼
				{
					LED_Control.Auto_ON.dark_count=0;
					
					if(time_2ms_count%500*60)	//60s һ����
					{
						LED_Control.Auto_ON.bright_count++;
						if(LED_Control.Auto_ON.bright_count>=AUTO_ON_RECOED_NUM)
						{
							LED_Control.Auto_ON.bright_count=0;
						}
						LED_Control.Auto_ON.bright_record[LED_Control.Auto_ON.bright_count]=LED_Control.Lux.rawdata;	//һ��ѭ��40����
					}
				}
				
				if(LED_Control.Auto_ON.dark_count>2)	//��Լ1.3s������
				{
					LED_Control.Auto_ON.dark_count=0;	//�����Ա���һ��
					LED_Control.state=AUTO_POWER_ON;	//�л����Զ���������״̬����ȷ����
					
					LightDimmer.RatioData.ratio=calc_ratioFormLux(1,YHE_BEST_LUX);	//��д�ĺ���  calc_ratioFormLux
					LightDimmer.RatioData.ratio_f=LightDimmer.RatioData.ratio;	//��ֹratio_f��ֵ�������Ӱ�죬ֱ��ͬ��
					LED_Control.Out.Main_pwmTar=(u16)(5*(100-LightDimmer.RatioData.ratio));
					
					LED_Control.Auto_ON.wait_confirm_time=0;	//�Դ�ȷ��ʱ��������㣬��Ϊ����AUTO_ON״ֻ̬���ܴ�������ת������������ʹ��
				}
			}
			//LED_Control.Out.Main_pwmTar=500;	//500��0ռ�ձ� Ϩ�� ע�͸þ��ֹ�Ժ������Ӱ��
			break;
		}
		case OFF_DARK:
		{
			
			break;
		}
		case AUTO_POWER_ON:
		{
			LED_Control.Auto_ON.wait_confirm_time++;
			if(LED_Control.Auto_ON.wait_confirm_time<500*60)	//1min��
			{
				if(wait_AutoOn_ACK()==1)
				{
					LED_Control.Auto_ON.wait_confirm_time=0;	//�����Ա���һ��
					LED_Control.state=ON_FIXED_LIGHT;	//��ȷ���Զ��������л�������״̬
				}
			}
			else	//��ζ�ŵ���һ����Ҳû��ACK �ͽ��뵽Ԥ��Ϩ��״̬
			{
				LED_Control.Auto_ON.wait_confirm_time=0;	//�����Ա���һ��
				LED_Control.state=AUTO_POWER_OFF;	//�л������ر�״̬����ʾ״̬��
			}
			break;
		}
		case WAIT_AUTO_OFF:
		{
			LED_Control.Auto_OFF.wait_off_time++;
			if(LED_Control.Auto_OFF.wait_off_time<500*120)	//��120��֮�ڼ��ACK
			{
				if(wait_AutoOn_ACK()==1)
				{
					LED_Control.Auto_OFF.wait_off_time=0;	//�����Ա���һ��
					LED_Control.state=ON_FIXED_LIGHT;	//��ȷ���Զ��������л�������״̬
				}

			}
			else	//����120s��û��ACK����Ϩ�� 2���Ӷ�ʱ
			{
				LED_Control.Auto_OFF.wait_off_time=0;//�����Ա���һ��
				LED_Control.state=AUTO_POWER_OFF;	//������Ӧ���л���POWER_OFF״̬
			}
			break;
		}
		case AUTO_POWER_OFF:
		{
			static u8 flag=0;
			LED_Control.Auto_OFF.wait_off_confim_time++;
			if(LED_Control.Auto_OFF.wait_off_confim_time>500*16)	//14s���Ϩ�� ���3��Ϩ����Ϊ�˷�ֹ���ѭ���Զ���
			{
				LED_Control.Auto_OFF.wait_off_confim_time=0;//�����Ա���һ��
				LED_Control.state=POWER_OFF;	//������Ӧ���л���POWER_OFF״̬
			}
			if(wait_AutoOn_ACK()==1)	//������Ӧ
			{
				LED_Control.Auto_OFF.wait_off_confim_time=0;//�����Ա���һ��
				LED_Control.state=ON_FIXED_LIGHT;	//��ȷ���Զ��������л�������״̬
			}
			if(time_2ms_count%600==0)	//ÿ��2s�ͷ�ת״̬
			{
				flag=!flag;
				LED_Control.Out.Main_pwmTar=(u16)(5*(100-LightDimmer.RatioData.ratio*flag));
			}
			break;
		}
		case ON_FIXED_LIGHT:
		{
			LED_Control.Out.Main_pwmTar=(u16)(5*(100-LightDimmer.RatioData.ratio));	//��ֹ����״̬û�гɹ�����
			
			if(TOUCH_DIMMER==TOUCHING&&LED_Control.dimmer.touchDimmer_last==DISTOUCH)	//״̬�л���
			{
				//LED_Control.dimmer.enable=!LED_Control.dimmer.enable;
				LED_Control.state=ON_DIMMING;	//�л�������̬
			}
			LED_Control.dimmer.touchDimmer_last=TOUCH_DIMMER;
			
			if(TOUCH_ON_OFF==TOUCHING&&LED_Control.touchPower_last==DISTOUCH)	//ֱ�Ӱ����ؿ���
			{
				LED_Control.state=POWER_OFF;	//�л����ر�̬
			}
			LED_Control.touchPower_last=TOUCH_ON_OFF;
			/*
			��ʱ�رչ���
			*/
			if(TOUCH_AUTO_OFF==TOUCHING&&LED_Control.Auto_OFF.touchAoff_last==DISTOUCH)
			{
				LED_Control.state=WAIT_AUTO_OFF;	//�л����Զ��رյȴ�̬
			}
			LED_Control.Auto_OFF.touchAoff_last=TOUCH_AUTO_OFF;
			
			break;
		}
		case ON_DIMMING:
		{
			
			if(TOUCH_DIMMER==TOUCHING&&LED_Control.dimmer.touchDimmer_last==DISTOUCH)	//״̬�л���
			{
				//LED_Control.dimmer.enable=!LED_Control.dimmer.enable;
				LED_Control.state=ON_FIXED_LIGHT;	//�л�������̬
			}
			LED_Control.dimmer.touchDimmer_last=TOUCH_DIMMER;
			
			if(time_2ms_count%15==0&&LightDimmer.DisData.raw_validflag==1)
			{
				LED_dimmer(); 
			}
			
			if(LightDimmer.DisData.filterdata>=DIMMER_DISMAX)	//��������������
			{
				if(LED_Control.dimmer.delayOFF_count<0xFFFF)
				{
					LED_Control.dimmer.delayOFF_count++;
				}
				if(LED_Control.dimmer.delayOFF_count>=700)	//����700*2=1400ms���˳�Ϊ����״̬
				{
					LED_Control.state=ON_FIXED_LIGHT;	//�л�������̬
				}
			}
			else
			{
				LED_Control.dimmer.delayOFF_count=0;
			}
			
			/*
			�¼ӵİ����������� ��������
			*/
			if(TOUCH_AUTO_OFF==TOUCHING)
			{
				LED_Control.dimmer.delayOFF_count=0;	//����رյ������
				if(LightDimmer.Touch_AF.AF_down_count<0xFFFF)
				LightDimmer.Touch_AF.AF_down_count++;
			}
			else
			{
				LightDimmer.Touch_AF.AF_down_count=0;
			}
			
			if(TOUCH_ON_OFF==TOUCHING)
			{
				LED_Control.dimmer.delayOFF_count=0;	//����رյ������
				if(LightDimmer.Touch_AF.AF_up_count<0xFFFF)
				LightDimmer.Touch_AF.AF_up_count++;
			}
			else
			{
				LightDimmer.Touch_AF.AF_up_count=0;
			}
			
			if(LightDimmer.Touch_AF.AF_down_count>200)	//���������ʱ�����200*2=400ms
			{
				if(time_2ms_count%12==0)	//��Ȼ��ӵ�̫�� 2*12=24ms �ӵ�100��Ҫ2400ms ��2.4min
				{
					if(LightDimmer.RatioData.ratio>=1)
					{
						LightDimmer.RatioData.ratio--;
						LightDimmer.RatioData.ratio_f=LightDimmer.RatioData.ratio;	//��ֹratio_f��ֵ�������Ӱ�죬ֱ��ͬ��
					}
				}
			}
			else
			{
				
			}
			
			if(LightDimmer.Touch_AF.AF_up_count>200)	//���������ʱ�����200*2=400ms
			{
				if(time_2ms_count%12==0)	//��Ȼ��ӵ�̫�� 2*12=24ms �ӵ�100��Ҫ2400ms ��2.4min
				{
					if(LightDimmer.RatioData.ratio<=99)
					{
						LightDimmer.RatioData.ratio++;
						LightDimmer.RatioData.ratio_f=LightDimmer.RatioData.ratio;	//��ֹratio_f��ֵ�������Ӱ�죬ֱ��ͬ��
					}
				}
			}
			else
			{
				
			}
			
			
//			TIM3_1=(u16)(5*(100-LightDimmer.RatioData.ratio));	//�ڴ������Ż�����ֱ�ӽ�Ŀ��ֵ����pwm����������м侭��2������
//			TIM3_2=(u16)(5*(100-LightDimmer.RatioData.ratio));
			LED_Control.Out.Main_pwmTar=(u16)(5*(100-LightDimmer.RatioData.ratio));
			break;
		}
	}
	
	if(LED_Control.state!=ON_DIMMING)
	{
		LED_Control.dimmer.delayOFF_count=0;
	}
	if(LED_Control.state!=AUTO_POWER_OFF)	//�ǹرյƹ���ʾģʽ�£���������ʱ����������
	{
		LED_Control.Auto_OFF.wait_off_confim_time=0;
	}
	
	if(LED_Control.Out.Main_pwmDeal>LED_Control.Out.Main_pwmTar)
	{
		LED_Control.Out.Main_pwmDeal--;
	}
	else if(LED_Control.Out.Main_pwmDeal<LED_Control.Out.Main_pwmTar)
	{
		LED_Control.Out.Main_pwmDeal++;	//�ĳɸ��潥����
	}
//	LED_Control.Out.Main_pwmDeal=LED_Control.Out.Main_pwmTar;	//����ĳɸ��潥����
	TIM3_1=LED_Control.Out.Main_pwmDeal;	//
//	TIM3_2
	
}

u16 Averaging_u16(u16 data[],u16 num)	//��u16ƽ��ֵ
{
	u32 sum=0;
	u16 result=0;
	for(int i=0;i<num;i++)
	{
		sum+=data[i];
	}
	
	result=(u16)(sum/num);
	return result;
}

u16 calc_ratioFormLux(u16 lux_last,u16 lux_now)	//ͨ��������ǿ�����貹���ԴPWMռ�ձ�
{
	u16 pwm_out=30;
	if(lux_now>=lux_last+7)
	{
		pwm_out=(u16)((lux_now-lux_last)/6);
	}
	else
	{
		pwm_out=30;
	}
	
	pwm_out=pwm_out>100?100:pwm_out; 
	pwm_out=pwm_out<0?0:pwm_out;
	/*
	����������
	*/
	return pwm_out;	//��������ٲ�
}

//#define ACK_DIS 150
u8 wait_AutoOn_ACK(void)	//�ݶ��������
{
	static u32 last_use_time=0;	//��һ�ε������������ʱ��
	static u16 confirm_count=0;
	if(time_2ms_count-last_use_time>200)	//����400ms����Ϊ�������ط�����
	{
		confirm_count=0;
	}
	
	if(LightDimmer.DisData.filterdata<200)	//����С��һ��ֵ ��++
	{
		confirm_count++;
	}
	
	last_use_time=time_2ms_count;
	
	if(confirm_count>250)	//0.5s confirmʱ��
	{
		return 1;
	}
	else
	{
		return 0;
	}
	
}


void LED_dimmer(void)	//�����Ӻ���
{
	Dis_dimmer_Debounce(LightDimmer.DisData.filterdata);
	
	if(LightDimmer.Dis_diff.trigger_flag==0&&LightDimmer.DisData.filterdata<DIMMER_DISMAX)	//��û�ж�ratio���д���  ֻ��һ����Χ�ڵ���
	{
		LightDimmer.RatioData.ratio_f+=LightDimmer.Dis_diff.now_data/4.0f;
		
		LightDimmer.RatioData.ratio_f=LightDimmer.RatioData.ratio_f>100?100:LightDimmer.RatioData.ratio_f;
		LightDimmer.RatioData.ratio_f=LightDimmer.RatioData.ratio_f<0?0:LightDimmer.RatioData.ratio_f;
		
		LightDimmer.RatioData.ratio=(u16)LightDimmer.RatioData.ratio_f;
	}
	else
	{
		LightDimmer.RatioData.ratio_f=LightDimmer.RatioData.ratio;	//��ֹratio_f��ֵ�������Ӱ�죬ֱ��ͬ��
	}
	
//	LightDimmer.RatioData.ratio_filter=0.4f*LightDimmer.RatioData.ratio_filter+0.6f*LightDimmer.RatioData.ratio_f;
	
	LightDimmer.RatioData.record[LightDimmer.RatioData.recode_count]=LightDimmer.RatioData.ratio;	//
	LightDimmer.RatioData.recode_count++;
	if(LightDimmer.RatioData.recode_count>=RECODE_NUM)
	{
		LightDimmer.RatioData.recode_count=0;	//��ΧΪ0-RECODE_NUM-1
	}
}


//s16 dis_difference_value=0;


void GY_53_dis_diff(u16 dis)	//��һ�������������disΪ�ο�
{
//	if(time_2ms_count%20==0)
//	{
		LightDimmer.Dis_diff.now_data=dis-LightDimmer.DisData.lastdata;	//ԭʼ����
		
		LightDimmer.DisData.lastdata=dis;
//	}
}


u16 test_t=0;
//u16 pwm_duty_ratio_recode[RECODE_NUM]={0};	//��¼��ȥ20֡���ݣ��Ա���ʱ�ع�����
//u16 dis_diff_filter=0;
//#define DISTURBANCE_THRESHOLD 30//��λΪmm�������ֵ
#define TIME_RECALL 10	//��Ϊ��2ms��ʱ��  ʵ��ִ������15*2=30 �ع�ʱ��Ϊ30*10=300
void Dis_dimmer_Debounce(u16 dis)	//�������������������ú�������1ʱratio�ż�����¼
{	//���۲췢��ѡ����ٲ���ģʽ����±�죬diff��С����ֵ��С����Ϊ����һ���ͺ��˲������������Ҳ��С��
	//���Լ򵥼��㣬���������������ÿ���ƶ�30-40cm������30���㣬����30�����£�����40�ż�Ϊ���䣿��
	//��ô����30�㣬��1s  300mm����20ms  6mm,�����������ƶ�һ��20ms����ֻ�ƶ�6mm
//	GY_53_dis_diff();
	GY_53_dis_diff(dis);	//��ȡ�������һ�β����ı仯
	
	test_t=ABS(LightDimmer.Dis_diff.now_data);
	if(test_t>DISTURBANCE_THRESHOLD)	//�����ֵ̫������лع���ʧ��
	{
		
		s8 count_recall=LightDimmer.RatioData.recode_count-TIME_RECALL;
		
		LightDimmer.Dis_diff.recover_count=0;	//���� flag���0
		
		if(count_recall<0)
		{
			count_recall+=RECODE_NUM;	//Ųһ������
		}
		if(LightDimmer.Dis_diff.trigger_flag!=1)	//���ڻع�״̬�Żع�����Ȼ���ݻ���
		{
			LightDimmer.RatioData.ratio=LightDimmer.RatioData.record[count_recall];	//�ع�	//����Ļع� ����һ��������ǣ�ratio_fû�лع������ջ��ǻ���ratio��ֵ������
			LightDimmer.RatioData.ratio_f=LightDimmer.RatioData.ratio;
		}
	}
	
	if(LightDimmer.Dis_diff.recover_count<0xFFFF)
	{
		LightDimmer.Dis_diff.recover_count++;	//
	}
	
	if(LightDimmer.Dis_diff.recover_count<TIME_RECALL)
	{
		LightDimmer.Dis_diff.trigger_flag=1;
	}
	else
	{
		LightDimmer.Dis_diff.trigger_flag=0;
	}
	
	
}



