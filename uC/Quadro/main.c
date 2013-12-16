#include "cmsis_boot/stm32f10x.h"
#include "leds.h"
#include "motors.h"
#include "sensors.h"
#include "lipo.h"
#include "buzzer.h"
#include "regulation.h"
#include "aparature.h"


#define BTN (GPIOC->IDR & GPIO_IDR_IDR13)

int main(void)
{
	int i;
	for(i = 0; i < 500000;i++); //delay to ensure sensors poweron


	clocks_init();
	leds_init();
	gpio_init();
	motors_init();
	do{	sensors_error_flag = 0;
		sensors_init(); }
	while(sensors_error_flag);

	buzzer_init();
	lipo_init();
	usart_init();
	aparature_init();
	regulation_init();

	play_buzz(1000, 2, 1, 2);
	wait_buzz();
	play_buzz(500, 2, 1, 3);
	wait_buzz();

	usart_puts("Quadro started!\r\n");


	while(1)
	{
		regulation_loop();
	}
}

void clocks_init()
{
	//HSE i zegar na 72MHz
	RCC->CR = RCC_CR_HSEON;
	while(!(RCC->CR & RCC_CR_HSERDY)){};
	FLASH->ACR |= FLASH_ACR_LATENCY_2;
	RCC->CR |= RCC_CR_CSSON;
	RCC->CFGR = (RCC_CFGR_PLLMULL9 | RCC_CFGR_ADCPRE_DIV8 |  RCC_CFGR_PPRE1_DIV2 | RCC_CFGR_PLLSRC_HSE);
	RCC->CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY)){};
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while(!(RCC->CFGR & RCC_CFGR_SWS_PLL));

	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN | RCC_APB1ENR_I2C2EN | RCC_APB1ENR_TIM4EN | RCC_APB1ENR_USART2EN | RCC_APB1ENR_TIM2EN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN | RCC_APB2ENR_ADC1EN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_SPI1EN | RCC_APB2ENR_TIM1EN | RCC_APB2ENR_AFIOEN;
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
}

void gpio_init()
{
	GPIOA->CRL &= ~(GPIO_CRL_CNF1 | GPIO_CRL_CNF4 | GPIO_CRL_CNF2 | GPIO_CRL_CNF3 |  GPIO_CRL_CNF5  | GPIO_CRL_CNF6 | GPIO_CRL_CNF7);
	GPIOA->CRL |= GPIO_CRL_MODE1 | GPIO_CRL_MODE2 | GPIO_CRL_CNF2_1 | GPIO_CRL_CNF3_1 | GPIO_CRL_MODE4 |
			GPIO_CRL_MODE5 | GPIO_CRL_CNF5_1 | GPIO_CRL_MODE6 | GPIO_CRL_CNF6_1 | GPIO_CRL_MODE7 | GPIO_CRL_CNF7_1;


	GPIOB->CRL &= ~(GPIO_CRL_CNF1 | GPIO_CRL_CNF6 | GPIO_CRL_CNF7  | GPIO_CRL_CNF0);
	GPIOB->CRL |= GPIO_CRL_MODE1 | GPIO_CRL_CNF1_1 | GPIO_CRL_MODE6 | GPIO_CRL_CNF6_1 | GPIO_CRL_MODE7 | GPIO_CRL_CNF7_1;

	GPIOB->CRH &= ~(GPIO_CRH_CNF12 | GPIO_CRH_CNF13 | GPIO_CRH_CNF14 | GPIO_CRH_CNF8 | GPIO_CRH_CNF9);
	GPIOB->CRH |= GPIO_CRH_MODE8 | GPIO_CRH_CNF8_1 | GPIO_CRH_MODE9 | GPIO_CRH_CNF9_1 | GPIO_CRH_MODE10 | GPIO_CRH_CNF10 | GPIO_CRH_MODE11 | GPIO_CRH_CNF11 |
			GPIO_CRH_MODE12 | GPIO_CRH_MODE13 | GPIO_CRH_MODE14;


	GPIOC->CRL |= GPIO_CRL_MODE3 | GPIO_CRL_MODE2 | GPIO_CRL_MODE1;
	GPIOC->CRL &= ~(GPIO_CRL_CNF3 | GPIO_CRL_CNF2 | GPIO_CRL_CNF1);

	GPIOC->CRH &= ~(GPIO_CRH_CNF13);
	GPIOC->CRH |= GPIO_CRH_CNF13_1;
	GPIOC->ODR |= GPIO_ODR_ODR13;
}
