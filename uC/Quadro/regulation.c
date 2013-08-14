#include "cmsis_boot/stm32f10x.h"
#include "sensors.h"
#include "usart.h"

void regulation_init()
{
	TIM1->PSC = 7199;
	TIM1->ARR = 100;
	TIM1->DIER = TIM_DIER_UIE;
	TIM1->CR1 = TIM_CR1_CEN;

	NVIC_EnableIRQ(TIM1_UP_IRQn);
}

float anglex = 0;
void TIM1_UP_IRQHandler()
{
	if(TIM1->SR & TIM_SR_UIF)
	{
		sensors_read();

		anglex += ((float)gyro.x * gyro.scale) / 100;

		//usart_puti(anglex);
		//usart_puts("\r\n");
		//debug_data();

		TIM1->SR &= ~TIM_SR_UIF;
	}

}

float filterAngle;

float comp_filter(float newAngle, float newRate, float dt) {

	float filterTerm0;
	float filterTerm1;
	float filterTerm2;
	float timeConstant;

	timeConstant=0.5; // default 1.0

	filterTerm0 = (newAngle - filterAngle) * timeConstant * timeConstant;
	filterTerm2 += filterTerm0 * dt;
	filterTerm1 = filterTerm2 + ((newAngle - filterAngle) * 2 * timeConstant) + newRate;
	filterAngle = (filterTerm1 * dt) + filterAngle;

	return filterAngle; // This is actually the current angle, but is stored for the next iteration
}

