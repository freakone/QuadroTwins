#include "cmsis_boot/stm32f10x.h"
#include "lipo.h"

void lipo_init()
{
	DMA1_Channel1->CNDTR = 2;
	DMA1_Channel1->CPAR = (uint32_t)&ADC1->DR;
	DMA1_Channel1->CMAR = (uint32_t)&ADCVal;
	DMA1_Channel1->CCR = DMA_CCR1_MINC  | DMA_CCR1_CIRC | DMA_CCR1_MSIZE_0 | DMA_CCR1_PSIZE_0;
	DMA1_Channel1->CCR |= DMA_CCR1_EN;

	ADC1->CR2 = ADC_CR2_ADON | ADC_CR2_CONT | ADC_CR2_TSVREFE | ADC_CR2_DMA;
	ADC1->SQR1 = ADC_SQR1_L_0;
	ADC1->SQR3 = 16 | (8 << 5);
	ADC1->SMPR2 |= ADC_SMPR2_SMP8;
	ADC1->SMPR1 |= ADC_SMPR1_SMP16;
	ADC1->CR1 = ADC_CR1_SCAN;
	ADC1->CR2 |= ADC_CR2_ADON;


}


