#ifndef PWM_H
#define PWM_H
#include "sys.h"

#define ESC_CYCLE 500

#define TIM3_1 TIM3->CCR1
#define TIM3_2 TIM3->CCR2

#define TIM1_1 TIM1->CCR1
#define TIM1_2 TIM1->CCR2


//#define PA9 PAout(9)
//#define PA10 PAout(10)

void TIM3_PWM_Init(u16 arr,u16 psc);
void TIM1_PWM_Init(u16 arr,u16 psc);

#endif
