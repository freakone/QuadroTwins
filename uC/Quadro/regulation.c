#include "cmsis_boot/stm32f10x.h"
#include "sensors.h"
#include <math.h>
#include "regulation.h"
#include "motors.h"

#define PI 3.14159265

int iPower = 0;
void regulation_init()
{
	pid.p = 1;
	pid.i = 0.1;
	pid.d = 0.01;
	pid.dt = 1;
	pid.it = 1;

	iPower = iMotorMin;

	TIM1->PSC = 7199;
	TIM1->ARR = 50;
	TIM1->DIER = TIM_DIER_UIE;
//	TIM1->CR1 = TIM_CR1_CEN;

	NVIC_EnableIRQ(TIM1_UP_IRQn);
}

float x_angle = 0;
float last_error = 0;
float sum_error = 0;
float angle_set = 0;

void TIM1_UP_IRQHandler()
{
	if(TIM1->SR & TIM_SR_UIF)
	{
		sensors_read();

		float x_val, y_val, z_val, result,  x2, y2, z2, accel_angle_x;

		x_val = ((float)acc.x * acc.scale);
		y_val = ((float)acc.y * acc.scale);
		z_val = ((float)acc.z * acc.scale);

		x2 = (x_val*x_val);
		y2 = (y_val*y_val);
		z2 = (z_val*z_val);

		   //X Axis
		result=sqrt(x2+z2);
		result=(y_val/result);
		accel_angle_x = atan(result)  * 180 / PI;
		x_angle = comp_filter(accel_angle_x, ((float)gyro.x * gyro.scale), 0.005);

		float error = angle_set-x_angle;

		float val = pid.p * error + pid.d * (error - last_error) + pid.i * sum_error;
		last_error = error;
		sum_error += error;

		int minusval = iPower - (int)val;
		if(minusval < iMotorMin)
			minusval = iMotorMin;

		int plusval = iPower + (int)val;

		if(plusval > iMotorMax)
			plusval = iMotorMax;

		motor_set(3, minusval);
		motor_set(4, minusval);

		motor_set(2, plusval);
		motor_set(1, plusval);

		TIM1->SR &= ~TIM_SR_UIF;

	}

}

float filterAngle = 0;
float filterTerm0;
float filterTerm1;
float filterTerm2;
float comp_filter(float newAngle, float newRate, float dt) {

	float timeConstant = 0.5;

	filterTerm0 = (newAngle - filterAngle) * timeConstant * timeConstant;
	filterTerm2 += filterTerm0 * dt;
	filterTerm1 = filterTerm2 + ((newAngle - filterAngle) * 2 * timeConstant) + newRate;
	filterAngle = (filterTerm1 * dt) + filterAngle;

	return filterAngle; // This is actually the current angle, but is stored for the next iteration
}

