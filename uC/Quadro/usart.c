#include "cmsis_boot/stm32f10x.h"
#include "sensors.h"
void usart_init()
{
	USART2->BRR = 36000000 / 115200;
	USART2->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;

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

	char str[18];
/*	sprintf(str, "magneto: %d %d %d \r\n", magneto.x, magneto.y, magneto.z);
	usart_puts(str);*/
	sprintf(str, "acc: %d %d %d \r\n", acc.x, acc.y, acc.z);
	usart_puts(str);
	/*sprintf(str, "gyro: %d %d %d \r\n", gyro.x, gyro.y, gyro.z);
	usart_puts(str);*/
}
