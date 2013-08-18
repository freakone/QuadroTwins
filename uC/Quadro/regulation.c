#include "cmsis_boot/stm32f10x.h"
#include "sensors.h"
#include <math.h>
#include "regulation.h"
#include "motors.h"

#define PI 3.14159265

int iPower = 0;
Axis fAngles[500];
int fAnglesPos = 0;
void regulation_init()
{
	pid[0].p = 1;
	pid[0].i = 0.1;
	pid[0].d = 0.01;

	iPower = iMotorMin;

	dest_angle.x = 0;
	dest_angle.y = 0;
	dest_angle.z = 0;

	TIM1->PSC = 7199;
	TIM1->ARR = 50;
	TIM1->DIER = TIM_DIER_UIE;
//	TIM1->CR1 = TIM_CR1_CEN;

	NVIC_EnableIRQ(TIM1_UP_IRQn);
}

void TIM1_UP_IRQHandler()
{
	if(TIM1->SR & TIM_SR_UIF)
	{
		sensors_read();
		curr_angle = getCurrentAngle(0.005);
		PID_Calc();


		if(fAnglesPos > 480)
			fAnglesPos = 0;

		fAngles[fAnglesPos] = curr_angle;
		fAnglesPos++;

		TIM1->SR &= ~TIM_SR_UIF;

	}

}

void PID_Calc()
{
	//X
	float error = dest_angle.x-curr_angle.x;

	float valx = pid[0].p * error + pid[0].d * (error - pid[0].last_error) + pid[0].i * pid[0].sum_error;
	pid[0].last_error = error;
	pid[0].sum_error += error;

	error = dest_angle.y-curr_angle.y;

	float valy = pid[1].p * error + pid[1].d * (error - pid[1].last_error) + pid[1].i * pid[1].sum_error;
	pid[1].last_error = error;
	pid[1].sum_error += error;

	int motor[] = {iPower, iPower, iPower, iPower};

	motor[0] += (int)valx;
	motor[1] += (int)valx;
	motor[2] -= (int)valx;
	motor[3] -= (int)valx;

	motor[0] += (int)valy;
	motor[1] -= (int)valy;
	motor[2] += (int)valy;
	motor[3] -= (int)valy;

	int i;
	for(i = 0; i < 4; i++)
	{
		if(motor[i] > iMotorMax)
			motor[i] = iMotorMax;
		else
		if(motor[i] < iMotorMin)
			motor[i] = iMotorMin;

		motor_set(i+1, motor[i]);
	}
}

Axis getCurrentAngle(float dt)
{
	Axis temp;

	float x_val, y_val, z_val, result,  x2, y2, z2, accel_angle;

	x_val = ((float)acc.x * acc.scale);
	y_val = ((float)acc.y * acc.scale);
	z_val = ((float)acc.z * acc.scale);

	x2 = (x_val*x_val);
	y2 = (y_val*y_val);
	z2 = (z_val*z_val);

	//X Axis
	result=sqrt(x2+z2);
	result=(y_val/result);
	accel_angle = atan(result)  * 180 / PI;
	temp.x = comp_filter(accel_angle, ((float)gyro.x * gyro.scale), dt, 0);

	//Y
	result=sqrt(y2+z2);
	result=(x_val/result);
	accel_angle = atan(result)  * 180 / PI;
	temp.y = comp_filter(accel_angle, ((float)gyro.y * gyro.scale), dt, 1);

	return temp;
}

float filterAngle[] = {0, 0, 0};
float comp_filter(float newAngle, float newRate, float dt, int i) {

	float filterTerm0;
	float filterTerm1;
	float filterTerm2=0;
	float timeConstant = 0.5;

	filterTerm0 = (newAngle - filterAngle[i]) * timeConstant * timeConstant;
	filterTerm2 += filterTerm0 * dt;
	filterTerm1 = filterTerm2 + ((newAngle - filterAngle[i]) * 2 * timeConstant) + newRate;
	filterAngle[i] = (filterTerm1 * dt) + filterAngle[i];

	return filterAngle[i]; // This is actually the current angle, but is stored for the next iteration
}

