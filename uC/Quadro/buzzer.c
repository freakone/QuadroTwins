#include "cmsis_boot/stm32f10x.h"

void buzzer_init()
{
	TIM3->PSC = 120;
	TIM3->ARR = 1000;
	TIM3->CCR4 = 0;
	TIM3->CCMR2 = TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1;
	TIM3->CCER = TIM_CCER_CC4E;
	TIM3->DIER = TIM_DIER_UIE;
	TIM3->CR1  = TIM_CR1_CEN;

	NVIC_EnableIRQ(TIM3_IRQn);
}

int t_time = 0;
int t_mode = 0;
int t_counter = 0;
int t_counter2 = 0;
int t_ccr = 0;
int t_clicks = 0;
void TIM3_IRQHandler(void)
{
	if(TIM3->SR & TIM_SR_UIF)
    {
	  TIM3->SR &= ~TIM_SR_UIF;

	  if(t_mode == 2)
	  {
		  if(t_counter2 > (t_time * 300 / t_clicks))
		  {
			  TIM3->CCR4 = TIM3->CCR4 == 0 ? t_ccr : 0;
			  t_counter2 = 0;
		  }

		  t_counter2++;
	  }

	  if(t_time*300  < t_counter)
	  {
		  TIM3->CCR4 = 0;
		  TIM3->CR1 &= ~TIM_CR1_CEN;
		  return;
	  }

	  t_counter++;

    }

}

void play_buzz(int high, int mode, int time, int clicks)
{
	uint32_t APBFreq = 72000000;
	uint32_t TimerFrequency = 800000;
	uint32_t PWMFrequency = high;

	t_mode = mode;
	t_time = time;
	t_counter = 0;
	t_clicks = clicks * 2;
	t_counter2 = 0;
	if(mode == 0)
	{
		TIM3->CCR4 = 0;
		TIM3->CR1 &= ~TIM_CR1_CEN;
		return;
	}

	TIM3->PSC =   APBFreq/TimerFrequency - 1;
	TIM3->ARR =   TimerFrequency/PWMFrequency - 1;
	TIM3->CCR4 = (TimerFrequency/PWMFrequency - 1)/2;
	t_ccr = TIM3->CCR4;
	TIM3->CR1 |= TIM_CR1_CEN;
}

void wait_buzz()
{
	while(TIM3->CR1 & TIM_CR1_CEN);
}
