#include "cmsis_boot/stm32f10x.h"
#include "sensors.h"
#include "regulation.h"

void usart_init()
{
	NVIC_SetPriority(USART2_IRQn, 26);
	USART2->BRR = 36000000 / 115200;
	USART2->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;

	NVIC_EnableIRQ(USART2_IRQn);

}

void usart_put(uint8_t ch)
{
	while(!(USART2->SR & USART_SR_TXE));
	USART2->DR = ch;
}

void usart_puts(uint8_t * str)
{
	while(*str != 0)
	{
		usart_put(*str);
		str++;
	}
}

void usart_puti(int i)
{
	char s[10];
	sprintf(s, "%d", i);
	usart_puts(s);
}

void debug_data()
{

	char str[100];
/*	sprintf(str, "magneto: %d %d %d \r\n", magneto.x, magneto.y, magneto.z);
	usart_puts(str);
	sprintf(str, "acc: %d %d %d \r\n", acc.x, acc.y, acc.z);
	usart_puts(str);
	/*sprintf(str, "gyro: %d %d %d \r\n", gyro.x, gyro.y, gyro.z);
	usart_puts(str);*/

	sprintf(str, "Power: %d P: %d I: %d D: %d Angle: %d \r\n", iPower, (int)(pid.p*100), (int)(pid.i*100), (int)(pid.d*100), (int)(x_angle*100));
	usart_puts(str);


}

void USART2_IRQHandler()
{
	if(USART2->SR & USART_SR_RXNE)
	{
		uint8_t data = USART2->DR;

		switch(data)
		{
		case 0x01:
			iPower+=100;
			break;
		case 0x02:
			iPower-=100;
			break;
		case 0x03:
			pid.p += 1;
			break;
		case 0x04:
			pid.p -= 1;
			break;
		case 0x05:
			pid.i += 0.01;
			break;
		case 0x06:
			pid.i -= 0.01;
			break;
		case 0x07:
			pid.d += 0.001;
			break;
		case 0x08:
			pid.d -= 0.001;
			break;
		case 0x09:
			TIM1->CR1 = TIM_CR1_CEN;
			break;
		case 0x10:
			TIM1->CR1 &= ~TIM_CR1_CEN;
			break;
		}

		debug_data();

	}

}
