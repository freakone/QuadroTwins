#include "cmsis_boot/stm32f10x.h"
#include "hdr_bitband.h"

#define L1 GPIOA->ODR ^= GPIO_ODR_ODR1;
#define L2 GPIOB->ODR ^= GPIO_ODR_ODR12;
#define L3 GPIOC->ODR ^= GPIO_ODR_ODR1;
#define L4 GPIOC->ODR ^= GPIO_ODR_ODR2;
#define LR GPIOC->ODR ^= GPIO_ODR_ODR3;

void leds_init()
{
	TIM2->PSC = 140;
	TIM2->ARR = 65535;
	TIM2->DIER = TIM_DIER_UIE;
	TIM2->CR1 = TIM_CR1_CEN;

	NVIC_EnableIRQ(TIM2_IRQn); // Enable interrupt from TIM3 (NVIC level)
	//L2;
	//L4;


}


uint32_t* leds[]= {BITBAND(&GPIOA->ODR, 1), BITBAND(&GPIOB->ODR, 12),BITBAND(&GPIOC->ODR, 1),BITBAND(&GPIOC->ODR, 2)};
int i = 0;
void TIM2_IRQHandler(void)
{
	if(TIM2->SR & TIM_SR_UIF)
    {
	  TIM2->SR &= ~TIM_SR_UIF;
	  LR;
	  *leds[i] = 1;

	  if(i > 0)
		  *leds[i-1] = 0;
	  else
		  *leds[3] = 0;

	  if(i == 3)
		  i = 0;
	  else
		  i++;
    }
}
