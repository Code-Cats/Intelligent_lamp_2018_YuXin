#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

extern u32 time_2ms_count;

void TIM1_Int_Init(u16 arr,u16 psc);
//void TIM3_Int_Init(u16 arr,u16 psc);
//void TIM3_Int_Fix(u16 arr,u16 psc);
#endif
