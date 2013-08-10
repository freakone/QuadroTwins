#include "cmsis_boot/stm32f10x.h"
#include "sensors.h"

void sensors_init()
{
	I2C2->CR1 |= I2C_CR1_SWRST;
	I2C2->CR1 &= ~I2C_CR1_SWRST;

	I2C2->TRISE = 37;               // limit slope
	I2C2->CCR = 160;               // setup speed (100kHz)
	I2C2->CR2 |= 36;      // config I2C2 module

	I2C2->CR1 |= I2C_CR1_PE;// enable peripheral

//accelero conf
	uint8_t acc_speed[] = {0x20, 151};
	i2c_write(acc, acc_speed, 2);

	uint8_t acc_high_res[] = {0x23, 8};
	i2c_write(acc, acc_high_res, 2);

	//gyro conf
	uint8_t gyro_speed[] = {0x20, 255};
	i2c_write(gyr, gyro_speed, 2);


}

void i2c_write(uint8_t address, uint8_t* data, uint32_t length)
{
	uint32_t dummy;

	I2C2->CR1 |= I2C_CR1_START;				// request a start
	while(!(I2C2->SR1 & I2C_SR1_SB));		// wait for start to finish
	dummy = I2C2->SR1;						// read of SR1 clears the flag
	I2C2->DR = address;						// transfer address
	while (!(I2C2->SR1 & I2C_SR1_ADDR));	// wait for address transfer
	dummy = I2C2->SR1;						// clear the flag
	dummy = I2C2->SR2;

	while (length--)							// transfer whole block
	{
		while (!(I2C2->SR1 & I2C_SR1_TXE));		// wait for DR empty
		I2C2->DR = *data++;						// transfer one byte, increment pointer
	}

	while (!(I2C2->SR1 & I2C_SR1_TXE) || !(I2C2->SR1 & I2C_SR1_BTF));
												// wait for bus not-busy
	I2C2->CR1 |= I2C_CR1_STOP;					// request a stop
}

void i2c_read( uint8_t adres, uint8_t reg_adres, uint8_t * dane, uint8_t len )
{
	uint32_t dummy;

	I2C2->CR1 |= I2C_CR1_START;
	while( !( I2C2->SR1 & I2C_SR1_SB ));
	I2C2->DR = adres;
	while( !( I2C2->SR1 & I2C_SR1_ADDR ));
	dummy = I2C2->SR2;
	while( !( I2C2->SR1 & I2C_SR1_TXE ));
	I2C2->DR = reg_adres;
	while( !( I2C2->SR1 & I2C_SR1_BTF ));
	I2C2->CR1 |= I2C_CR1_START;
	while( !( I2C2->SR1 & I2C_SR1_SB ));
	I2C2->DR = adres | 0x01;
	while( !( I2C2->SR1 & I2C_SR1_ADDR ));
	dummy = I2C2->SR2;

	while( len )
	{
	   if( len == 1 )
	       I2C2->CR1 &= ~I2C_CR1_ACK;

	   while( !( I2C2->SR1 & I2C_SR1_RXNE ));
	   *( dane++ ) = I2C2->DR;

	   len--;
	}

	I2C2->CR1 |= I2C_CR1_STOP;
}
