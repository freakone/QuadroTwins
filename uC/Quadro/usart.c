#include "cmsis_boot/stm32f10x.h"
#include "sensors.h"
#include "regulation.h"
#include "lipo.h"

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


void dec2hascii(uint32_t liczba, uint8_t length){

	uint32_t buffer;
	int i;
	for (i=length;i > 0;i--)
	{
		buffer = (liczba % (1<< (i*4)) / (1<< ((i*4)-4)));
		if (buffer>=0 && buffer<=9)
			usart_put((uint8_t)buffer+48);
		else if (buffer>=10 && buffer<=15)
			usart_put((uint8_t)buffer+55);

	}

}

int32_t hascii2dec(int8_t* p, volatile int8_t len){

	volatile int32_t t = 0;

	volatile int i;
	for(i = len -1; i >= 0; i--)
	{
		if(*p>='0' && *p<='9'){
		t+=(int32_t)((*p-48)*(1<< (i*4)));
		}else if(*p>='A' && *p<='F'){
			t+=(int32_t)((*p-55)*(1<< (i*4)));
		}

		++p;
	}
		return t;

}

uint8_t buffer[50];
uint8_t buffer_pos = 0;
char str[100];

void readbuff(int8_t *p, int8_t *buff, int8_t len, int8_t start)
{
	int i ;

	for(i = 0; i < len; i++)
	{
		p[i] = buff[i+start];
	}

}

void buffer_parse()
{
	int8_t temp[5];
	int i;

	if(buffer[0] == 0xFF && buffer_pos > 2)
	{
		switch(buffer[1])
		{
		case 0x20: //set pid x
			readbuff(temp, buffer, 5, 2);
			pid[0].p = ((float)hascii2dec(temp, 5)/100);
			readbuff(temp, buffer, 5, 7);
			pid[0].i = ((float)hascii2dec(temp, 5)/100);
			readbuff(temp, buffer, 5, 12);
			pid[0].d = ((float)hascii2dec(temp, 5)/100);

			usart_put(0xFF);
			usart_put(0x20);
			dec2hascii((int)(pid[0].p*100), 5);
			dec2hascii((int)(pid[0].i*100), 5);
			dec2hascii((int)(pid[0].d*100), 5);
			usart_put(0x0A);

			break;
		case 0x21: //set pid y
			readbuff(temp, buffer, 5, 2);
			pid[1].p = ((float)hascii2dec(temp, 5)/100);
			readbuff(temp, buffer, 5, 7);
			pid[1].i = ((float)hascii2dec(temp, 5)/100);
			readbuff(temp, buffer, 5, 12);
			pid[1].d = ((float)hascii2dec(temp, 5)/100);

			usart_put(0xFF);
			usart_put(0x21);
			dec2hascii((int)(pid[1].p*100), 5);
			dec2hascii((int)(pid[1].i*100), 5);
			dec2hascii((int)(pid[1].d*100), 5);
			usart_put(0x0A);
			break;
		case 0x22: //set power

			readbuff(temp, buffer, 5, 2);
			iPower = hascii2dec(temp, 5);

			usart_put(0xFF);
			usart_put(0x22);
			dec2hascii(iPower, 5);
			usart_put(0x0A);

			break;

		case 0x23: //set angle
			readbuff(temp, buffer, 5, 2);
			dest_angle.x = ((float)hascii2dec(temp, 5)/100);
			readbuff(temp, buffer, 5, 7);
			dest_angle.y = ((float)hascii2dec(temp, 5)/100);
			readbuff(temp, buffer, 5, 12);
			dest_angle.z = ((float)hascii2dec(temp, 5)/100);

			usart_put(0xFF);
			usart_put(0x23);
			dec2hascii((int)(dest_angle.x*100), 5);
			dec2hascii((int)(dest_angle.y*100), 5);
			dec2hascii((int)(dest_angle.z*100), 5);
			usart_put(0x0A);
			break;


		case 0x24: //czytaj k¹ty

			usart_put(0xFF);
			usart_put(0x24);
			dec2hascii(fAnglesPos, 5);

			switch(buffer[2])
			{
			case 0x01:
				for(i = 0; i < fAnglesPos; i++)
					dec2hascii((int)(fAngles[i].x*100), 5);
			case 0x02:
				for(i = 0; i < fAnglesPos; i++)
					dec2hascii((int)(fAngles[i].y*100), 5);
			case 0x03:
				for(i = 0; i < fAnglesPos; i++)
					dec2hascii((int)(fAngles[i].z*100), 5);
			}

			usart_put(0x0A);

			break;


		}


	}

	buffer_pos = 0;
}

void USART2_IRQHandler()
{
	if(USART2->SR & USART_SR_RXNE)
	{
		uint8_t data = USART2->DR;
		switch(data)
		{
		case 0x01: // STARt
			TIM1->CR1 |= TIM_CR1_CEN;
			break;

		case 0x02: //STOP
			TIM1->CR1 &= ~TIM_CR1_CEN;
			motor_set(1, 0);
			motor_set(2, 0);
			motor_set(3, 0);
			motor_set(4, 0);
			break;

		case 0x03: //STATUS
			dec2hascii((int)(pid[0].p*100), 5);
			dec2hascii((int)(pid[0].i*100), 5);
			dec2hascii((int)(pid[0].d*100), 5);
			dec2hascii((int)(pid[1].p*100), 5);
			dec2hascii((int)(pid[1].i*100), 5);
			dec2hascii((int)(pid[1].d*100), 5);
			dec2hascii(iPower, 5);
			dec2hascii(ADCVal[0], 5);
			dec2hascii(ADCVal[1], 5);
			break;


		case 0x0A:
			buffer_parse();
			break;

		default:
			buffer[buffer_pos] = data;
			buffer_pos++;

			break;

		}



	}

}
