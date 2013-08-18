void regulation_init();
void TIM1_UP_IRQHandler();

typedef struct {

	float p;
	float i;
	float d;
	float last_error;
	float sum_error;

} PIDStruct;

PIDStruct pid[3];

typedef struct {

	float x;
	float y;
	float z;
} Axis;

Axis curr_angle;
Axis dest_angle;
extern int iPower;
extern Axis fAngles[500];
extern int fAnglesPos;
Axis getCurrentAngle(float dt);
float comp_filter(float newAngle, float newRate, float dt, int i);
void PID_Calc();
