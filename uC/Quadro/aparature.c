 #include "cmsis_boot/stm32f10x.h"
#include "aparature.h"

int offset = 0x231;
int min = 0;
int max = 400;

int prevstate[8];
void aparature_init()
{
	prevstate[0] = 0;

		AFIO->EXTICR[3] = AFIO_EXTICR4_EXTI15_PB;
		
		/*EXT 1 - przerwanie zewnętrzne do pilota */

		EXTI->IMR = EXTI_IMR_MR15;
		EXTI->FTSR = EXTI_FTSR_TR15;
		EXTI->RTSR = EXTI_RTSR_TR15;

		NVIC_EnableIRQ(EXTI15_10_IRQn);

		TIM1->PSC = 71;
		TIM1->ARR = 1;
		TIM1->DIER = TIM_DIER_UIE;
		TIM1->CR1 = TIM_CR1_CEN;

		NVIC_EnableIRQ(TIM1_UP_IRQn);
}

int counter = 0;
void TIM1_UP_IRQHandler()
{
	if(TIM1->SR & TIM_SR_UIF)
	{
		aparature_increment();
		if(GPIOB->IDR & GPIO_IDR_IDR15)
			counter = 0;
		else
			counter++;

		if(counter > 30000)
		{
			pilot.throttle_up_down = 0;
		}
		TIM1->SR &= ~TIM_SR_UIF;
	}

}

void aparature_increment()
{
	prevstate[0]++;
}

void EXTI15_10_IRQHandler(void)
{
	int val = 0;

		if(EXTI->PR & EXTI_PR_PR15) //PB15
		{
			if(GPIOB->IDR & GPIO_IDR_IDR15 && prevstate[0] > 0)
				prevstate[0] = 0;
			else
			{
				pilot.throttle_up_down = prevstate[0];
			}

			EXTI->PR |= EXTI_PR_PR15;
		 }
}
