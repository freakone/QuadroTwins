void regulation_init();

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
extern uint8_t armed;
extern float filterAngle[3];
Axis getCurrentAngle(float dt);
float comp_filter(float newAngle, float newRate, float dt, int i);
void PID_Calc(float dt);
void regulation_loop();
extern int temp;
