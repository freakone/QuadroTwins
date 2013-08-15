void regulation_init();
void TIM1_UP_IRQHandler();
float comp_filter(float newAngle, float newRate, float dt);
extern int iPower;
extern float x_angle;

typedef struct {

	float p;
	float i;
	float d;
	float dt;
	float it;

} PIDStruct;

PIDStruct pid;
