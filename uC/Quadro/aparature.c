#include "cmsis_boot/stm32f10x.h"
#include "aparature.h"

int offset = 0x43F;
int prevstate[8];
void aparature_init()
{
		AFIO->EXTICR[3] = AFIO_EXTICR4_EXTI15_PB;

		EXTI->IMR = EXTI_IMR_MR15;
		EXTI->FTSR = EXTI_FTSR_TR15;
		EXTI->RTSR = EXTI_RTSR_TR15;

		NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void EXTI15_10_IRQHandler(void)
{
	int val = 0;

		if(EXTI->PR & EXTI_PR_PR15) //PB15
		{
			if(GPIOB->IDR & GPIO_IDR_IDR15)
				prevstate[0] = TIM2->CNT;
			else if(prevstate[0] > 0)
			{
				val = TIM2->CNT;

				if(val < prevstate[0])
					val += 65535;

				pilot.throttle_up_down = val - prevstate[0] - offset;

			}

			EXTI->PR |= EXTI_PR_PR15;
		 }
}
