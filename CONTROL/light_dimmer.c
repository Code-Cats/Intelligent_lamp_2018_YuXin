#include "light_dimmer.h"
#include "GY_53_deal.h"
#include "pwm.h"
#include "touch.h"

//LED_ControlStateTypeDef LedControlState=POWER_OFF;	//LED控制状态枚举	已注释 集合到了LEDControl里
LightDimmerTypeDef LightDimmer;

LEDControlTypeDef LED_Control={0};

#define DISTURBANCE_THRESHOLD 26

extern u32 time_2ms_count;

//#define TOUCH_DIMMER	//调光选择
//#define TOUCH_ON_OFF	//开关
//#define TOUCH_AUTO_OFF	//定时开关

/*
	POWER_OFF,	//关状态
	OFF_BRIGHT,	//关闭时外界亮
	OFF_DARK,	//关闭时外界由亮至暗
	AUTO_POWER_ON,	//根据环境光自动开
	WAIT_AUTO_OFF,	//自动关灯的延时过程
	AUTO_POWER_OFF,	//自动关过渡态
	ON_FIXED_LIGHT,	//固定光强状态
	ON_DIMMING,	//调光状态
*/
#define AUTO_ON_BRIGHT_THRESHOLD 140
#define AUTO_ON_DARK_THRESHOLD 15
//从关灯状态切换到开灯，根据当前反馈光和目标最优光进行计算，假设光强是叠加的，那么测定固定了距离的叠加曲线，就可以算出来需要多少光强
#define DIMMER_DISMAX 250

#define YHE_BEST_LUX 350	//人眼最佳照明亮度
void LED_ControlTask(void)	//定时周期2ms
{
	
	switch(LED_Control.state)	//该语句每2ms运行一次，其他频率在内部语句再做限制
	{
		case POWER_OFF:
		{
			LED_Control.Out.Main_pwmTar=500;	//500是0占空比 熄灭
			
			if(TOUCH_ON_OFF==TOUCHING&&LED_Control.touchPower_last==DISTOUCH)	//直接按开关开启
			{
				LED_Control.state=ON_FIXED_LIGHT;	//切换到定光态
				
				LightDimmer.RatioData.ratio=calc_ratioFormLux(LED_Control.Lux.rawdata,YHE_BEST_LUX);	//后面这个有待优化
				LightDimmer.RatioData.ratio_f=LightDimmer.RatioData.ratio;	//防止ratio_f的值对其造成影响，直接同化
				LED_Control.Out.Main_pwmTar=(u16)(5*(100-LightDimmer.RatioData.ratio));
			}
			LED_Control.touchPower_last=TOUCH_ON_OFF;
			
			if(time_2ms_count%500==1)	//这里的运行周期是1s
			{
				if(LED_Control.Lux.rawdata>AUTO_ON_BRIGHT_THRESHOLD)
				{
					LED_Control.Auto_ON.bright_count++;	//光强大于一定值，即认为使用人处于工作状态，计数防止误检测
				}
				else
				{
					LED_Control.Auto_ON.bright_count=0;
				}
				
				if(LED_Control.Auto_ON.bright_count>15)	//大于15*1=15秒就认为可以了
				{
					LED_Control.Auto_ON.bright_count=0;	//清零以便下一次
					LED_Control.state=OFF_BRIGHT;	//切换到自动开的亮态
					for(int i=0;i<AUTO_ON_RECOED_NUM;i++)	//将亮记录数组预先填满，防止之后亮的时间小于一个完整循环的时间
					{
						LED_Control.Auto_ON.bright_record[i]=LED_Control.Lux.rawdata;
					}
				}
			}//这里的运行周期是1s 结束
			
			break;
		}
		case OFF_BRIGHT:	//进入该状态即认为已经有一个非干扰的工作环境光，等待光强降低就自动打开，通过循环记录该阶段光强采样
		{	
			
			if(TOUCH_ON_OFF==TOUCHING&&LED_Control.touchPower_last==DISTOUCH)	//直接按开关开启
			{
				LED_Control.state=ON_FIXED_LIGHT;	//切换到定光态
				
				LightDimmer.RatioData.ratio=calc_ratioFormLux(LED_Control.Lux.rawdata,YHE_BEST_LUX);	//后面这个有待优化
				LightDimmer.RatioData.ratio_f=LightDimmer.RatioData.ratio;	//防止ratio_f的值对其造成影响，直接同化
				LED_Control.Out.Main_pwmTar=(u16)(5*(100-LightDimmer.RatioData.ratio));
			}
			LED_Control.touchPower_last=TOUCH_ON_OFF;
			
			if(time_2ms_count%200)//这里的运行周期是404s
			{
				if(LED_Control.Lux.rawdata<AUTO_ON_DARK_THRESHOLD)
				{
					LED_Control.Auto_ON.dark_count++;	//光强小于一定值，即认为使用人从处于工作状态到黑暗状态
				}
				else	//还是亮的，继续记录
				{
					LED_Control.Auto_ON.dark_count=0;
					
					if(time_2ms_count%500*60)	//60s 一分钟
					{
						LED_Control.Auto_ON.bright_count++;
						if(LED_Control.Auto_ON.bright_count>=AUTO_ON_RECOED_NUM)
						{
							LED_Control.Auto_ON.bright_count=0;
						}
						LED_Control.Auto_ON.bright_record[LED_Control.Auto_ON.bright_count]=LED_Control.Lux.rawdata;	//一个循环40分钟
					}
				}
				
				if(LED_Control.Auto_ON.dark_count>2)	//大约1.3s后亮起
				{
					LED_Control.Auto_ON.dark_count=0;	//清零以便下一次
					LED_Control.state=AUTO_POWER_ON;	//切换到自动开的亮起状态（待确定）
					
					LightDimmer.RatioData.ratio=calc_ratioFormLux(1,YHE_BEST_LUX);	//待写的函数  calc_ratioFormLux
					LightDimmer.RatioData.ratio_f=LightDimmer.RatioData.ratio;	//防止ratio_f的值对其造成影响，直接同化
					LED_Control.Out.Main_pwmTar=(u16)(5*(100-LightDimmer.RatioData.ratio));
					
					LED_Control.Auto_ON.wait_confirm_time=0;	//对待确认时间计数清零，因为进入AUTO_ON状态只可能从这里跳转，所以在这里使用
				}
			}
			//LED_Control.Out.Main_pwmTar=500;	//500是0占空比 熄灭 注释该句防止对后续造成影响
			break;
		}
		case OFF_DARK:
		{
			
			break;
		}
		case AUTO_POWER_ON:
		{
			LED_Control.Auto_ON.wait_confirm_time++;
			if(LED_Control.Auto_ON.wait_confirm_time<500*60)	//1min内
			{
				if(wait_AutoOn_ACK()==1)
				{
					LED_Control.Auto_ON.wait_confirm_time=0;	//清零以便下一次
					LED_Control.state=ON_FIXED_LIGHT;	//已确定自动开启，切换到定光状态
				}
			}
			else	//意味着到了一分钟也没有ACK 就进入到预备熄灭状态
			{
				LED_Control.Auto_ON.wait_confirm_time=0;	//清零以便下一次
				LED_Control.state=AUTO_POWER_OFF;	//切换到待关闭状态（提示状态）
			}
			break;
		}
		case WAIT_AUTO_OFF:
		{
			LED_Control.Auto_OFF.wait_off_time++;
			if(LED_Control.Auto_OFF.wait_off_time<500*120)	//在120秒之内检测ACK
			{
				if(wait_AutoOn_ACK()==1)
				{
					LED_Control.Auto_OFF.wait_off_time=0;	//清零以便下一次
					LED_Control.state=ON_FIXED_LIGHT;	//已确定自动开启，切换到定光状态
				}

			}
			else	//过了120s后还没有ACK，就熄灭 2分钟定时
			{
				LED_Control.Auto_OFF.wait_off_time=0;//清零以便下一次
				LED_Control.state=AUTO_POWER_OFF;	//无人响应，切换到POWER_OFF状态
			}
			break;
		}
		case AUTO_POWER_OFF:
		{
			static u8 flag=0;
			LED_Control.Auto_OFF.wait_off_confim_time++;
			if(LED_Control.Auto_OFF.wait_off_confim_time>500*16)	//14s后就熄灭 最后3秒熄灭是为了防止造成循环自动亮
			{
				LED_Control.Auto_OFF.wait_off_confim_time=0;//清零以便下一次
				LED_Control.state=POWER_OFF;	//无人响应，切换到POWER_OFF状态
			}
			if(wait_AutoOn_ACK()==1)	//有手响应
			{
				LED_Control.Auto_OFF.wait_off_confim_time=0;//清零以便下一次
				LED_Control.state=ON_FIXED_LIGHT;	//已确定自动开启，切换到定光状态
			}
			if(time_2ms_count%600==0)	//每过2s就翻转状态
			{
				flag=!flag;
				LED_Control.Out.Main_pwmTar=(u16)(5*(100-LightDimmer.RatioData.ratio*flag));
			}
			break;
		}
		case ON_FIXED_LIGHT:
		{
			LED_Control.Out.Main_pwmTar=(u16)(5*(100-LightDimmer.RatioData.ratio));	//防止其他状态没有成功置数
			
			if(TOUCH_DIMMER==TOUCHING&&LED_Control.dimmer.touchDimmer_last==DISTOUCH)	//状态切换的
			{
				//LED_Control.dimmer.enable=!LED_Control.dimmer.enable;
				LED_Control.state=ON_DIMMING;	//切换到调光态
			}
			LED_Control.dimmer.touchDimmer_last=TOUCH_DIMMER;
			
			if(TOUCH_ON_OFF==TOUCHING&&LED_Control.touchPower_last==DISTOUCH)	//直接按开关开启
			{
				LED_Control.state=POWER_OFF;	//切换到关闭态
			}
			LED_Control.touchPower_last=TOUCH_ON_OFF;
			/*
			定时关闭功能
			*/
			if(TOUCH_AUTO_OFF==TOUCHING&&LED_Control.Auto_OFF.touchAoff_last==DISTOUCH)
			{
				LED_Control.state=WAIT_AUTO_OFF;	//切换到自动关闭等待态
			}
			LED_Control.Auto_OFF.touchAoff_last=TOUCH_AUTO_OFF;
			
			break;
		}
		case ON_DIMMING:
		{
			
			if(TOUCH_DIMMER==TOUCHING&&LED_Control.dimmer.touchDimmer_last==DISTOUCH)	//状态切换的
			{
				//LED_Control.dimmer.enable=!LED_Control.dimmer.enable;
				LED_Control.state=ON_FIXED_LIGHT;	//切换到定光态
			}
			LED_Control.dimmer.touchDimmer_last=TOUCH_DIMMER;
			
			if(time_2ms_count%15==0&&LightDimmer.DisData.raw_validflag==1)
			{
				LED_dimmer(); 
			}
			
			if(LightDimmer.DisData.filterdata>=DIMMER_DISMAX)	//距离大于这个距离
			{
				if(LED_Control.dimmer.delayOFF_count<0xFFFF)
				{
					LED_Control.dimmer.delayOFF_count++;
				}
				if(LED_Control.dimmer.delayOFF_count>=700)	//超过700*2=1400ms就退出为定光状态
				{
					LED_Control.state=ON_FIXED_LIGHT;	//切换到定光态
				}
			}
			else
			{
				LED_Control.dimmer.delayOFF_count=0;
			}
			
			/*
			新加的按键长按调光 按键复用
			*/
			if(TOUCH_AUTO_OFF==TOUCHING)
			{
				LED_Control.dimmer.delayOFF_count=0;	//清零关闭调光计数
				if(LightDimmer.Touch_AF.AF_down_count<0xFFFF)
				LightDimmer.Touch_AF.AF_down_count++;
			}
			else
			{
				LightDimmer.Touch_AF.AF_down_count=0;
			}
			
			if(TOUCH_ON_OFF==TOUCHING)
			{
				LED_Control.dimmer.delayOFF_count=0;	//清零关闭调光计数
				if(LightDimmer.Touch_AF.AF_up_count<0xFFFF)
				LightDimmer.Touch_AF.AF_up_count++;
			}
			else
			{
				LightDimmer.Touch_AF.AF_up_count=0;
			}
			
			if(LightDimmer.Touch_AF.AF_down_count>200)	//如果触摸的时间大于200*2=400ms
			{
				if(time_2ms_count%12==0)	//不然会加的太快 2*12=24ms 加到100需要2400ms 即2.4min
				{
					if(LightDimmer.RatioData.ratio>=1)
					{
						LightDimmer.RatioData.ratio--;
						LightDimmer.RatioData.ratio_f=LightDimmer.RatioData.ratio;	//防止ratio_f的值对其造成影响，直接同化
					}
				}
			}
			else
			{
				
			}
			
			if(LightDimmer.Touch_AF.AF_up_count>200)	//如果触摸的时间大于200*2=400ms
			{
				if(time_2ms_count%12==0)	//不然会加的太快 2*12=24ms 加到100需要2400ms 即2.4min
				{
					if(LightDimmer.RatioData.ratio<=99)
					{
						LightDimmer.RatioData.ratio++;
						LightDimmer.RatioData.ratio_f=LightDimmer.RatioData.ratio;	//防止ratio_f的值对其造成影响，直接同化
					}
				}
			}
			else
			{
				
			}
			
			
//			TIM3_1=(u16)(5*(100-LightDimmer.RatioData.ratio));	//在此做了优化，不直接将目标值赋给pwm进行输出，中间经过2级处理
//			TIM3_2=(u16)(5*(100-LightDimmer.RatioData.ratio));
			LED_Control.Out.Main_pwmTar=(u16)(5*(100-LightDimmer.RatioData.ratio));
			break;
		}
	}
	
	if(LED_Control.state!=ON_DIMMING)
	{
		LED_Control.dimmer.delayOFF_count=0;
	}
	if(LED_Control.state!=AUTO_POWER_OFF)	//非关闭灯光提示模式下，都进行延时计数的清零
	{
		LED_Control.Auto_OFF.wait_off_confim_time=0;
	}
	
	if(LED_Control.Out.Main_pwmDeal>LED_Control.Out.Main_pwmTar)
	{
		LED_Control.Out.Main_pwmDeal--;
	}
	else if(LED_Control.Out.Main_pwmDeal<LED_Control.Out.Main_pwmTar)
	{
		LED_Control.Out.Main_pwmDeal++;	//改成跟随渐变型
	}
//	LED_Control.Out.Main_pwmDeal=LED_Control.Out.Main_pwmTar;	//后面改成跟随渐变型
	TIM3_1=LED_Control.Out.Main_pwmDeal;	//
//	TIM3_2
	
}

u16 Averaging_u16(u16 data[],u16 num)	//求u16平均值
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

u16 calc_ratioFormLux(u16 lux_last,u16 lux_now)	//通过两个光强计算需补充光源PWM占空比
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
	待补充内容
	*/
	return pwm_out;	//这里后面再补
}

//#define ACK_DIS 150
u8 wait_AutoOn_ACK(void)	//暂定这个名字
{
	static u32 last_use_time=0;	//上一次调用这个函数的时间
	static u16 confirm_count=0;
	if(time_2ms_count-last_use_time>200)	//大于400ms就认为是两个地方调用
	{
		confirm_count=0;
	}
	
	if(LightDimmer.DisData.filterdata<200)	//距离小于一定值 就++
	{
		confirm_count++;
	}
	
	last_use_time=time_2ms_count;
	
	if(confirm_count>250)	//0.5s confirm时间
	{
		return 1;
	}
	else
	{
		return 0;
	}
	
}


void LED_dimmer(void)	//调光子函数
{
	Dis_dimmer_Debounce(LightDimmer.DisData.filterdata);
	
	if(LightDimmer.Dis_diff.trigger_flag==0&&LightDimmer.DisData.filterdata<DIMMER_DISMAX)	//若没有对ratio进行处理  只在一定范围内调试
	{
		LightDimmer.RatioData.ratio_f+=LightDimmer.Dis_diff.now_data/4.0f;
		
		LightDimmer.RatioData.ratio_f=LightDimmer.RatioData.ratio_f>100?100:LightDimmer.RatioData.ratio_f;
		LightDimmer.RatioData.ratio_f=LightDimmer.RatioData.ratio_f<0?0:LightDimmer.RatioData.ratio_f;
		
		LightDimmer.RatioData.ratio=(u16)LightDimmer.RatioData.ratio_f;
	}
	else
	{
		LightDimmer.RatioData.ratio_f=LightDimmer.RatioData.ratio;	//防止ratio_f的值对其造成影响，直接同化
	}
	
//	LightDimmer.RatioData.ratio_filter=0.4f*LightDimmer.RatioData.ratio_filter+0.6f*LightDimmer.RatioData.ratio_f;
	
	LightDimmer.RatioData.record[LightDimmer.RatioData.recode_count]=LightDimmer.RatioData.ratio;	//
	LightDimmer.RatioData.recode_count++;
	if(LightDimmer.RatioData.recode_count>=RECODE_NUM)
	{
		LightDimmer.RatioData.recode_count=0;	//范围为0-RECODE_NUM-1
	}
}


//s16 dis_difference_value=0;


void GY_53_dis_diff(u16 dis)	//上一次数据以输入的dis为参考
{
//	if(time_2ms_count%20==0)
//	{
		LightDimmer.Dis_diff.now_data=dis-LightDimmer.DisData.lastdata;	//原始数据
		
		LightDimmer.DisData.lastdata=dis;
//	}
}


u16 test_t=0;
//u16 pwm_duty_ratio_recode[RECODE_NUM]={0};	//记录过去20帧数据，以便随时回滚数据
//u16 dis_diff_filter=0;
//#define DISTURBANCE_THRESHOLD 30//单位为mm跳变的阈值
#define TIME_RECALL 10	//因为是2ms定时器  实际执行周期15*2=30 回滚时间为30*10=300
void Dis_dimmer_Debounce(u16 dis)	//跳变消除函数，仅当该函数返回1时ratio才继续记录
{	//经观察发现选择快速测量模式后更新变快，diff变小，阈值变小。因为用了一阶滞后滤波，跳变的区别也变小了
	//可以简单计算，正常调光人手最多每秒移动30-40cm，按照30计算，超过30不更新（超过40才记为跳变？）
	//那么按照30算，即1s  300mm，即20ms  6mm,即正常人手移动一个20ms周期只移动6mm
//	GY_53_dis_diff();
	GY_53_dis_diff(dis);	//获取相对于上一次采样的变化
	
	test_t=ABS(LightDimmer.Dis_diff.now_data);
	if(test_t>DISTURBANCE_THRESHOLD)	//如果差值太大，则进行回滚和失能
	{
		
		s8 count_recall=LightDimmer.RatioData.recode_count-TIME_RECALL;
		
		LightDimmer.Dis_diff.recover_count=0;	//清零 flag变成0
		
		if(count_recall<0)
		{
			count_recall+=RECODE_NUM;	//挪一个周期
		}
		if(LightDimmer.Dis_diff.trigger_flag!=1)	//不在回滚状态才回滚，不然数据会乱
		{
			LightDimmer.RatioData.ratio=LightDimmer.RatioData.record[count_recall];	//回滚	//这里的回滚 存在一个问题就是，ratio_f没有回滚，最终还是会让ratio的值。。。
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



