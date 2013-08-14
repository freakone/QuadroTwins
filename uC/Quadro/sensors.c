#include "cmsis_boot/stm32f10x.h"
#include "sensors.h"

int sensors_error_flag = 0;
void sensors_init()
{
	i2c_config();

	gyro.address = 212;
	gyro.scale = 0.070;
	gyro.pid.p = 1;
	gyro.pid.i = 0.1;
	gyro.pid.d = 0.01;
	gyro.pid.dt = 1;

	acc.address = 48;
	acc.scale = 0.004;
	magneto.address = 28;

	//accelero conf
	i2c_write2(acc.address, 0x20, 151);
	i2c_write2(acc.address, 0x21, 0);
	i2c_write2(acc.address, 0x22, 8);
	i2c_write2(acc.address, 0x23, 40);
	i2c_write2(acc.address, 0x24, 64);

	//gyro conf
	i2c_write2(gyro.address, 0x20, 0x0F);
	i2c_write2(gyro.address, 0x21, 0x00);
	i2c_write2(gyro.address, 0x22, 0x00);// póki co pin Ÿle poprowadzony wiêc niepotrzebne
	i2c_write2(gyro.address, 0x23, 0x20);
	i2c_write2(gyro.address, 0x24, 0x00);

	//magneto conf
	i2c_write2(magneto.address, 0x11, 0x80);
	i2c_write2(magneto.address, 0x10, 0x01);




	AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI3_PB;
	AFIO->EXTICR[1] |= AFIO_EXTICR2_EXTI4_PC | AFIO_EXTICR2_EXTI5_PC;

	EXTI->IMR |= EXTI_IMR_MR3 | EXTI_IMR_MR4 | EXTI_IMR_MR5;
	EXTI->RTSR |= EXTI_RTSR_TR3 | EXTI_RTSR_TR4 | EXTI_RTSR_TR5;

	NVIC_EnableIRQ(EXTI3_IRQn);
	NVIC_EnableIRQ(EXTI4_IRQn);
	NVIC_EnableIRQ(EXTI9_5_IRQn);


}

int error_check()
{
	if((I2C2->SR1 & I2C_SR1_TIMEOUT) | (I2C2->SR1 & I2C_SR1_PECERR) | (I2C2->SR1 & I2C_SR1_AF) | (I2C2->SR1 & I2C_SR1_ARLO) |
			(I2C2->SR1 & I2C_SR1_BERR))
	{
		sensors_error_flag = 1;
		return 1;
	}
	return 0;

}

void i2c_config()
{
	I2C2->CR1 &= ~I2C_CR1_PE;
	I2C2->CR1 |= I2C_CR1_SWRST;
	I2C2->CR1 &= ~I2C_CR1_SWRST;

	I2C2->TRISE = 37;               // limit slope
	I2C2->CCR = 160;               // setup speed (100kHz)
	I2C2->CR2 |= 36;      // config I2C2 module

	I2C2->CR1 |= I2C_CR1_PE;// enable peripheral
}

void sensors_read()
{

	acc.x = (i2c_read2(acc.address, 0x29)&0xFF) << 8;
	acc.x |= (i2c_read2(acc.address, 0x28)&0xFF);
	acc.y = (i2c_read2(acc.address, 0x2B)&0xFF) << 8;
	acc.y |= (i2c_read2(acc.address, 0x2A)&0xFF);
	acc.z = (i2c_read2(acc.address, 0x2D)&0xFF) << 8;
	acc.z |= (i2c_read2(acc.address, 0x2C)&0xFF);

	gyro.x = (i2c_read2(gyro.address, 0x29)&0xFF) << 8;
	gyro.x |= (i2c_read2(gyro.address, 0x28)&0xFF);
	gyro.y = (i2c_read2(gyro.address, 0x2B)&0xFF) << 8;
	gyro.y |= (i2c_read2(gyro.address, 0x2A)&0xFF);
	gyro.z = (i2c_read2(gyro.address, 0x2D)&0xFF) << 8;
	gyro.z |= (i2c_read2(gyro.address, 0x2C)&0xFF);


	magneto.x = (i2c_read2(magneto.address, 0x01)&0xFF) << 8;
	magneto.x |= (i2c_read2(magneto.address, 0x02)&0xFF);
	magneto.y = (i2c_read2(magneto.address, 0x03)&0xFF) << 8;
	magneto.y |= (i2c_read2(magneto.address, 0x04)&0xFF);
	magneto.z = (i2c_read2(magneto.address, 0x05)&0xFF) << 8;
	magneto.z |= (i2c_read2(magneto.address, 0x06)&0xFF);

}

void EXTI3_IRQHandler(void)
{
	if(EXTI->PR & EXTI_PR_PR3) //AKCELERO
	{

		EXTI->PR |= EXTI_PR_PR3;
	}
}

void EXTI4_IRQHandler(void)
{
	if(EXTI->PR & EXTI_PR_PR4) //MAGNETO
	{
		EXTI->PR |= EXTI_PR_PR4;

	}
}
void EXTI9_5_IRQHandler(void)
{
	if(EXTI->PR & EXTI_PR_PR5)//GYRO
	{
		EXTI->PR |= EXTI_PR_PR5;

	}
}

void i2c_write2(uint8_t address, uint8_t reg, uint8_t data)
{
	uint8_t acc[] = {reg, data};
	i2c_write(address, acc, 2);

}

void i2c_write(uint8_t address, uint8_t* data, uint32_t length)
{
	uint32_t dummy;

	while(I2C2->SR2 & I2C_SR2_BUSY)
	{
		if(error_check())
		{
			i2c_config();
			return;
		}
	}

	I2C2->CR1 |= I2C_CR1_START;
	while(!(I2C2->SR1 & I2C_SR1_SB))
	{
		if(error_check())
		{
			i2c_config();
			return;
		}
	}
	dummy = I2C2->SR1;
	I2C2->DR = address;
	while (!(I2C2->SR1 & I2C_SR1_ADDR))
	{
		if(error_check())
		{
			i2c_config();
			return;
		}
	}
	dummy = I2C2->SR1;
	dummy = I2C2->SR2;

	while (length--)
	{
		while (!(I2C2->SR1 & I2C_SR1_TXE))
		{
			if(error_check())
			{
				i2c_config();
				return;
			}

		}
		I2C2->DR = *data++;
	}

	while (!(I2C2->SR1 & I2C_SR1_TXE) || !(I2C2->SR1 & I2C_SR1_BTF))
	{
		if(error_check())
		{
			i2c_config();
			return;
		}
	}

	I2C2->CR1 |= I2C_CR1_STOP;
}

uint8_t i2c_read2(uint8_t adres, uint8_t reg_adres)
{
	uint8_t dane[1];
	i2c_read(adres, reg_adres, &dane, 1);
	return dane[0];
}

void i2c_read( uint8_t adres, uint8_t reg_adres, uint8_t * dane, uint8_t len )
{
	uint32_t dummy;

	while(I2C2->SR2 & I2C_SR2_BUSY)
	{
		if(error_check())
		{
			i2c_config();
			return;
		}
	}

	I2C2->CR1 |= I2C_CR1_START;
	while( !( I2C2->SR1 & I2C_SR1_SB ))
	{
		if(error_check())
		{
			i2c_config();
			return;
		}
	}
	I2C2->DR = adres;
	while( !( I2C2->SR1 & I2C_SR1_ADDR ))
	{
		if(error_check())
		{
			i2c_config();
			return;
		}
	}
	dummy = I2C2->SR2;
	while( !( I2C2->SR1 & I2C_SR1_TXE ))
	{
		if(error_check())
		{
			i2c_config();
			return;
		}
	}
	I2C2->DR = reg_adres;
	while( !( I2C2->SR1 & I2C_SR1_BTF ))
	{
		if(error_check())
		{
			i2c_config();
			return;
		}
	}
	I2C2->CR1 |= I2C_CR1_START;
	while( !( I2C2->SR1 & I2C_SR1_SB ))
	{
		if(error_check())
		{
			i2c_config();
			return;
		}
	}
	I2C2->DR = adres | 0x01;
	while( !( I2C2->SR1 & I2C_SR1_ADDR ))
	{
		if(error_check())
		{
			i2c_config();
			return;
		}
	}
	dummy = I2C2->SR2;

	 I2C2->CR1 |= I2C_CR1_ACK;
	while( len )
	{
	   if( len == 1 )
	      I2C2->CR1 &= ~I2C_CR1_ACK;

	   while( !( I2C2->SR1 & I2C_SR1_RXNE ))
	   {
		   if(error_check())
		   	{
			   i2c_config();
		   		return;
		   	}
	   }
	   *( dane++ ) = I2C2->DR;

	   len--;
	}

	I2C2->CR1 |= I2C_CR1_STOP;
}
