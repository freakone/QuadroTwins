#include "cmsis_boot/stm32f10x.h"

void motors_init()
{

	TIM4->PSC = 0x10;
	TIM4->ARR = 0xfa0;
	TIM4->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1;
	TIM4->CCMR2 = TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1;
	TIM4->CCER = TIM_CCER_CC1E | TIM_CCER_CC2E  | TIM_CCER_CC3E | TIM_CCER_CC4E;
	TIM4->CR1 = TIM_CR1_CEN;


	motor_set(1, 0);
	motor_set(2, 0);
	motor_set(3, 0);
	motor_set(4, 0);

}


void motor_set(int i, unsigned int val)
{
	if(val > 2815)
		return;

	switch(i)
	{
	case 1:
		TIM4->CCR1 = val;
		return;
	case 2:
		TIM4->CCR2 = val;
		return;
	case 3:
		TIM4->CCR3 = val;
		return;
	case 4:
		TIM4->CCR4 = val;
		return;


	}

}
