#include "cmsis_boot/stm32f10x.h"
#include "sensors.h"
#include <math.h>
#include "regulation.h"
#include "motors.h"
#include "aparature.h"

#define PI 3.14159265

int iPower = 0;
uint8_t armed = 0;
void regulation_init()
{
	pid[0].p = 1.0;
	pid[0].i = 0.05;
	pid[0].d = 0.10;

	pid[1].p = 1.0;
	pid[1].i = 0.05;
	pid[1].d = 0.10;

	iPower = iMotorMin;

	dest_angle.x = 0;
	dest_angle.y = 0;
	dest_angle.z = 0;
}

int temp = 0;

void regulation_loop()
{
	sensors_read();
	curr_angle = getCurrentAngle(0.0031);

	if(pilot.throttle_up_down > 0x220 && armed == 0xFF)
	{
		iPower = iMotorMin + (pilot.throttle_up_down - 500)*2;
		PID_Calc(0.0031);

	}
	else
	{
		motor_set(1, 0);
		motor_set(2, 0);
		motor_set(3, 0);
		motor_set(4, 0);
	}
	//temp++;


			/*	usart_put(0xFF);
				usart_put(0x27);
				dec2hascii((int)(curr_angle.x*100), 8);
				dec2hascii((int)(gyro.x*gyro.scale*100), 8);
				usart_put(0x0A);
				temp = 0;
	*/
}

void PID_Calc(float dt)
{
	//X
	float error = dest_angle.x - curr_angle.x;
	pid[0].sum_error += error;
	float valx = (pid[0].p * error) - (pid[0].d * (gyro.x * gyro.scale)) + (pid[0].i * pid[0].sum_error);
	pid[0].last_error = error;


	/*error = dest_angle.y-curr_angle.y;

	float valy = pid[1].p * error + pid[1].d * (error - pid[1].last_error) + pid[1].i * pid[1].sum_error;
	pid[1].last_error = error;
	pid[1].sum_error += error;*/

	int motor[] = {iPower, iPower, iPower, iPower};

	motor[0] -= (int)valx;
	motor[1] -= (int)valx;
	motor[2] += (int)valx;
	motor[3] += (int)valx;

	/*motor[0] += (int)valy;
	motor[1] -= (int)valy;
	motor[2] += (int)valy;
	motor[3] -= (int)valy;*/

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

	y_val = ((float)acc.x * acc.scale);
	x_val = ((float)acc.y * acc.scale);
	z_val = ((float)acc.z * acc.scale);

	x2 = (x_val*x_val);
	y2 = (y_val*y_val);
	z2 = (z_val*z_val);

	//X Axis
	result=sqrt(y2+z2);
	result=(x_val/result);
	accel_angle = atan(result)  * 180 / PI;
	temp.x = comp_filter(accel_angle, ((float)gyro.x * gyro.scale), dt, 0);

	//Y
	result=sqrt(x2+z2);
	result=(y_val/result);
	accel_angle = atan(result)  * 180 / PI;
	temp.y = comp_filter(accel_angle, ((float)gyro.y * gyro.scale), dt, 1);

	return temp;
}

float filterAngle[] = {0, 0, 0};
float filterTerm2[] = {0, 0, 0};
float comp_filter(float newAngle, float newRate, float dt, int i) {

	float filterTerm0;
	float filterTerm1;

	float k = 0.3;

	filterTerm0 = (newAngle - filterAngle[i])*2*k;
	filterTerm2[i] +=  (newAngle - filterAngle[i])*k*k*dt;
	filterTerm1 = filterTerm2[i] + filterTerm0 + newRate;
	filterAngle[i] += (filterTerm1 * dt);

	return filterAngle[i]; // This is actually the current angle, but is stored for the next iteration
}

